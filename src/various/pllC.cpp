#
/*
 *
 *    Copyright (C) 2008, 2009, 2010
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
 *	This pll was found to give reasonable results.
 *	source DTTSP, all rights acknowledged
 *
 *	This is the version for complex values
 */
#include	"pllC.h"
//
//	"rate"	is the samplerate
//	"lofreq" and "hifreq" the frequencies (in Hz) where the lock is 
//	kept in between,
//	"bandwidth" the bandwidth of the signal to be received

		pllC::pllC (int32_t rate,
	                    DSPFLOAT freq,
	                    DSPFLOAT lofreq, DSPFLOAT hifreq,
	                    DSPFLOAT bandwidth,
	                    SinCos *Table) {
DSPFLOAT	omega	= 2.0 * M_PI / rate;
float	eta		= 0.1;

	NcoPhaseIncr	= freq * omega;		// this will change during runs
	NcoLLimit	= lofreq * omega;	// boundary for changes
	NcoHLimit	= hifreq * omega;

	pll_Alpha	= 2 * eta * bandwidth * omega;	// pll bandwidth
	pll_Beta	= (pll_Alpha * pll_Alpha) / 1.0; // second order term
	NcoPhase	= 0;
	this	-> mySinCos	= Table;
	phzError	= 0;
}
//
		pllC::~pllC (void) {
}
//
//	It turned out that under Fedora we had from time
//	to time an infinite value for signal. Still have
//	to constrain this value
void		pllC::do_pll (DSPCOMPLEX signal) {
DSPCOMPLEX	NcoSignal;
DSPCOMPLEX	quadRef;

	NcoSignal 	= mySinCos -> getComplex (NcoPhase);
	    
	pll_Delay	= NcoSignal * signal;
	phzError	= - myAtan. atan2 (imag (pll_Delay), real (pll_Delay));
	NcoPhaseIncr	+= pll_Beta * phzError;
	locked		= false;
	if (NcoPhaseIncr < NcoLLimit)
	   NcoPhaseIncr = NcoLLimit;
	else
	if (NcoPhaseIncr > NcoHLimit)
	   NcoPhaseIncr = NcoHLimit;
	else
	   locked	= true;

	NcoPhase	+= NcoPhaseIncr + pll_Alpha * phzError;
	if (NcoPhase >= 2 * M_PI)
	   NcoPhase = fmod (NcoPhase, 2 * M_PI);
	while (NcoPhase < 0)
	   NcoPhase += 2 * M_PI;
}

DSPCOMPLEX	pllC::getDelay (void) {
	return pll_Delay;
}

DSPFLOAT	pllC::getPhaseIncr(void) {
	return NcoPhaseIncr;
}

DSPFLOAT	pllC::getNco (void) {
	return NcoPhase;
}

DSPFLOAT	pllC::getPhaseError (void) {
	return phzError;
}

bool		pllC::isLocked	(void) {
	return locked;
}
