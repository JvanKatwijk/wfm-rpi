#
/*
 *    Copyright (C)  2014
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Programming
 *
 *    This file is part of the SDR-J.
 *    Many of the ideas as implemented in SDR-J are derived from
 *    other work, made available through the GNU general Public License. 
 *    All copyrights of the original authors are recognized.
 *
 *    SDR-J is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    SDR-J is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with SDR-J; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *	parts are based on/inspired by CuteSDR (Moe Wheatley)
 *	
 *	all Copyrights are gratefully acknowledged
 */

#include	<stdlib.h>
#include	<stdio.h>
#include	"rds-decoder.h"
#include	"radio.h"
#include	"iir-filters.h"
#include	"sincos.h"

const DSPFLOAT	RDS_BITCLK_HZ =	1187.5;
/*
 *	RDS is a bpsk-like signal, with a baudrate 1187.5
 *	on a carrier of  3 * 19 k.
 *	48 cycles per bit, 1187.5 bits per second.
 *	With a reduced sample rate of 48K this would mean
 *	48000 / 1187.5 samples per bit, i.e. between 40 and 41
 *	samples per bit.
 *	Notice that mixing to zero IF has been done
 *	so we are handling a baseband signal
 */
	rdsDecoder::rdsDecoder (RadioInterface *myRadio,
				int32_t		rate) {
int16_t	i;
int16_t	length;
float	synchronizerSamples;
int16_t	symbolCeiling;

	this	-> myRadioInterface	= myRadio;
	this	-> sampleRate	= rate;
	prevBit			= 0;
//
//	The matched filter is a borrowed from the cuteRDS, who in turn
//	borrowed it from course material 
//      http://courses.engr.illinois.edu/ece463/Projects/RBDS/RBDS_project.doc
//	Note that the formula down has a discontinuity for
//	two values of x, we better make the symbollength odd
//
//	Filterlength should be twice the "bit"length
	synchronizerSamples	= sampleRate / (DSPFLOAT)RDS_BITCLK_HZ;
	symbolCeiling		= ceil (synchronizerSamples);
	length			= (symbolCeiling & ~01) + 1;
	rdsfilterSize		= 2 * length + 1;
	rdsBuffer		= new DSPFLOAT [rdsfilterSize];
	memset (rdsBuffer, 0, rdsfilterSize * sizeof (DSPFLOAT));
	ip			= 0;
	rdsKernel		= new DSPFLOAT [rdsfilterSize];
	rdsKernel [length] = 0;
	for (i = 1; i <= length; i ++) {
	   DSPFLOAT x = ((DSPFLOAT)i) / rate * RDS_BITCLK_HZ;
	   rdsKernel [length + i] =  0.75 * cos (4 * M_PI * x) *
					    ((1.0 / (1.0 / x - 64 * x)) -
					    ((1.0 / (9.0 / x - 64 * x))) );
	   rdsKernel [length - i] = - 0.75 * cos (4 * M_PI * x) *
					    ((1.0 / (1.0 / x - 64 * x)) -
					    ((1.0 / (9.0 / x - 64 * x))) );
	}
//
//	The matched filter is followed by a pretty sharp filter
//	to eliminate all remaining "noise".
//	Note that we are only interested in the envelope, not the
//	phase. That is why we can use a very narrow IIR
	sharpFilter		= new BandPassIIR (5, RDS_BITCLK_HZ - 10,
	                                              RDS_BITCLK_HZ + 10,
	                                              rate, S_BUTTERWORTH);
	rdsLastSyncSlope	= 0;
	rdsLastSync		= 0;
	rdsLastData		= 0;
	previousBit		= 0;

	my_rdsGroup		= new RDSGroup		();
	my_rdsGroup		->  clear ();
	my_rdsBlockSync		= new rdsBlockSynchronizer (myRadioInterface);
	my_rdsBlockSync		-> setFecEnabled (true);
	my_rdsGroupDecoder	= new rdsGroupDecoder	(myRadioInterface);

	connect (this, SIGNAL (setCRCErrors (int)),
		 myRadioInterface, SLOT (setCRCErrors (int)));
	connect (this, SIGNAL (setSyncErrors (int)),
		 myRadioInterface, SLOT (setSyncErrors (int)));
}

	rdsDecoder::~rdsDecoder (void) {
	delete		my_rdsGroupDecoder;
	delete		my_rdsGroup;
	delete		my_rdsBlockSync;
	delete		rdsKernel;
	delete		rdsBuffer;
	delete		sharpFilter;
}

void	rdsDecoder::reset	(void) {
	my_rdsGroupDecoder	-> reset ();
}

DSPFLOAT	rdsDecoder::Match	(DSPFLOAT v) {
int16_t		i;
DSPFLOAT	tmp = 0;

	rdsBuffer [ip] = v;
	for (i = 0; i < rdsfilterSize; i ++) {
	   int16_t index = (ip - i);
	   if (index < 0)
	      index += rdsfilterSize;
	   tmp += rdsBuffer [index] * rdsKernel [i];
	}

	ip = (ip + 1) % rdsfilterSize;
	return tmp;
}
/*
 *	Signal (i.e. "v") is already downconverted and lowpass filtered
 *	when entering this stage.
 */

static inline
int	sign (float s) {
	return s < 0 ? -1 : 1;
}

void	rdsDecoder::doDecode (DSPFLOAT v) {
DSPFLOAT	rdsMag;
DSPFLOAT	rdsSlope	= 0;
bool		bit;

	v		= Match (v);
	rdsMag		= sharpFilter	-> Pass (v * v);
	rdsSlope	= rdsMag - rdsLastSync;
	rdsLastSync	= rdsMag;
	if ((sign (rdsSlope) == -1) && (sign (rdsLastSyncSlope) == 1)) {
//	top of the sine wave: get the data
	   bit = rdsLastData >= 0;
	   processBit (bit ^ previousBit);
	   previousBit = bit;
	}

	rdsLastData		= v;
	rdsLastSyncSlope	= rdsSlope;
	my_rdsBlockSync -> resetResyncErrorCounter ();
}

void	rdsDecoder::processBit (bool bit) {
	switch (my_rdsBlockSync -> pushBit (bit, my_rdsGroup)) {
	   case rdsBlockSynchronizer::RDS_WAITING_FOR_BLOCK_A:
	      break;		// still waiting in block A

	   case rdsBlockSynchronizer::RDS_BUFFERING:
	      break;	// just buffer

	   case rdsBlockSynchronizer::RDS_NO_SYNC:
//	      resync if the last sync failed
	      setSyncErrors (my_rdsBlockSync -> getNumSyncErrors ());
	      my_rdsBlockSync -> resync ();
	      break;

	   case rdsBlockSynchronizer::RDS_NO_CRC:
	      setCRCErrors (my_rdsBlockSync -> getNumCRCErrors ());
	      my_rdsBlockSync -> resync ();
	      break;

	   case rdsBlockSynchronizer::RDS_COMPLETE_GROUP:
	      my_rdsGroupDecoder -> decode (my_rdsGroup);
	      my_rdsGroup -> clear ();
	      break;
	}
}

