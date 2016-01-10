#
/*
 *    Copyright (C) 2010, 2011
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
 */
#ifndef	SIMPLE_PLL
#define	SIMPLE_PLL

#include	"fm-constants.h"
#include	"sincos.h"
#include	"Xtan2.h"


class	pll {
public:
		pll	(float	omega,
	                 float	gain,
	                 SinCos	*mySinCos,
	                 float	width	= 1.0);
	        ~pll	(void);
	bool	isLocked	(void);
	float	getPilotPhase	(DSPCOMPLEX pilot);
	float	getPhaseError	(void);
	float	getPhaseIncr	(void);
	DSPCOMPLEX getDelay	(void);
private:
	float		pilot_OscillatorPhase;
	DSPCOMPLEX	pilot_oldValue;
	float		omega;
	float		gain;
	float		width;
	SinCos		*mySinCos;
	DSPCOMPLEX	pll_delay;
	float		phaseIncr;
	float		avgError;
	bool		pll_isLocked;
	float		pll_phaseError;
	compAtan	myAtan;
};

#endif

