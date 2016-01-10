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
#include	"Xtan2.h"
#include	"gui.h"

//
//	Just to play around a little, I implemented 5 common 
//	fm decoders. The main source of inspiration is found in
//	a Diploma Thesis "Implementation of FM demodulator Algorithms
//	on a High Performance Digital Signal Processor", especially
//	chapter 3.
	fm_Demodulator::fm_Demodulator (RadioInterface	*mr,
	                                int32_t	rateIn,
	                                SinCos	*mySinCos,
	                                DSPFLOAT K_FM) {
int32_t	i;

	         this	-> rateIn	= rateIn;
	         this	-> mySinCos	= mySinCos;
	         this	-> K_FM		= K_FM;

	         connect (this, SIGNAL (showDecoder (const QString &)),
	                  mr, SLOT (showDecoder (const QString &)));
	         setDecoder (FM4DECODER);
	
	         this	-> max_freq_deviation =
	                                        0.95 * (0.5 * rateIn);
	         myfm_pll		= new pll (0, 1.1, mySinCos);
	         ArcsineSize		= 4 * 8192;
	         Arcsine		= new DSPFLOAT [ArcsineSize];
	         for (i = 0; i < ArcsineSize; i ++)
	            Arcsine [i] = asin (2.0 * i / ArcsineSize - 1.0) / 2.0;
	         Imin1			= 1;
	         Qmin1			= 1;
	         Imin2			= 1;
	         Qmin2			= 1;
	         oldZ			= DSPCOMPLEX (0, 0);
	         fm_afc			= 0;
	         fm_cvt			= 1.0;
		 fm_cvt			= 0.50 * (rateIn / (M_PI * 150000));
}

		fm_Demodulator::~fm_Demodulator (void) {
	delete	Arcsine;
	delete	myfm_pll;
}

void		fm_Demodulator::setDecoder	(int8_t nc) {
QString	s;
	this	-> selectedDecoder = nc;
	switch (selectedDecoder) {
	   default:
	   case FM1DECODER:
	      s =	QString ("Difference based"); break;
	   case FM2DECODER:
	      s =	QString ("Complex Baseband Delay"); break;
	   case FM3DECODER:
	      s =	QString ("Mixed Demodulator"); break;
	   case FM4DECODER:
	      s = 	QString ("Pll decoder"); break;
	   case FM5DECODER:
	      s	= 	QString ("Real Baseband Delay"); break;
	}
	showDecoder (s);
}

DSPFLOAT	fm_Demodulator::demodulate (DSPCOMPLEX z) {
DSPFLOAT	res;
DSPFLOAT	I, Q;
#define	DCAlpha	0.0001
//#define	DCAlpha	0.000001
	if (jan_abs (z) <= 0.001)
	   I = Q = 0.001;	// do not make these 0 too often
	else { 
	   I = real (z) / abs (z);
	   Q = imag (z) / abs (z);
	}

	z	= DSPCOMPLEX (I, Q);
	switch (selectedDecoder) {
	   default:
	   case FM1DECODER:
	      res	= Imin1 * (Q - Qmin2) - Qmin1 * (I - Imin2);
	      res	/= Imin1 * Imin1 + Qmin1 * Qmin1;
	      Imin2	= Imin1;
	      Qmin2	= Qmin1;
	      fm_afc	= (1 - DCAlpha) * fm_afc + DCAlpha * res;
	      res	= (res - fm_afc) * fm_cvt;
	      res	/= K_FM;
	      break;

	   case FM2DECODER:
	      res	= arg (z * DSPCOMPLEX (Imin1, - Qmin1));
	      fm_afc	= (1 - DCAlpha) * fm_afc + DCAlpha * res;
	      res	= (res - fm_afc) * fm_cvt;
	      res	/= K_FM;
	      break;

	   case FM3DECODER:
	      res	= myAtan. atan2 (Q * Imin1- I * Qmin1,
	                                 I * Imin1 + Q * Qmin1);
	      fm_afc	= (1 - DCAlpha) * fm_afc + DCAlpha * res;
	      res	= (res - fm_afc) * fm_cvt;
	      res	/= K_FM;
	      break;
//
	   case FM4DECODER:
	      myfm_pll	-> getPilotPhase (z);
//	      myfm_pll	-> do_pll (z);
//	lowpass the NCO frequency term to get a DC offset
	      fm_afc	= (1 - DCAlpha) * fm_afc +
	                   DCAlpha * myfm_pll -> getPhaseIncr ();
	      res	= (myfm_pll -> getPhaseIncr () - fm_afc) * fm_cvt;
	      res	/= K_FM;
	      break;

	   case FM5DECODER:
	      res	= (Imin1 * Q - Qmin1 * I + 1.0) / 2.0;
	      res	= Arcsine [(int)(res * ArcsineSize)];
	      fm_afc	= (1 - DCAlpha) * fm_afc + DCAlpha * res;
	      res	= (res - fm_afc) * fm_cvt;
	      res	/= K_FM;
	      break;
	}
//
//	and shift ...
	Imin2	= Imin1;
	Qmin2	= Qmin2;
	Imin1	= I;
	Qmin1	= Q;
	oldZ	= z;
	return res;
}

DSPFLOAT	fm_Demodulator::get_dcComponent (void) {
	return fm_afc;
}

