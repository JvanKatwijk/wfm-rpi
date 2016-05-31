#
/*
 *
 *	Copyright (C) 2004, 2005, 2006, 2007 by
 *	Frank Brickle, AB2KT and Bob McGwier, N4HY
 *	The authors can be reached by email at
 *	ab2kt@arrl.net or rwmcgwier@comcast.net
 *	or by paper mail at
 *	The DTTS Microwave Society
 *	6 Kathleen Place
 *	Bridgewater, NJ 08807
 *
 *	Adapted for the SDR-J fm receiver
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair programming
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
 */
#include	"pllC.h"
//
//	"rate"	is the samplerate
//	"lofreq" and "hifreq" the frequencies (in Hz) where the lock is 
//	kept in between,
//	"bandwidth" the bandwidth of the signal to be received

		pllC::pllC (int32_t	rate,
	                    DSPFLOAT	freq,
	                    DSPFLOAT	lofreq,
	                    DSPFLOAT	hifreq,
	                    DSPFLOAT	bandwidth,
	                    DSPCOMPLEX	*Table) {
DSPFLOAT	omega	= 2.0 * M_PI / rate;
float		eta	= 0.2f;

	this	-> rate	= rate;
	phaseIncr	= freq * omega;		// this will change during runs
	NcoLLimit	= lofreq * omega;	// boundary for changes
	NcoHLimit	= hifreq * omega;
	pll_Alpha	= eta * bandwidth * omega; // pll bandwidth
	pll_Beta	= (pll_Alpha * pll_Alpha) / 2.0; // 2nd order term
	this	-> Table	= Table;
	NcoPhase	= 0;
	phaseError	= 0;
	locked		= false;
}
//
		pllC::~pllC (void) {
}
//

void		pllC::do_pll (DSPCOMPLEX signal) {
DSPCOMPLEX	refSignal;

	locked		= false;	// unless proven otherwise
	refSignal	= conj (Table [int (NcoPhase / (2 * M_PI) * rate)]);
	pll_Delay	= signal * refSignal;
	phaseError	= myAtan. atan2 (imag (pll_Delay), real (pll_Delay));

	phaseIncr	+= pll_Beta * phaseError;
	if (phaseIncr < NcoLLimit)
	   phaseIncr = NcoLLimit;
	else
	if (phaseIncr > NcoHLimit) 
	   phaseIncr = NcoHLimit;
	else
	   locked = true;

	NcoPhase	+= phaseIncr + pll_Alpha * phaseError;
	if (NcoPhase >= 2 * M_PI) {
	   NcoPhase = fmod (NcoPhase, 2 * M_PI);
	   return;
	}
	if (NcoPhase > 0)
	   return;

	if (NcoPhase > - 2 * M_PI) 
	   NcoPhase += 2 * M_PI;
	else
	   NcoPhase = 2 * M_PI - fmod (-NcoPhase, 2 * M_PI);
}

DSPCOMPLEX	pllC::getDelay (void) {
	return pll_Delay;
}

DSPFLOAT	pllC::getPhaseIncr(void) {
	return	phaseIncr;
}

DSPFLOAT	pllC::getNco (void) {
	return NcoPhase;
}

DSPFLOAT	pllC::getPhaseError (void) {
	return phaseError;
}

bool		pllC::isLocked	(void) {
	return locked;
}

