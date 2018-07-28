#
/*
 *    Copyright (C)  2015 .. 2017
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

#ifndef __RADIO__
#define __RADIO__

#include	<QDialog>
#include	<QInputDialog>
#include	<QComboBox>
#include	"ui_sdrgui.h"
#include	<QTimer>
#include	<QQueue>
#include	<QWheelEvent>
#include	<QLineEdit>
#include	"fm-constants.h"
#include	"ringbuffer.h"
class	keyPad;

class	QSettings;
class	fmProcessor;
class	audioBase;
class	deviceHandler;
class	lowpassFIR;
class	squelch;
class	newConverter;
class	reSampler;
class	programList;

#ifdef	HAVE_STREAMER
class	streamerServer;
#endif
/*
 *	The main gui object. It inherits from
 *	QDialog and the generated form
 */
class RadioInterface: public QDialog,
		      private Ui_elektorSDR {
Q_OBJECT
public:
		RadioInterface		(QSettings	*,
	                                 QString,
	                                 QWidget *parent = NULL);
		~RadioInterface		();

private:
	void		localConnects		(void);
	void		dumpControlState	(QSettings *);

	RingBuffer<DSPCOMPLEX>	*audioSamples;
	RingBuffer<int16_t>	*audioBuffer;
	newConverter	*theConverter;
	reSampler	*audioDecimator;
	DSPCOMPLEX	*audioOut;
	keyPad		*mykeyPad;
	QSettings	*fmSettings;
	int32_t		inputRate;
	int32_t		fmRate;
	int32_t		workingRate;
	int32_t		audioRate;
	audioBase	*soundOut;
	deviceHandler	*myRig;
	int16_t		*outTable;
	int16_t		numberofDevices;

	int16_t		currAttSliderValue;
	DSPFLOAT	attValueL;
	DSPFLOAT	attValueR;

	lowpassFIR	*fmAudioFilter;
	int32_t		fmBandwidth;
	int32_t		fmFilterDegree;

	QComboBox	*theCards;
	void		restoreGUIsettings	(QSettings *);
	void		setTuner		(int32_t);
	void		Display			(int32_t);
	
	QTimer		*displayTimer;
	QTimer		*autoIncrementTimer;
	int16_t		IncrementIndex;
	int32_t		autoIncrement_amount;
	int32_t		fmIncrement;
	int32_t		minLoopFrequency;
	int32_t		maxLoopFrequency;
	int32_t         mapIncrement            (int32_t);
        int32_t         IncrementInterval       (int16_t);
	void		set_incrementFlag	(int16_t);
	void		stopIncrementing	(void);
	int32_t		get_Increment_for	(int16_t);

	fmProcessor	*myFMprocessor;
	squelch		*mySquelch;
	bool		squelchOn;

	uint8_t		rdsModus;
	uint8_t		channelSelector;

	void		IncrementFrequency	(int32_t);
	QString		RadioText;
	QString		StationLabel;

	int32_t		currentPIcode;
	int32_t		frequencyforPICode;
	int8_t		runMode;
	float		audioGain;
	float		leftChannel;
	float		rightChannel;
//
	float		Tau;
	float		alpha;
	float		xkm1;
	float		ykm1;
	void		sendSampletoOutput	(DSPCOMPLEX);
	programList	*myList;
	QLineEdit	*myLine;
/*
 *	The private slots link the GUI elements
 *	to the GUI code
 */
private slots:
	void	setStart		(void);
	void	updateTimeDisplay	(void);

	void	handle_freqButton	(void);
	void	abortSystem		(int);
	void	TerminateProcess	(void);

	void	setfmMode		(const QString &);
	void	setfmRdsSelector	(const QString &);
	void	setfmDecoder		(const QString &);
	void	setfmDeemphasis		(const QString &);
	void	setfmLFcutoff		(const QString &);
	void	set_squelchValue	(int);
	void	set_squelchMode		(void);

	void	autoIncrement_timeout	(void);
	void	autoIncrementButton	(void);
	void	autoDecrementButton	(void);
	void	set_Increment		(int);
	void	set_minimum		(int);
	void	set_maximum		(int);
	void	incrementButton		(void);
	void	decrementButton		(void);
	void	set_freqSave		(void);
	void	handle_myLine		(void);
public slots:
	void	setAudioChannelSelect	(const QString &);
	void	setCRCErrors		(int);
	void	setSyncErrors		(int);
	void	setbitErrorRate		(double);
	void	setGroup		(int);
	void	setPTYCode		(int);
	void	setPiCode		(int);
	void	clearStationLabel	(void);
	void	setStationLabel		(const QString &);
	void	clearRadioText		(void);
	void	setRadioText		(const QString &);
	void	setAFDisplay		(int);
	void	setRDSisSynchronized	(bool);
	void	setMusicSpeechFlag	(int);
	void	clearMusicSpeechFlag	(void);
	void	showLocked		(bool, float);
	void	audiosamplesAvailable	(int);
	void	set_audioGain		(float);
	void	newFrequency		(int);
	void	showDecoder		(const QString &);
	void	closeEvent              (QCloseEvent *event);

};

#endif

