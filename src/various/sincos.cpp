#
/*
 *    Copyright (C) 2008, 2009, 2010
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Programming
 *
 *    This file is part of SDR-J.
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

#include	"sincos.h"
//
//	As it turns out, when using DAB sticks, this simple function is the
//	real CPU burner, with a usage of up to 22 %

	SinCos::SinCos (DSPCOMPLEX *Table, int32_t Rate) {
           this	-> Table	= Table;
	   this	-> localTable	= false;
	   this	-> Rate		= Rate;
	   this	->	C	= Rate / (2 * M_PI);
}

	SinCos::SinCos (int32_t Rate) {
int32_t	i;
	   this	-> Rate		= Rate;
	   this	-> localTable	= true;
	   this	-> Table	= new DSPCOMPLEX [Rate];
	   for (i = 0; i < Rate; i ++) 
	      Table [i] = DSPCOMPLEX (cos (2 * M_PI * i / Rate),
	                              sin (2 * M_PI * i / Rate));
	   this	->	C	= Rate / (2 * M_PI);
}

	SinCos::~SinCos (void) {
	   if (localTable)
	      delete [] Table;
}
//	Heavy code: executed millions of times
//	we get all kinds of very strange values here, so
//	testing over the whole domain is needed
int32_t	SinCos::fromPhasetoIndex (DSPFLOAT Phase) {	
	if (Phase >= 0)
	   return (int32_t (Phase * C)) % Rate;
	else
	   return Rate - (int32_t (- Phase * C)) % Rate;
}

DSPFLOAT	SinCos::getSin (DSPFLOAT Phase) {
	if (Phase < 0)
	   return -getSin (- Phase);
	return imag (Table [fromPhasetoIndex (Phase)]);
}

DSPFLOAT	SinCos::getCos (DSPFLOAT Phase) {
	if (Phase >= 0)
	   return real (Table [(int32_t (Phase * C)) % Rate]);
	else
	   return real (Table [(int32_t (-Phase * C)) % Rate]);
}

DSPCOMPLEX	SinCos::getComplex (DSPFLOAT Phase) {
	if (Phase >= 0)
	   return Table [(int32_t (Phase * C)) % Rate];
	else
	   return Table [Rate - (int32_t ( - Phase * C)) % Rate];
}

DSPCOMPLEX	SinCos::getConjunct (DSPFLOAT Phase) {
int32_t	temp	= Phase * C;

	if (Phase >= 0)
	   if (temp >= Rate)
	      return conj (Table [temp % Rate]);
	   else
	      return conj (Table [temp]);
	else			// Phase < 0
	   if (temp > -Rate)
	      return conj (Table [Rate + temp]);
	   else
	   return conj (Table [(Rate + temp) % Rate]);
}


