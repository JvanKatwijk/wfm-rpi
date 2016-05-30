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
 */

#include	"fm-demodulator.h"
#include	"gui.h"

#include	"the-decoders.h"

#define	DCAlpha	0.0001
//
//	Just to play around a little, I implemented 5 common 
//	fm decoders. The main source of inspiration is found in
//	a Diploma Thesis "Implementation of FM demodulator Algorithms
//	on a High Performance Digital Signal Processor", especially
//	chapter 3.
	fm_Demodulator::fm_Demodulator (RadioInterface	*mr,
	                                int32_t	rateIn,
	                                DSPCOMPLEX	*Table,
	                                DSPFLOAT K_FM) {
int32_t	i;

	         this	-> rateIn	= rateIn;
	         this	-> Table	= Table;
	         this	-> K_FM		= K_FM;

	         theDecoder		= new decoderBase ();
	         connect (this, SIGNAL (showDecoder (const QString &)),
	                  mr, SLOT (showDecoder (const QString &)));
	         setDecoder (FM4DECODER);

	         fm_afc			= 0;
		 fm_cvt			= 0.90 * (rateIn / (M_PI * 150000));
}

		fm_Demodulator::~fm_Demodulator (void) {
	delete theDecoder;
}

void		fm_Demodulator::setDecoder	(int8_t nc) {
QString	s;
	this	-> selectedDecoder = nc;
	delete theDecoder;

	switch (selectedDecoder) {
	   default:
	   case FM1DECODER:
	      theDecoder	= new decoder_1 ();
	      s =	QString ("Difference based"); break;
	   case FM2DECODER:
	      theDecoder	= new decoder_2 ();
	      s =	QString ("Complex Baseband Delay"); break;
	   case FM3DECODER:
	      theDecoder	= new decoder_3 ();
	      s =	QString ("Mixed Demodulator"); break;
	   case FM4DECODER:
	      theDecoder	= new decoder_4 (rateIn, Table);
	      s = 	QString ("Pll decoder"); break;
	   case FM5DECODER:
	      theDecoder	= new decoder_5 ();
	      s	= 	QString ("Real Baseband Delay"); break;
	}
	showDecoder (s);
}

DSPFLOAT	fm_Demodulator::demodulate (DSPCOMPLEX z) {
float	res = theDecoder -> decode (z);

	fm_afc	= (1 - DCAlpha) * fm_afc + DCAlpha * res;
	res	= (res - fm_afc) * fm_cvt;
	res	/= K_FM;
	return res;
}

DSPFLOAT	fm_Demodulator::get_dcComponent (void) {
	return fm_afc;
}

