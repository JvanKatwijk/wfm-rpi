#
/*
 *    Copyright (C)  2011, 2012, 2013
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
 *
 */
#
#ifndef __FM_CONSTANTS
#define	__FM_CONSTANTS

#include	<math.h>
#include	<complex>
#include	<stdint.h>
#include	<limits>
#include	<stdlib.h>
#include	<unistd.h>

//#define	QT_STATIC_CONST

#ifndef __FREEBSD__
#include	<malloc.h>
#endif

#ifdef __MINGW32__
#include	"windows.h"
#else
#ifndef	__FREEBSD__
#include	"alloca.h"
#endif
#include	"dlfcn.h"
typedef	void	*HINSTANCE;
#endif
//
typedef	float	DSPFLOAT;
//
//
typedef	std::complex<DSPFLOAT>	DSPCOMPLEX;

using namespace std;

#define	Hz(x)		(x)
#define	Khz(x)		(x * 1000)
#define	KHz(x)		(x * 1000)
#define	Mhz(x)		(Khz (x) * 1000)
#define	MHz(x)		(KHz (x) * 1000)
/*
 */
#define	MINIMUM(x, y)	((x) < (y) ? x : y)
#define	MAXIMUM(x, y)	((x) > (y) ? x : y)

#define	IandQ		0100
#define	QandI		0101
#define I_Only		0102
#define	Q_Only		0104

#define	CURRENT_VERSION	"0.993"

#define	MSECFORTIMER	10

#define	RDS_DECIMATOR		8
#define	PILOTFILTER_SIZE	35
#define	RDSLOWPASS_SIZE		11
#define	HILBERT_SIZE		9
#define	RDSBANDFILTER_SIZE	35
#define	FFT_SIZE		512
#define	PILOT_WIDTH		100
#define	RDS_WIDTH		1500
#define	LEVEL_SIZE		512
#define	LEVEL_FREQ		3
//
#define	FM_MONO			0101
#define	FM_STEREO		0102
//	common functions


static inline
float	jan_abs (DSPCOMPLEX z) {
float	re	= real (z);
float	im	= imag (z);
	if (re < 0) re = - re;
	if (im < 0) im = - im;
	return re + im;
}
static inline
bool	isIndeterminate (DSPFLOAT x) {
	return x != x;
}

static inline
bool	isInfinite (double x) {
	return x == numeric_limits<DSPFLOAT>::infinity ();
}
//
static inline
DSPCOMPLEX cmul (DSPCOMPLEX x, float y) {
	return DSPCOMPLEX (real (x) * y, imag (x) * y);
}

static inline
DSPCOMPLEX cdiv (DSPCOMPLEX x, float y) {
	return DSPCOMPLEX (real (x) / y, imag (x) / y);
}

static inline
float	get_db (DSPFLOAT x, int32_t y) {
	return 20 * log10 ((x + 1) / (float)(y));
}
//
static	inline
DSPFLOAT	PI_Constrain (DSPFLOAT val) {
	if (0 <= val && val < 2 * M_PI)
	   return val;
	if (val >= 2 * M_PI)
	   return fmod (val, 2 * M_PI);
//	apparently val < 0
	if (val > - 2 * M_PI)
	   return val + 2 * M_PI;
	return 2 * M_PI - fmod (- val, 2 * M_PI);
}
#endif

