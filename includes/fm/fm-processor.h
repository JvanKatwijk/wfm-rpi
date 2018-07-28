#
/*
 *    Copyright (C) 2015 .. 2017
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Computing
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

#ifndef	__FM_PROCESSOR
#define	__FM_PROCESSOR

#include	<QThread>
#include	<QObject>
#include	<sndfile.h>
#include	"fm-constants.h"
#include	"fir-filters.h"
#include	"ringbuffer.h"

class		deviceHandler;
class		RadioInterface;
class		fmDecoder;
class		rdsDecoder;

class	fmProcessor:public QThread {
Q_OBJECT
public:
			fmProcessor (deviceHandler *,
	                             RadioInterface *,
	                             int32_t,	// inputRate
	                             int32_t,	// fmRate
	                             RingBuffer<DSPCOMPLEX> *);
	        	~fmProcessor (void);
	void		stop		(void);		// stop the processor
	void		set_fmDecoder	(uint8_t);
	void		set_fmAttenuation	(int16_t, int16_t);

	bool		isLocked		(void);
	void		set_squelchValue	(int16_t);
	void		reset_rds		(void);
	void		set_fmMode		(bool);
	void		set_rdsMode		(bool);
private:
	deviceHandler	*myRig;
	RadioInterface	*myRadioInterface;
	void		run			(void);
	RingBuffer<DSPCOMPLEX> *fmSamples;
	fmDecoder	*my_fmDecoder;
	rdsDecoder	*my_rdsDecoder;
	int32_t		inputRate;
	int32_t		fmRate;
	int32_t		decimatingScale;
	DecimatingFIR	*decimatingFilter;
volatile
	bool		running;
	float		peakLevel;
	int16_t		peakLevelcnt;
	float		audioGain;
	float		audioGainAverage;
	float		max_freq_deviation;
	float		norm_freq_deviation;
signals:
	void		setPLLisLocked		(bool);
	void		new_audioGain		(float);
};

#endif

