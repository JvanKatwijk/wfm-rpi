#
/*
 *    Copyright (C) 2011, 2012, 2013
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
 */

#include	"audio-base.h"
#include	<stdio.h>

/*
 *	The class is the abstract sink for the data generated
 *	It will handle the "dumping" though
 */
	audioBase::audioBase	(RingBuffer<int16_t> *b) {
	buffer			= b;
}

	audioBase::~audioBase	(void) {
}

void	audioBase::restart	(void) {
}

void	audioBase::stop	(void) {
}
//
//	This one is a hack for handling different baudrates coming from
//	the aac decoder
void	audioBase::audioOut	(uint32_t rate) {
int16_t V [4800];

	while (buffer -> GetRingBufferReadAvailable () > rate / 10) {
	   int16_t amount = buffer -> getDataFromBuffer (V, rate / 10);
	   audioOut_48000 (V, amount / 2);
	}
}
void	audioBase::audioOut_48000	(int16_t *V, int32_t amount) {
float *buffer = (float *)alloca (2 * amount * sizeof (float));
int32_t	i, n;

	for (i = 0; i < amount; i ++) {
	   buffer [2 * i]	= V [2 * i] / 32767.0;
	   buffer [2 * i + 1]	= V [2 * i + 1] / 32767.0;
	}

	audioOutput (buffer, amount);
}

//
//	The audioOut function is the one that really should be
//	reimplemented in the offsprings of this class
void	audioBase::audioOutput	(float *v, int32_t amount) {
	(void)v;
	(void)amount;
}

