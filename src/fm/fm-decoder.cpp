#
/*
 *    Copyright (C) 2015
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
 *
 *    This file is part of the SDR-J program suite.
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
#include	"fm-decoder.h"
#include	"radio.h"
#include	"fm-constants.h"
#include	"fm-demodulator.h"
#include	"fft-filters.h"
#include	"fir-filters.h"
#include	"rds-decoder.h"
#include	"pllC.h"

#define	PILOT_FREQUENCY		19000
#define	LRDIFF_FREQUENCY	(2 * PILOT_FREQUENCY)
#define	RDS_FREQUENCY		(3 * PILOT_FREQUENCY)
#define	OMEGA_DEMOD		(2 * M_PI / currentRate)
#define	OMEGA_PILOT		PILOT_FREQUENCY * OMEGA_DEMOD
#define	OMEGA_RDS		RDS_FREQUENCY * OMEGA_DEMOD
#define	PILOT_DELAY		(FFT_SIZE - PILOTFILTER_SIZE) * OMEGA_PILOT

//	
//	In order to spread the load of fm processing over more than a single
//	thread we have 
//	- fmprocessor running in its thread, it will receive the
//	samples and decimate them
//	- fmdecoder running in its thread, it will decode the
//	fm signal on the lower rate return audio

	fmDecoder::fmDecoder (RadioInterface	*mr,
	                      RingBuffer<DSPCOMPLEX> *audioBuffer,
	                      int32_t		currentRate,
	                      rdsDecoder	*myRdsDecoder) {
int	i;
	connect (this, SIGNAL (audiosamplesAvailable (int)),
	         mr, SLOT (audiosamplesAvailable (int)));
	connect (this,  SIGNAL (showLocked (bool, float)),
	         mr, SLOT (showLocked (bool, float)));
	this	-> myRadioInterface	= mr;
	this	-> audioBuffer		= audioBuffer;
	this	-> currentRate		= currentRate;
	this	-> my_rdsDecoder	= myRdsDecoder;

	rdsModus		= false;
	fmModus			= FM_STEREO;	// default
	sampleBuffer		= new RingBuffer<DSPCOMPLEX> (8 * 32768);
	newDemodulator		= 0;
	Table			= new DSPCOMPLEX [currentRate];
	for (i = 0; i < currentRate; i ++)
	   Table [i] = DSPCOMPLEX (cos (2 * M_PI * i / currentRate),
	                           sin (2 * M_PI * i / currentRate));
	pilotBandFilter		= new fftFilter  (FFT_SIZE,
	                                          PILOTFILTER_SIZE);
	pilotBandFilter		-> setBand (PILOT_FREQUENCY - PILOT_WIDTH / 2,
		                            PILOT_FREQUENCY + PILOT_WIDTH / 2,
		                            currentRate);
//
//	we really need to get rid of the rubbish
	lrdiffFilter		= new lowpassFIR (5, 15000, currentRate);
	lrplusFilter		= new lowpassFIR (5, 15000, currentRate);
	rdsLowPassFilter	= new fftFilter (FFT_SIZE,
	                                         RDSLOWPASS_SIZE);
	rdsLowPassFilter	-> setLowPass (RDS_WIDTH, currentRate);
//
//	the constant K_FM is still subject to many questions
	DSPFLOAT	F_G	= 0.65 * currentRate / 2; // highest freq in message
	DSPFLOAT	Delta_F	= 0.90 * currentRate / 2;
	DSPFLOAT	B_FM	= 2 * (Delta_F + F_G);
	K_FM			= 2 * M_PI / currentRate * B_FM;

	theDemodulator		= new fm_Demodulator (mr,
	                                              currentRate,
	                                              Table,
	                                              K_FM);

	theHilbertFilter	= new HilbertFilter (HILBERT_SIZE,
	                                             currentRate / 2 - 1,
	                                             currentRate);
	rdsBandFilter		= new fftFilter (FFT_SIZE,
	                                         RDSBANDFILTER_SIZE);
	rdsBandFilter		-> setBand (RDS_FREQUENCY - RDS_WIDTH / 2,
	                                    RDS_FREQUENCY + RDS_WIDTH / 2,
	                                    currentRate);
	rds_plldecoder		= new pllC (currentRate,
	                                    0,
	                                    0 - 100,
	                                    0 + 100,
	                                    50,
	                                    Table);
	pilotRecover		= new pllC (currentRate,
	                                    PILOT_FREQUENCY,
	                                    PILOT_FREQUENCY - 100,
	                                    PILOT_FREQUENCY + 100,
	                                    10,
	                                    Table);
	current_rdsPhase	= 0;
	start ();
}

	fmDecoder::~fmDecoder (void) {
	fprintf (stderr, "fm-decoder wordt deleted\n");
	running = false;
	while (!isFinished ())
	   usleep (100);
	fprintf (stderr, "fm-decoder is gestopt\n");
	
	delete	rdsBandFilter;
	delete	rdsLowPassFilter;
	delete	theHilbertFilter;
	delete	rds_plldecoder;
	delete	theDemodulator;
	delete	sampleBuffer;
	delete	pilotBandFilter;
	delete	pilotRecover;
	delete[] Table;
}

void	fmDecoder::stop	(void) {
	running	= false;
	while (!isFinished ())
	   usleep (100);
	fprintf (stderr, "fm-decoder is gestopt\n");
}

void	fmDecoder::doDecode (DSPCOMPLEX *v, int16_t size) {
	sampleBuffer -> putDataIntoBuffer (v, size);
}

void	fmDecoder::set_fmMode (uint8_t mode) {
	fmModus		= mode ? FM_STEREO : FM_MONO;
}

void	fmDecoder::set_fmDemodulator (uint8_t m) {
	newDemodulator = m;
}


#define	SIZE	(16384)
void	fmDecoder::run (void) {
DSPCOMPLEX	fmData [SIZE];
DSPCOMPLEX	audioSample;
float		rdsSample;
int16_t		decimationCount	= 0;
int32_t		totalAmount	= 0;
DSPCOMPLEX	localBuffer [2048];

	running	= true;
	try {
	   int16_t i;
	   int16_t	localBufferp	= 0;
//
//	we get SIZE samples in in app SIZE * 1000 / currentRate msec
//	for 8192 and 192000 that means app 50 msec
	   while (running) {
	      while (running &&
	         sampleBuffer -> GetRingBufferReadAvailable () < SIZE)
	         usleep (20000);
	      if (!running)
	        throw (22);
//
//	Someone might have requested a different demodulator
	      if (newDemodulator != 0)
	         theDemodulator -> setDecoder (newDemodulator);
	      newDemodulator = 0;
	      int16_t amount = sampleBuffer -> getDataFromBuffer (fmData, SIZE);
	      
	      if (fmModus == FM_MONO) {
	         for (i = 0; i < amount; i ++) {
	            do_mono (fmData [i], &audioSample, &rdsSample);

//	It is assumed that the gui thread will deal with filtering etc
	            localBuffer [localBufferp ++] = audioSample;
	            if (localBufferp >= 2048) {
	               audioBuffer -> putDataIntoBuffer (localBuffer, 2048);
	               emit audiosamplesAvailable (2048);
	               localBufferp = 0;
	            }

	            if ((rdsModus)) {
	               if (++decimationCount >= RDS_DECIMATOR) {
	                  my_rdsDecoder -> doDecode (rdsSample);
	                  decimationCount = 0;
	               }
	            }
	         }
	      }
	      else {		// must be stereo!!
	         for (i = 0; i < amount; i ++) {
	            do_stereo (fmData [i], &audioSample, &rdsSample);

//	It is assumed that the gui thread will deal with filtering etc
	            localBuffer [localBufferp ++] = audioSample;
	            if (localBufferp >= 2048) {
	               audioBuffer -> putDataIntoBuffer (localBuffer, 2048);
	               emit audiosamplesAvailable (2048);
	               localBufferp = 0;
	            }

	            if ((rdsModus)) {
	               if (++ decimationCount >= RDS_DECIMATOR) {
	                  my_rdsDecoder -> doDecode (rdsSample);
	                  decimationCount = 0;
	               }
	            }
	         }
	      }
	      totalAmount += amount;
	      if (totalAmount > currentRate) {
	         totalAmount = 0;
	         showLocked (isLocked (), 
	                     theDemodulator -> get_dcComponent ());
	      }
	   }
	}
	catch (int e) {;}
}
//
//	Processor for Mono
void	fmDecoder::do_mono (DSPCOMPLEX		in,
	                    DSPCOMPLEX		*audioOut,
	                    float		*rdsValue) {
DSPCOMPLEX	rdsBase;
DSPFLOAT	demod	= theDemodulator -> demodulate (in);

	*audioOut	= DSPCOMPLEX (demod, demod);
//
//	fully inspired by cuteSDR, we try to decode the rds stream
//	by simply am decoding it (after creating a decent complex
//	signal by Hilbert filtering)

	rdsBase = theHilbertFilter -> Pass (5 * demod, 5 * demod);
	rdsBase	= rdsBandFilter -> Pass (rdsBase);
//
//	from 0 .. 2 * M_PI to 0 .. currentRate
	int32_t thePhase = current_rdsPhase / (2 * M_PI) * currentRate;
	rdsBase	*= Table [thePhase];
	current_rdsPhase	-= OMEGA_RDS;
	if (current_rdsPhase < 0)
	   current_rdsPhase += 2 * M_PI;

	rds_plldecoder -> do_pll (rdsBase);
	*rdsValue = 5 * imag (rds_plldecoder -> getDelay ());
	*rdsValue	= rdsLowPassFilter -> Pass (*rdsValue);
}

void	fmDecoder::do_stereo (DSPCOMPLEX	in,
	                      DSPCOMPLEX	*audioOut,
	                      DSPFLOAT		*rdsValue) {

float	LRPlus	= 0;
DSPCOMPLEX	LRDiff;
DSPCOMPLEX	pilot;
DSPCOMPLEX	rdsBase;
float		currentPilotPhase;


//	first step: get the demodulated signal
//
//	It consists of LRPlus, pilot, LRDiff and rds components
	DSPFLOAT demod		= theDemodulator  -> demodulate (in);

	LRPlus			= demod;
	rdsBase	=
	pilot	=
	LRDiff	= theHilbertFilter	-> Pass (demod, demod);
/*
 *	get the phase for the "carrier to be inserted" right
 */
	pilot			= pilotBandFilter -> Pass (pilot);
	pilotRecover		-> do_pll (pilot);
	currentPilotPhase	= pilotRecover -> getNco ();

//	for the actual phase we should take into account
//	the delay caused by the FIR bandfilter
	currentPilotPhase	+= PILOT_DELAY;
//
//	shift the LRDiff signal down to baseband	38Khz
	int32_t thePhase	= currentPilotPhase / (2 * M_PI) * currentRate;
	LRDiff  *= conj (Table [(2 * thePhase) % currentRate]);

//	get rid of junk
	LRDiff			= lrdiffFilter	-> Pass (LRDiff);
//	.... and for the LplusR as well
	LRPlus			= lrplusFilter	-> Pass (LRPlus);
	float v			= imag (LRDiff) - real (LRDiff);
	*audioOut		= DSPCOMPLEX (LRPlus, 2 * v);
//
//	shift the rds signal to baseband and filter it 57 Khz
	rdsBase	*= conj (Table [(3 * thePhase) % currentRate]);
	*rdsValue = 10 * imag (rdsLowPassFilter -> Pass (rdsBase));
}

bool	fmDecoder::isLocked (void) {
	if (!running)
	   return false;
	return ((fmModus == FM_STEREO) && (pilotRecover -> isLocked ()));
}

void	fmDecoder::set_rdsModus (bool m) {
	rdsModus	= m;
}

