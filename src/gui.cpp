#
/*
 *    Copyright (C) 2014
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Programming
 *
 *    This file is part of the  SDR-J series.
 *    Many of the ideas as implemented in the SDR-J are derived from
 *    other work, made available through the (a) GNU general Public License. 
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
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#include	<Qt>
#include	<QSettings>
#include	<QMessageBox>
#include	<QDebug>
#include	<QDateTime>
#include	"fm-constants.h"
#include	"gui.h"
#include	"popup-keypad.h"
#include	"fm-processor.h"
#include	"fm-demodulator.h"
#include	"rds-decoder.h"
#include	"newconverter.h"
#include	"resampler.h"
#include	"squelchClass.h"
#include	"program-list.h"
#include	"virtual-input.h"
#ifdef	HAVE_SDRPLAY
#include	"sdrplay.h"
#elif	HAVE_AIRSPY
#include	"airspy-handler.h"
#elif	HAVE_DABSTICK
#include	"dabstick.h"
#elif	HAVE_EXTIO
#include	"extio-handler.h"
#elif	HAVE_SW_ELAD_S1
#include	"sw-elad-s1.h"
#endif
#include	"audiosink.h"
#ifdef	HAVE_STREAMER
#include	"streamer.h"
#endif

#ifdef __MINGW32__
#include	<iostream>
#include	<windows.h>
#endif
//
//	Processing modes
#define	IDLE		0100
#define	PAUSED		0101
#define	RUNNING		0102
#define	STOPPING	0103
//
//
	enum Channels {
	   S_STEREO		= 0,
	   S_LEFT		= 1,
	   S_RIGHT		= 2,
	   S_LEFTplusRIGHT	= 0103,
	   S_LEFTminusRIGHT	= 0104
	};
static
int16_t	delayTable [] = {1, 3, 5, 7, 9, 10, 15};
#define delayTableSize	((int)(sizeof (delayTable) / sizeof (int16_t)))
/*
 *	We use the creation function merely to set up the
 *	user interface and make the connections between the
 *	gui elements and the handling agents. All real action
 *	is embedded in actions, initiated by gui buttons
 */
	RadioInterface::RadioInterface (QSettings	*Si,
	                                QString		stationList,
	                                QWidget		*parent): QDialog (parent) {
int16_t	i;
QString h;
int	k;
bool	success;
int32_t	startFreq;
int16_t	latency		= 1;

	setupUi (this);
	fmSettings		= Si;
//
//	before printing anything, we set
	setlocale (LC_ALL, "");

	runMode			= IDLE;
//
#ifdef	HAVE_DABSTICK
	myRig = new dabStick	(fmSettings, false, &success);
#elif	HAVE_SDRPLAY
	myRig = new sdrplay	(fmSettings, false, &success);
#elif	HAVE_AIRSPY
	myRig = new airspyHandler (fmSettings, false, &success);
#elif	HAVE_EXTIO
	myRig		= new extioHandler (fmSettings, &success);
#elif	HAVE_ELAD
	myRig		= new eladHandler (fmSettings, false, &success);
#endif
	if (!success) {
	   QMessageBox::warning (this, tr ("sdr"),
	                               tr ("Device problem\n"));
	   exit (1);
	}
//
//	about Rates
//	inputRate is the rate with which we get in samples from the device
//	fmRate is the rate where we do the demodulation
//	workingRate is the assumed rate for the output samples
//	audioRate is the real rate for the audio samples
	inputRate	= myRig	-> getRate ();
	fmRate		= inputRate % 256000 == 0 ? 256000 :
	                  inputRate % 192000 == 0 ? 192000 :
	                  inputRate % 240000 == 0 ? 240000 :
	                  inputRate < Khz (400) ? inputRate : 
	                  inputRate < Khz (850) ? inputRate / 2 :
	                  inputRate < Khz (1300) ? inputRate / 4 :
	                  inputRate < Khz (1900) ? inputRate / 6 :
	                                           inputRate / 10;
	latency			=
	                     fmSettings -> value ("latency", 1). toInt ();
	this	-> audioRate	=
	                     fmSettings	-> value ("audioRate",
	                                           48000). toInt ();
	this	-> workingRate	= 48000;
	this	-> audioDecimator	=
	                         new reSampler (fmRate, workingRate, 8192);
	this	-> audioOut		=
	                         new DSPCOMPLEX [audioDecimator -> getOutputsize ()];

#ifdef	HAVE_STREAMER
	theStreamer		= new streamerServer ();
	audioRate		= 48000;
	theSink			= new audioSink (this -> audioRate, latency,
	                                         theStreamer);
#else
	theSink			= new audioSink (this -> audioRate, latency);
#endif

	audioSamples		= new RingBuffer<DSPCOMPLEX> (2 *
	                                                   this -> audioRate);
	mySquelch		= new squelch (1,
	                                       workingRate / 10,
	                                       workingRate / 20,
	                                       workingRate);
	squelchOn		= false;
	theConverter		= NULL;
	int32_t	dummy;
	if (audioRate != workingRate)
           theConverter = new newConverter (workingRate, audioRate,
                                            workingRate / 20, dummy);

	outTable		= new int16_t
	                             [theSink -> numberofDevices () + 1];
	for (i = 0; i < theSink -> numberofDevices (); i ++)
	   outTable [i] = -1;

	if (!setupSoundOut (streamOutSelector,
	                    theSink,
	                    this -> audioRate,
	                    outTable)) {
	   fprintf (stderr, "Cannot open any output device\n");
	   abortSystem (33);
	}

	myFMprocessor		= NULL;
	startFreq		= myRig	-> defaultFrequency	();
	startFreq		= fmSettings -> value ("frequency",
	                                                startFreq). toInt ();
	setTuner (startFreq);
	Display (myRig -> getVFOFrequency ());
	h	= fmSettings -> value ("streamOutSelector",
	                                    "default"). toString ();
	k	= streamOutSelector -> findText (h);
	if (k != -1) {
	   streamOutSelector -> setCurrentIndex (k);
	   setStreamOutSelector (k);
	}

	fmFilterDegree	= 3;
	fmBandwidth	= fmSettings -> value ("audiowidth", 12000). toInt ();
	fmAudioFilter	= new lowpassFIR (fmFilterDegree,
	                                  fmBandwidth / 2,
	                                  fmRate);
//	Set relevant sliders etc to the value they had last time
	restoreGUIsettings	(fmSettings);
//
//	Display the version
#ifdef	HAVE_SDRPLAY
	QString v = "wfm-rpi-sdrplay";
#elif	HAVE_AIRSPY
	QString v = "wfm-rpi-airspy";
#elif	HAVE_DABSTICK
	QString v = "wfm-rpi-dabstick";
#elif	HAVE_ELAD
	QString v = "wfm-rpi-elad";
#elif	HAVE_EXTIO
	QString v = "wfm-rpi-extio";
#else
	QString v = "UNIDENTIFIED";
#endif
	
//	v. append (CURRENT_VERSION);
	systemindicator	-> setText (v);

	currentPIcode		= 0;
	frequencyforPICode	= 0;

	myFMprocessor	= new fmProcessor  (myRig,
	                                    this,
	                                    inputRate,
	                                    fmRate,
	                                    audioSamples);
	lcd_fmRate		-> display ((int)this -> fmRate);
	lcd_inputRate		-> display ((int)this -> inputRate);
	lcd_OutputRate		-> display ((int)this -> audioRate);

	setfmMode		(fmMode			-> currentText ());
	setfmRdsSelector	(fmRdsSelector		-> currentText ());
	setfmDecoder		(fmDecoder		-> currentText ());
	setAudioChannelSelect	(audioChannelSelect	-> currentText ());
	setfmDeemphasis		(fmDeemphasisSelector	-> currentText ());
	setfmLFcutoff		(fmLFcutoff		-> currentText ());

	audioGain	= 1.0;

	fmIncrement	=
	           fmSettings -> value ("fm_increment", 100). toInt ();
	fm_increment	-> setValue (fmIncrement);

	incrementingFlag -> setText (" ");
	IncrementIndex		= 0;
//	settings for the auto tuner
	IncrementIndex		=
	             fmSettings -> value ("IncrementIndex", 0). toInt ();
	fmIncrement		=
	             fmSettings -> value ("fm_increment", 100). toInt ();

	minLoopFrequency	=
	   fmSettings -> value ("min_loop_frequency", 86500). toInt ();
	if (minLoopFrequency == 0)
	   minLoopFrequency = 86500;

	maxLoopFrequency	=
	   fmSettings -> value ("max_loop_frequency", 110000). toInt ();
	if (maxLoopFrequency == 0)
	   maxLoopFrequency = 110000;

	fm_increment	-> setValue (fmIncrement);	// 
	minimumSelect	-> setValue (KHz (minLoopFrequency) / MHz (1));
	maximumSelect	-> setValue (KHz (maxLoopFrequency) / MHz (1));

	connect (fm_increment, SIGNAL (valueChanged (int)),
	              this, SLOT (set_Increment (int)));
	connect (minimumSelect, SIGNAL (valueChanged (int)),
	              this, SLOT (set_minimum (int)));
	connect (maximumSelect, SIGNAL (valueChanged (int)),
	              this, SLOT (set_maximum (int)));
//	he does the connections from the gui buttons, sliders etc
	localConnects		();
	mykeyPad		= new keyPad (this);
	connect (freqButton, SIGNAL (clicked (void)),
	         this, SLOT (handle_freqButton (void)));
//	create a timer for displaying the "real" time
	displayTimer		= new QTimer ();
	displayTimer		-> setInterval (1000);
	connect (displayTimer,
	         SIGNAL (timeout ()),
	         this,
	         SLOT (updateTimeDisplay ()));
	displayTimer		-> start (1000);
	autoIncrementTimer	= new QTimer;
	autoIncrementTimer	-> setSingleShot (true);
	autoIncrementTimer	-> setInterval (5000);
	connect (autoIncrementTimer, SIGNAL (timeout ()),
	         this, SLOT (autoIncrement_timeout ()));
//
//	testje
	myList	= new programList (this, stationList);
	myList	-> show ();
	myLine	= NULL;
	connect (freqSave, SIGNAL (clicked (void)),
	         this, SLOT (set_freqSave (void)));
}

	RadioInterface::~RadioInterface () {
	fprintf (stderr, "delete radiointerface\n");
}
//
//	When pressing the freqButton, a form will appear on
//	which the "user" may key in a frequency. 
//
//	If it is already there, pressing the button (for the second time)
//	will hide the keypad again
void	RadioInterface::handle_freqButton (void) {
	if (mykeyPad -> isVisible ())
	   mykeyPad -> hidePad ();
	else
	   mykeyPad	-> showPad ();
}

void	RadioInterface::newFrequency	(int f) {
	stopIncrementing ();
	setTuner (f);
	Display (myRig -> getVFOFrequency ());
}

//	When stopping, we save the values of some state elements
void	RadioInterface::dumpControlState	(QSettings *s) {

	if (s == NULL)		// should not happen
	   return;

	s	-> setValue ("frequency", myRig -> getVFOFrequency ());
	s	-> setValue ("fm_increment",
	                               fm_increment -> value ());
//
//	now setting the parameters for the fm decoder
	s	-> setValue ("fmMode",
	                               fmMode 		-> currentText ());
//	s	-> setValue ("fmDecoder",
//	                               fmDecoder	-> currentText ());
	s	-> setValue ("fmRdsSelector",
	                               fmRdsSelector	-> currentText ());
	s	-> setValue ("fmChannelSelect",
	                               audioChannelSelect -> currentText ());
	s	-> setValue ("fmDeemphasisSelector",
	                               fmDeemphasisSelector -> currentText ());
	s	-> setValue ("fmLFcutoff",
	                               fmLFcutoff	-> currentText ());
	s	-> setValue ("streamOutSelector",
	                               streamOutSelector -> currentText ());
}
//
//	On start, we ensure that the streams are stopped so
//	that they can be restarted again.
void	RadioInterface::setStart	(void) {
bool	r = 0;

	if (runMode == RUNNING)
	   return;
//
//	always ensure that datastreams are stopped
	myRig		-> stopReader ();
	theSink 	-> stopWriter ();
//
	r = myRig		-> restartReader ();
	qDebug ("Starting %d\n", r);
	if (!r) {
	   QMessageBox::warning (this, tr ("sdr"),
	                               tr ("Opening  input stream failed\n"));
	   return;
	}

	theSink	-> restart ();
	myFMprocessor	-> start ();
	runMode	= RUNNING;
}

void	RadioInterface::TerminateProcess (void) {
	runMode		= STOPPING;
	usleep (10000);
//
//	It is pretty important that no one is attempting to
//	set things within the FMprocessor when it is
//	being deleted.
	displayTimer		-> stop ();
	autoIncrementTimer	-> stop ();
	myRig			-> stopReader ();
	theSink			-> stopWriter ();
	myList			-> saveTable ();

	fprintf (stderr, "Termination started\n");
	qDebug () <<  "Termination started";
	usleep (10000);
	accept ();
	fprintf (stderr, "accepting\n");
	if (myFMprocessor != NULL) 
	   delete myFMprocessor;

	if (theConverter != NULL)
	   delete theConverter;

	dumpControlState (fmSettings);
	runMode		= IDLE;
	delete		mykeyPad;
	delete		myRig;
	delete		myList;
//	delete		theSink;
#ifdef	HAVE_STREAMER
	delete		theStreamer;
#endif
}

void	RadioInterface::abortSystem (int d) {
	qDebug ("aborting for reason %d\n", d);
	accept ();
}
//
//	The generic setTuner.
void	RadioInterface::setTuner (int32_t n) {
	myRig		-> setVFOFrequency	(n);
	if (myFMprocessor != NULL)
	   myFMprocessor	-> reset_rds	();
}
//

void	RadioInterface::updateTimeDisplay (void) {
QDateTime	currentTime = QDateTime::currentDateTime ();

	timeDisplay	-> setText (currentTime.
	                            toString (QString ("dd.MM.yy:hh:mm:ss")));
}

/*
 *	there is a tremendous amount of signal/slot connections
 *	The local connects, knobs, sliders and displays,
 *	are connected here.
 */
void	RadioInterface::localConnects (void) {
	connect (startButton, SIGNAL (clicked ()),
	              this, SLOT (setStart ()));
	connect (quitButton, SIGNAL (clicked ()),
	              this, SLOT (TerminateProcess (void)));
	connect (streamOutSelector, SIGNAL (activated (int)),
	              this, SLOT (setStreamOutSelector (int)));
	connect (squelchButton , SIGNAL (clicked (void)),
                      this, SLOT (set_squelchMode (void)));
        connect (squelchSlider, SIGNAL (valueChanged (int)),
                      this, SLOT (set_squelchValue (int)));

	connect (fc_plus, SIGNAL (clicked (void)),
	              this, SLOT (autoIncrementButton (void)));
	connect (fc_minus, SIGNAL (clicked (void)),
	              this, SLOT (autoDecrementButton (void)));
	connect (f_plus, SIGNAL (clicked (void)),
	              this, SLOT (incrementButton (void)));
	connect (f_minus, SIGNAL (clicked (void)),
	              this, SLOT (decrementButton (void)));
//
//	fm specific buttons and sliders
	connect (audioChannelSelect, SIGNAL (activated (const QString &)),
	               this, SLOT (setAudioChannelSelect (const QString &)));
	connect (fmMode, SIGNAL (activated (const QString &)),
	               this, SLOT (setfmMode (const QString &)));
	connect (fmRdsSelector, SIGNAL (activated (const QString &)),
	               this, SLOT (setfmRdsSelector (const QString &)));
	connect (fmDecoder, SIGNAL (activated (const QString &)),
	               this, SLOT (setfmDecoder (const QString &)));
	connect (fmDeemphasisSelector, SIGNAL (activated (const QString &)),
	              this, SLOT (setfmDeemphasis (const QString &)));
	connect (fmLFcutoff, SIGNAL (activated (const QString &)),
	              this, SLOT (setfmLFcutoff (const QString &)));
}

//	Deemphasis	= 50 usec (3183 Hz, Europe)
//	Deemphasis	= 75 usec (2122 Hz US)
void	RadioInterface::setfmDeemphasis	(const QString& s) {
DSPFLOAT	Tau;
int16_t	v	= s. toInt ();
	switch (v) {
	   default:
	      v	= 1;
	   case	1:
	   case 50:
	   case 75:
//	pass the Tau
	      Tau	= 1000000.0 / v;
	      alpha	= 1.0 / (DSPFLOAT (fmRate) / Tau + 1.0);
	}
}

void	RadioInterface::setCRCErrors	(int n) {
//	crcErrors	-> display (n);
	(void)n;
}

void	RadioInterface::setSyncErrors	(int n) {
//	syncErrors	-> display (n);
	(void)n;
}

void	RadioInterface::setbitErrorRate	(double v) {
//	bitErrorRate	-> display (v);
	(void)v;
}

void	RadioInterface::setGroup	(int n) {
	(void)n;
//	rdsGroupDisplay	-> display (n);
}

void	RadioInterface::setPTYCode	(int n) {
	(void)n;
//	rdsPTYDisplay	-> display (n);
}

void	RadioInterface::setAFDisplay	(int n) {
	rdsAFDisplay	-> display (n);
}

void	RadioInterface::setPiCode	(int n) {
int32_t	t	= myRig -> getVFOFrequency ();

	if ((frequencyforPICode != t) || (n != 0)) {
	   currentPIcode	= n;
	   frequencyforPICode = t;
	}

	rdsPiDisplay	-> display (n);
}

void	RadioInterface::clearStationLabel (void) {
	StationLabel = QString ("");
	stationLabelTextBox	-> setText (StationLabel);
}
//
//	Note: although s is a char * type, its value does not
//	end with a zero, so it is not a C string
void	RadioInterface::setStationLabel (const QString &s) {
	stationLabelTextBox	-> setText (s);
}

void	RadioInterface::setMusicSpeechFlag (int n) {
	if (n != 0)
	   speechLabel -> setText (QString ("music"));
	else
	   speechLabel -> setText (QString ("speech"));
}

void	RadioInterface::clearMusicSpeechFlag (void) {
	speechLabel	-> setText (QString (""));
}

void	RadioInterface::clearRadioText (void) {
	RadioText = QString ("");
	radioTextBox	-> setText (RadioText);
}
//
void	RadioInterface::setRadioText (const QString &s) {
	radioTextBox	-> setText (s);
}

void	RadioInterface::setRDSisSynchronized (bool syn) {
	if (!syn)
	   rdsSyncLabel -> setStyleSheet ("QLabel {background-color:red}");
	else
	   rdsSyncLabel -> setStyleSheet ("QLabel {background-color:green}");
}
//

void	RadioInterface::setfmMode (const QString &s) {
	myFMprocessor	-> set_fmMode (s == "stereo");
}

void	RadioInterface::setfmRdsSelector (const QString &s) {
	myFMprocessor	-> set_rdsMode (s == "rds on");
}

void	RadioInterface::setfmDecoder (const QString &s) {
int8_t	decoder	= 0;

	if (s == "fm1decoder")
	   decoder = fm_Demodulator::FM1DECODER;
	else
	if (s == "fm2decoder")
	   decoder = fm_Demodulator::FM2DECODER;
	else
	if (s == "fm3decoder")
	   decoder = fm_Demodulator::FM3DECODER;
	else
	if (s == "fm4decoder")
	   decoder = fm_Demodulator::FM4DECODER;
	else
	   decoder = fm_Demodulator::FM5DECODER;

	myFMprocessor	-> set_fmDecoder (decoder);
}

void	RadioInterface::showDecoder (const QString &s) {
	decoderDisplay	-> setText (s);
}
//
static inline
int32_t numberofDigits (int32_t f) {

	if (f < 100000)
	   return 6;
	if (f < 100000000)
	   return 8;
	if (f < 1000000000)
	   return 9;
	return 10;
}

void	RadioInterface::Display (int32_t freq) {
	lcd_Frequency	-> setDigitCount (6);
	lcd_Frequency	-> display ((int)freq / 1000);
}
//
//	This signal will arrive once every "inputRate" samples
void	RadioInterface::showLocked (bool locked, float the_dcComponent) {
	if (locked)
	   pll_isLocked -> setStyleSheet ("QLabel {background-color:green}");
	else
	   pll_isLocked -> setStyleSheet ("QLabel {background-color:red}");
	dc_component	-> display (the_dcComponent);
}
//
//	do not forget that ocnt starts with 1, due
//	to Qt list conventions
bool	RadioInterface::setupSoundOut (QComboBox	*streamOutSelector,
	                               audioSink	*our_audioSink,
	                               int32_t		cardRate,
	                               int16_t		*table) {
uint16_t	ocnt	= 1;
uint16_t	i;

	for (i = 0; i < our_audioSink -> numberofDevices (); i ++) {
	   const char *so =
	             our_audioSink -> outputChannelwithRate (i, cardRate);
	   qDebug ("Investigating Device %d\n", i);

	   if (so != NULL) {
	      streamOutSelector -> insertItem (ocnt, so, QVariant (i));
	      table [ocnt] = i;
	      qDebug (" (output):item %d wordt stream %d\n", ocnt , i);
	      ocnt ++;
	   }
	}

	qDebug () << "added items to combobox";
	return ocnt > 1;
}

void	RadioInterface::setStreamOutSelector (int idx) {
int16_t	outputDevice;

	if (idx == 0)
	   return;

	outputDevice = outTable [idx];
	if (!theSink -> isValidDevice (outputDevice)) 
	   return;

	theSink	-> stopWriter	();
	if (!theSink -> selectDevice (outputDevice)) {
	   QMessageBox::warning (this, tr ("sdr"),
	                               tr ("Selecting  output stream failed\n"));
	   return;
	}
}
//
//	handling of audio is now done here
//	we apply successively
//	- deemphasis
//	- rate conversion - if required
//	- low pass filtering
void	RadioInterface::audiosamplesAvailable (int n) {
DSPCOMPLEX buffer [512];
int16_t	i, k;
int32_t audioAmount;
float	Re, Im;
DSPCOMPLEX	out;

	if (runMode == STOPPING)
	   return;
	(void)n;
	while (audioSamples -> GetRingBufferReadAvailable () > 512) {
	   audioSamples -> getDataFromBuffer (buffer, 512);
	   for (i = 0; i < 512; i ++) {
//	deemphasize
	      Re	= xkm1 = (real (buffer [i]) - xkm1) * alpha + xkm1;
	      Im	= ykm1 = (imag (buffer [i]) - ykm1) * alpha + ykm1;
	      Re	*= audioGain;
	      Im	*= audioGain;
//	now towards the final rate
	      out	= fmAudioFilter -> Pass (DSPCOMPLEX (Re, Im));
	      if (audioDecimator -> doResample (out, audioOut, &audioAmount)) {
	         for (k = 0; k < audioAmount; k ++) {
	            DSPCOMPLEX pcmSample;
	            if (squelchOn)
	               pcmSample = mySquelch -> doSquelch (audioOut [k]);
	            else
	               pcmSample = audioOut [k];
	            sendSampletoOutput (pcmSample);
	         }
	      }
	   }
	}
}
//
//	A final conversion is possible if/when the outputrate differs
//	from 48000
void    RadioInterface::sendSampletoOutput (DSPCOMPLEX s) {

	if (runMode == STOPPING)
	   return;

	switch (channelSelector) {
	   default:
	      case S_STEREO:
	         s = DSPCOMPLEX (real (s) + imag (s),
	                         - (- real (s) + imag (s)));
	         break;

	      case S_LEFT:
	         s = DSPCOMPLEX (real (s) + imag (s), 
	                         real (s) + imag (s));
	         break;

	      case S_RIGHT:
	          s = DSPCOMPLEX (- (imag (s) - real (s)),
	                          - (imag (s) - real (s)));
	          break;

	      case S_LEFTplusRIGHT:
	         s = DSPCOMPLEX (real (s),  real (s));
	         break;

	      case S_LEFTminusRIGHT:
	         s = DSPCOMPLEX (imag (s), imag (s));
	         break;
	}

        if (audioRate == workingRate)
           theSink      -> putSample (s);
        else {
           DSPCOMPLEX out [theConverter -> getOutputsize ()];
           int32_t amount;
           if (theConverter -> convert (s, out, &amount)) {
              int16_t i;
              for (i = 0; i < amount; i ++)
                 theSink -> putSample (out [i]);
           }
        }
}

void	RadioInterface::set_squelchMode	(void) {
	squelchOn = !squelchOn;
	squelchButton	-> setText (squelchOn ? QString ("squelchOn") :
	                                          QString ("squelchOff"));
}

void	RadioInterface::set_squelchValue (int n) {
	mySquelch -> setSquelchLevel (n);
}

void	RadioInterface::setfmLFcutoff	(const QString &s) {
int16_t width	= s. toInt ();

	delete fmAudioFilter;
	fmAudioFilter	= new lowpassFIR (fmFilterDegree,
	                                  width / 2,
	                                  fmRate);
	
}

void	RadioInterface::set_audioGain (float f) {
	audioGain	= f;
}

void	RadioInterface::restoreGUIsettings(QSettings *s) {
QString h;
int	k;

	k	= s -> value ("squelchSlider",
	                          squelchSlider -> value ()). toInt ();
	squelchSlider		-> setValue (k);

	h	= s -> value ("fmMode",
	                          fmMode -> currentText ()). toString ();
	k	= fmMode -> findText (h);
	if (k != -1)
	   fmMode -> setCurrentIndex (k);

	h	= s -> value ("fmRdsSelector",
	                          fmRdsSelector -> currentText ()). toString ();
	k	= fmRdsSelector -> findText (h);
	if (k != -1)
	   fmRdsSelector -> setCurrentIndex (k);

	h	= s -> value ("fmDecoder",
	                          fmDecoder -> currentText ()). toString ();
	k	= fmDecoder -> findText (h);
	if (k != -1)
	   fmDecoder -> setCurrentIndex (k);

	h	= s -> value ("audioChannelSelect",
	                          audioChannelSelect -> currentText ()). toString ();
	k	= audioChannelSelect	-> findText (h);
	if (k != -1)
	   audioChannelSelect -> setCurrentIndex (k);

	h	= s -> value ("fmDeemphasisSelector",
	                       fmDeemphasisSelector -> currentText ()). toInt ();
	k	= fmDeemphasisSelector -> findText (h);
	if (k!= -1)
	   fmDeemphasisSelector -> setCurrentIndex (k);

	h	= s -> value ("fmLFcutoff",
	                          fmLFcutoff -> currentText ()). toString ();
	k	= fmLFcutoff -> findText (h);
	if (k != -1)
	   fmLFcutoff -> setCurrentIndex (k);
}

//
/////////////////////////////////////////////////////////////////////////
//
static inline
bool    frequencyInBounds (int32_t f, int32_t l, int32_t u) {
        return l <= f && f <= u;
}

int32_t RadioInterface::IncrementInterval (int16_t index) {
        if (index < 0)
           index = - index;

        if (index == 0)
           index = 1;
        if (index >= delayTableSize)
           index = delayTableSize;

        return 1000 * delayTable [index - 1];
}

void    RadioInterface::set_incrementFlag (int16_t incr) {
char temp [128];

        if (incr == 0) {
           incrementingFlag ->
                         setStyleSheet ("QLabel {background-color:blue}");
           incrementingFlag -> setText (" ");
           return;
        }
        if (incr < 0)
           sprintf (temp, " << %d", IncrementInterval (incr) / 1000);
        else
           sprintf (temp, "%d >> ", IncrementInterval (incr) / 1000);
        incrementingFlag ->
                         setStyleSheet ("QLabel {background-color:green}");
        incrementingFlag -> setText (temp);
}
/////////////////////////////////////////////////////////////////////////
//	stopIncrementing is called from various places to
//	just interrupt the autoincrementing
void	RadioInterface::stopIncrementing (void) {
	set_incrementFlag (0);

	if (autoIncrementTimer	-> isActive ())
	   autoIncrementTimer -> stop ();

	IncrementIndex = 0;
}

void	RadioInterface::autoIncrementButton (void) {

	if (autoIncrementTimer	-> isActive ())
	   autoIncrementTimer -> stop ();

	if (++IncrementIndex > delayTableSize)
	   IncrementIndex = delayTableSize;

	if (IncrementIndex == 0) {
	   set_incrementFlag (0);
	   return;
	}
//
	autoIncrementTimer -> start (IncrementInterval (IncrementIndex));
	set_incrementFlag (IncrementIndex);
}

void	RadioInterface::autoDecrementButton (void) {
	if (autoIncrementTimer	-> isActive ())
	   autoIncrementTimer -> stop ();

	if (--IncrementIndex < - delayTableSize)
	   IncrementIndex = - delayTableSize;

	if (IncrementIndex == 0) {
	   set_incrementFlag (0);
	   return;
	}
//
	autoIncrementTimer -> start (IncrementInterval (IncrementIndex));
	set_incrementFlag (IncrementIndex);
}

void	RadioInterface::set_Increment (int v) {
	fmIncrement	= v;		// in Khz
}
//
//	min and max frequencies are specified in Mhz
void	RadioInterface::set_minimum	(int f) {
	   minLoopFrequency	= Khz (f);
}

void	RadioInterface::set_maximum	(int f) {
	   maxLoopFrequency	= Khz (f);
}

void	RadioInterface::autoIncrement_timeout (void) {
int32_t	amount;
int32_t	frequency;
int32_t	low, high;

	low	= KHz (minLoopFrequency);
	high	= KHz (maxLoopFrequency);
	amount	=  fmIncrement;
	if (IncrementIndex < 0)
	   amount = - amount;
//
	frequency	= myRig -> getVFOFrequency () + KHz (amount);

	if ((IncrementIndex < 0) &&
	   !frequencyInBounds (frequency, low, high))
	   frequency = high;

	if ((IncrementIndex > 0) &&
	   !frequencyInBounds (frequency, low, high))
	   frequency = low;

	setTuner (frequency);
	Display (myRig -> getVFOFrequency ());
	autoIncrementTimer	-> start (IncrementInterval (IncrementIndex));
}

void	RadioInterface::incrementButton (void) {
	newFrequency (myRig -> getVFOFrequency () + Khz (fmIncrement));
}

void	RadioInterface::decrementButton (void) {
	newFrequency (myRig -> getVFOFrequency () - Khz (fmIncrement));
}
///////////////////////////////////////////////////////////////////////

void	RadioInterface::setAudioChannelSelect	(const QString &s) {
	if (s == "stereo")
	   channelSelector = S_STEREO;
	else
	if (s == "Left")
	   channelSelector = S_LEFT;
	else
	if (s == "Right")
	   channelSelector = S_RIGHT;
	else
	if (s == "Left+Right")
	   channelSelector = S_LEFTplusRIGHT;
	else
	   channelSelector = S_LEFTminusRIGHT;
}

/////////////////////////////////////////////////////////////////////////

void	RadioInterface::set_freqSave	(void) {
	myLine	= new QLineEdit ();
	myLine	-> show ();
	connect (myLine, SIGNAL (returnPressed (void)),
	         this, SLOT (handle_myLine (void)));
}

void	RadioInterface::handle_myLine (void) {
int32_t	freq	= myRig -> getVFOFrequency ();
QString	programName	= myLine -> text ();
	myList	-> addRow (programName, QString::number (freq / Khz (1)));
	delete myLine;
	myLine	= NULL;
}


