#
/*
 *    Copyright (C) 2015
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Programming
 *
 *    This file is part of the SDR-J program suite.
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
#ifndef	__FM_DECODER__
#define	__FM_DECODER__
#include	"fm-constants.h"
#include	<QThread>
#include	"ringbuffer.h"
#include	"sincos.h"

class	RadioInterface;
class	rdsDecoder;
class	lowpassFIR;
class	fftFilter;
class	HilbertFilter;
class	fm_Demodulator;
class	pll;

class fmDecoder: public QThread  {
Q_OBJECT
public:
		fmDecoder (RadioInterface *,
	                   RingBuffer<DSPCOMPLEX> *,
	                   int32_t,
	                   rdsDecoder	*);
		~fmDecoder (void);
	void	doDecode	(DSPCOMPLEX *, int16_t);
	void	set_fmMode	(uint8_t);
	void	set_fmDemodulator	(uint8_t);
	bool	isLocked	(void);
	void	set_rdsModus	(bool);
	void	stop		(void);
private:
	RadioInterface	*myRadioInterface;
	RingBuffer<DSPCOMPLEX> *audioBuffer;
	int32_t		currentRate;
	RingBuffer<DSPCOMPLEX> *sampleBuffer;
	rdsDecoder	*my_rdsDecoder;
	void		run		(void);
	void		do_mono		(DSPCOMPLEX, DSPCOMPLEX *, float *);
	void		do_stereo	(DSPCOMPLEX, DSPCOMPLEX *, float *);
	bool		running;
	fftFilter	*rdsBandFilter;
	fftFilter	*pilotBandFilter;
	fftFilter	*rdsLowPassFilter;
	lowpassFIR	*lrdiffFilter;
	lowpassFIR	*lrplusFilter;
	HilbertFilter	*theHilbertFilter;
	pll		*rds_plldecoder;
	pll		*pilotRecover;
	fm_Demodulator	*theDemodulator;
	SinCos		*mySinCos;
	uint8_t		fmModus;
	bool		rdsModus;
	float		K_FM;
	uint8_t		newDemodulator;
signals:
	void	audiosamplesAvailable	(int);
	void	showLocked		(bool, float);
};
#endif

