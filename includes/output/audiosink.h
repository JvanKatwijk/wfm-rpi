#
/*
 *    Copyright (C)  2009, 2010, 2011
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

#ifndef __AUDIO_SINK
#define	__AUDIO_SINK
#include	"fm-constants.h"
#include	<sndfile.h>
#include	<portaudio.h>
#include	"ringbuffer.h"

#define		LOWLATENCY	0100
#define		HIGHLATENCY	0200
#define		VERYHIGHLATENCY	0300

#ifdef	HAVE_STREAMER
class	streamerServer;
#endif

class	audioSink  {
public:
#ifdef	HAVE_STREAMER
			audioSink		(int32_t, streamerServer *);
#else
			audioSink		(int32_t);
#endif
			~audioSink		(void);
	int16_t		numberofDevices		(void);
const	char		*outputChannelwithRate	(int16_t, int32_t);
	void		stop			(void);
	void		restart			(void);
	int32_t		putSample		(DSPCOMPLEX);
	int32_t		putSamples		(DSPCOMPLEX *, int32_t);
	int16_t		invalidDevice		(void);
	bool		isValidDevice		(int16_t);

	int32_t		capacity		(void);
	bool		selectDefaultDevice	(void);
	bool		selectDevice		(int16_t);
	void		startDumping		(SNDFILE *);
	void		stopDumping		(void);
	int32_t		getSelectedRate		(void);
private:
#ifdef	HAVE_STREAMER
	streamerServer	*theStreamer;
#endif
	bool		OutputrateIsSupported	(int16_t, int32_t);
	int32_t		CardRate;
	int32_t		size;
	uint8_t		Latency;
	bool		portAudio;
	bool		writerRunning;
	int16_t		numofDevices;
	int		paCallbackReturn;
	int16_t		bufSize;
	PaStream	*ostream;
	SNDFILE		*dumpFile;
	RingBuffer<float>	*_O_Buffer;
	PaStreamParameters	outputParameters;
protected:
static	int		paCallback_o	(const void	*input,
	                                 void		*output,
	                                 unsigned long	framesperBuffer,
	                                 const PaStreamCallbackTimeInfo *timeInfo,
					 PaStreamCallbackFlags statusFlags,
	                                 void		*userData);
};

#endif

