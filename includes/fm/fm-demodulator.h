#
/*
 *    Copyright (C) 2014
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

#ifndef	__FM_DEMODULATOR
#define	__FM_DEMODULATOR

#include	<QObject>
#include	"fm-constants.h"
#include	"sincos.h"


class	RadioInterface;
class	decoderBase;

class	fm_Demodulator: public QObject {
Q_OBJECT
public:
	enum fm_demod {
	   FM1DECODER	= 0001,
	   FM2DECODER	= 0002,
	   FM3DECODER	= 0003,
	   FM4DECODER	= 0004,
	   FM5DECODER	= 0005,
	   FM6DECODER	= 0006
	};
private:
	int32_t		rateIn;
	SinCos		*mySinCos;
	int8_t		selectedDecoder;
	decoderBase	*theDecoder;
	DSPFLOAT	fm_afc;
	DSPFLOAT	fm_cvt;
	DSPFLOAT	K_FM;
public:
		fm_Demodulator	(RadioInterface	*mr,
	                         int32_t	Rate_in,
	                         SinCos		*mySinCos,
	                         DSPFLOAT	K_FM);
		~fm_Demodulator	(void);
	void	setDecoder	(int8_t);
	DSPFLOAT	demodulate	(DSPCOMPLEX);
	DSPFLOAT	get_dcComponent	(void);
signals:
	void		showDecoder	(const QString &);
};
#endif

