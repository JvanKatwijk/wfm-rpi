#
/*
 *    Copyright (C) 2008, 2009, 2010
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Programming
 *
 *    This file is part of the SDR-J
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
 *	Simple straight forward pll
 */
#include	"pll.h"
//
//	rate is the samplerate
//	gain is the integration facor, determined by
//	the "bandwidth"
	pll::pll (float		omega,
	          float		gain,
	          SinCos	*mySinCos,
	          float		width) {
	   this	-> omega	= omega;
	   this	-> gain		= gain;
	   this	-> mySinCos	= mySinCos;
	   this -> width	= width;
	   pilot_OscillatorPhase	= 0;
	   pll_phaseError	= 0;
	   pll_isLocked		= false;
	   avgError		= 0;
}

	pll::~pll (void) {
}

bool	pll::isLocked (void) {
	return true;		// to be handled
	return pll_isLocked;
}

float	pll::getPilotPhase	(DSPCOMPLEX pilot) {
DSPCOMPLEX OscillatorValue	=
	                mySinCos -> getComplex (pilot_OscillatorPhase);
float	currentPhase;

	pll_delay		= pilot * conj (OscillatorValue);
	pll_phaseError		= myAtan. atan2 (imag (pll_delay), real (pll_delay));

	phaseIncr		= pll_phaseError * gain;
	avgError		= 0.999 * avgError + 0.001 * phaseIncr;
	pll_isLocked	= (- width / 2 <= avgError) && (avgError <= width / 2);

	pilot_OscillatorPhase	+= phaseIncr;
	currentPhase		= PI_Constrain (pilot_OscillatorPhase);
//
//	new phase:
	pilot_OscillatorPhase	= fmod (currentPhase + omega, 2 * M_PI);

	return currentPhase;
}

float	pll::getPhaseError	(void) {
	return pll_phaseError;
}

float	pll::getPhaseIncr	(void) {
	return phaseIncr + omega;
}

DSPCOMPLEX	pll::getDelay (void) {
	return pll_delay;
}

