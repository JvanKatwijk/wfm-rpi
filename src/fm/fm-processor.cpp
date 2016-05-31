#
/*
 *    Copyright (C) 2010, 2011, 2012
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Programming
 *
 *    This file is part of the SDR-J program suite.
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
 */
#include	"fm-processor.h"
#include	"gui.h"
#include	"fm-constants.h"
#include	"virtual-input.h"
#include	"rds-decoder.h"
#include	"fm-decoder.h"

#define	AUDIO_FREQ_DEV_PROPORTION 0.85f

//
//	Note that no decimation done as yet: the samplestream is still
//	full speed
	fmProcessor::fmProcessor (virtualInput		*vi,
	                          RadioInterface	*RI,
	                          int32_t		inputRate,
	                          int32_t		fmRate,
	                          RingBuffer<DSPCOMPLEX> *audioSamples) {
	running				= false;
	this	-> myRig		= vi;
	this	-> myRadioInterface	= RI;
	this	-> inputRate		= inputRate;
	this	-> fmRate		= fmRate;
	this	-> decimatingScale	= inputRate / fmRate;

	this	-> peakLevel		= -100;
	this	-> peakLevelcnt		= 0;
	this	-> max_freq_deviation	= 0.95 * (0.5 * fmRate);
	this	-> norm_freq_deviation	= 0.5 * max_freq_deviation;

//	Since data is coming with a pretty high rate, we need to filter
//	and decimate in an efficient way. We have an optimized
//	decimating filter (optimized or not, ot takes quite some
//	cycles when entering with high rates)
//	So, what we do here is reduce the speed and delegate the
//	hard word to others
	decimatingFilter	= new DecimatingFIR (9,
	                                             fmRate / 2,
	                                             inputRate,
	                                             decimatingScale);
	my_rdsDecoder		= new rdsDecoder (myRadioInterface,
	                                          fmRate / RDS_DECIMATOR);
	my_fmDecoder		= new fmDecoder	(myRadioInterface,
	                                         audioSamples,
	                                         fmRate,
	                                         my_rdsDecoder);
}

	fmProcessor::~fmProcessor (void) {
	if (running && isRunning ()) {
	   running	= false;
	   while (!isFinished ())
	      usleep (100);
	}
	my_fmDecoder -> stop ();
	fprintf (stderr, "fmprocessor is gestopt\n");
	delete	decimatingFilter;
	delete	my_rdsDecoder;
	delete	my_fmDecoder;
	fprintf (stderr, "end of delete fm processor\n");
}

//	We have a separate thread for the fm processing

#define	bufferSize	16384
void	fmProcessor::run (void) {
DSPCOMPLEX	dataBuffer	[bufferSize];
DSPCOMPLEX	passBuffer	[4096];
int16_t		passPointer	= 0;

	fprintf (stderr, "fmprocessor gestart\n");
	running	= true;		// will be set to false elsewhere
	try {
	   int16_t i;
	   DSPCOMPLEX v;
	   while (running) {
	      while (running && (myRig -> Samples () < bufferSize)) 
	         usleep (100);	// should be enough

	      if (!running)
	         throw (22);
//
//	This is what we get in, first thing: decimating
//	which - by the way - is a pretty resource consuming operation
	      int32_t amount = myRig -> getSamples (dataBuffer, bufferSize);
	      for (i = 0; i < amount; i ++) {
	         v =  DSPCOMPLEX (real (dataBuffer [i]) * 100,
	                          imag (dataBuffer [i]) * 100);
	         if (!decimatingFilter -> Pass (v, &v))
	            continue;
//
//	for the audiogain correction
//	we are now on a lower rate
	         if (abs (v) > peakLevel)
	            peakLevel = abs (v);
	         if (++peakLevelcnt >= fmRate / 2) {
	            DSPFLOAT	ratio	= 
	                          max_freq_deviation / norm_freq_deviation;
	            if (peakLevel > 0)
	               this -> audioGain	= 
	                  (ratio / peakLevel) / AUDIO_FREQ_DEV_PROPORTION;
	            if (audioGain <= 0.05)
	               audioGain = 0.05;
	            audioGain	= 0.99 * audioGainAverage + 0.01 * audioGain;
	            audioGainAverage = audioGain;
	            peakLevelcnt	= 0;
	            peakLevel		= -100;
	            emit new_audioGain (audioGain);
	         }
//
//	decoding will be done in a separate thread
	         passBuffer [passPointer ++] = v;
	         if (passPointer >= 4096) {
	            my_fmDecoder -> doDecode (passBuffer, 4096);
	            passPointer = 0;
	         }
//
	      } // end main loop
	   } // end "running" loop
	fprintf (stderr, "normal termination\n");
	}
	catch (int e) {;}
}

bool	fmProcessor::isLocked (void) {
	if (!running)
	   return false;
	return my_fmDecoder -> isLocked ();
}

void	fmProcessor::set_rdsMode (bool m) {
	my_fmDecoder -> set_rdsModus (m);
}

void	fmProcessor::reset_rds	(void) {
	if (!running)
	   return;
	my_rdsDecoder	-> reset ();
}

void	fmProcessor::set_fmMode (bool m) {
	if (!running)
	   return;
	my_fmDecoder	-> set_fmMode (m);
}

void	fmProcessor::set_fmDecoder (uint8_t d) {
	if (!running)
	   return;
	my_fmDecoder	-> set_fmDemodulator (d);
}

