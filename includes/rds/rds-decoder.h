#
/*
 *    Copyright (C) 2014
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Programming
 *
 *	cuteSDR (c) M Wheatly 2011
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

#ifndef	__RDS_DECODER
#define	__RDS_DECODER

#include	<QObject>
#include	"fm-constants.h"
#include	"rds-group.h"
#include	"rds-blocksynchronizer.h"
#include	"rds-groupdecoder.h"
#include	"fft.h"
#include	"iir-filters.h"
#include	"sincos.h"

class	RadioInterface;

class	rdsDecoder : public QObject {
Q_OBJECT
public:
		rdsDecoder (RadioInterface *, int32_t);
		~rdsDecoder (void);
	void	doDecode	(DSPFLOAT);
	void	reset		(void);
private:
	void			processBit	(bool);
	int32_t			sampleRate;
	RadioInterface		*myRadioInterface;
	RDSGroup		*my_rdsGroup;
	rdsBlockSynchronizer	*my_rdsBlockSync;
	rdsGroupDecoder		*my_rdsGroupDecoder;
	bool			prevBit;

	DSPFLOAT		*rdsBuffer;
	DSPFLOAT		*rdsKernel;
	int16_t			ip;
	int16_t			rdsfilterSize;
	DSPFLOAT		Match		(DSPFLOAT);
	BandPassIIR		*sharpFilter;
	DSPFLOAT		rdsLastSyncSlope;
	DSPFLOAT		rdsLastSync;
	DSPFLOAT		rdsLastData;
	bool			previousBit;
	DSPFLOAT		*syncBuffer;
	int16_t			p;
signals:
	void			setCRCErrors		(int);
	void			setSyncErrors		(int);
	void			setbitErrorRate		(int);
};

#endif

