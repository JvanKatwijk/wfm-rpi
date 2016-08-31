######################################################################
# Automatically generated by qmake (2.01a) Tue Oct 6 19:48:14 2009
# but modified by me to accomodate for the includes for qwt and
# portaudio
######################################################################

TEMPLATE	= app
QT		+= widgets 
CONFIG		+= console
DEFINES		+= SMALL_GUI
QMAKE_CFLAGS	+= -flto -ffast-math
QMAKE_CXXFLAGS	+= -flto -ffast-math
QMAKE_LFLAGS	+= -flto
#QMAKE_CFLAGS	+= -pg
#QMAKE_CXXFLAGS	+= -pg
#QMAKE_LFLAGS	+= -pg
########################
#
#	choose ONE device
#
#CONFIG	+= airspy
CONFIG	+= sdrplay
#CONFIG	+= dabstick-osmo
#	Handle with care
#CONFIG	+= extio
#	Handle not at all
#CONFIG	+= sw-elad-s1
#
#	Note that if you use "streamer", sound will not be send
#	to the local soundcard
#CONFIG	+= streamer
DEPENDPATH += . \
	      ./src \
	      ./includes \
	      ./includes/output \
	      ./includes/various \
	      ./includes/fm \
	      ./includes/rds \
	      ./includes/various \
	      ./src \
	      ./src/output \
	      ./src/various \
	      ./src/fm \
	      ./src/rds \
	      ./src/various 
	      
INCLUDEPATH += . \
	      ./includes \
	      ./includes/output \
	      ./includes/various \
	      ./includes/fm \
	      ./includes/rds \
	      ./includes/various \
	      ./input 

# Input
HEADERS += ./includes/gui.h \
	   ./includes/popup-keypad.h \
	   ./includes/fm-constants.h \
	   ./includes/various/squelchClass.h \
           ./includes/various/fft.h \
	   ./includes/various/oscillator.h \
           ./includes/various/ringbuffer.h \
	   ./includes/various/pllC.h \
	   ./includes/various/Xtan2.h \
	   ./includes/various/sincos.h \
	   ./includes/various/resampler.h \
	   ./includes/various/converter.h \
	   ./includes/various/fir-filters.h \
	   ./includes/various/fft-filters.h \
	   ./includes/various/iir-filters.h \
	   ./includes/various/program-list.h \
	   ./includes/output/audio-base.h \
	   ./includes/output/audiosink.h \
	   ./includes/fm/fm-demodulator.h \
	   ./includes/fm/fm-processor.h \
	   ./includes/fm/fm-decoder.h \
	   ./includes/fm/fm-levels.h \
	   ./includes/rds/rds-decoder.h \
	   ./includes/rds/rds-blocksynchronizer.h \
	   ./includes/rds/rds-group.h \
	   ./includes/rds/rds-groupdecoder.h  \
	   ./includes/rds/charsets.h \
	   ./input/virtual-input.h 

FORMS += ./src/sdrgui.ui 

SOURCES += ./src/main.cpp \
	   ./src/gui.cpp \
	   ./src/popup-keypad.cpp \
           ./src/various/fft.cpp \
	   ./src/various/oscillator.cpp \
	   ./src/various/pllC.cpp \
	   ./src/various/Xtan2.cpp \
	   ./src/various/sincos.cpp \
	   ./src/various/resampler.cpp \
	   ./src/various/fir-filters.cpp \
	   ./src/various/fft-filters.cpp \
	   ./src/various/iir-filters.cpp \
	   ./src/various/program-list.cpp \
	   ./src/output/audio-base.cpp \
	   ./src/output/audiosink.cpp \
	   ./src/fm/fm-demodulator.cpp \
	   ./src/fm/fm-processor.cpp \
	   ./src/fm/fm-decoder.cpp \
	   ./src/fm/fm-levels.cpp \
	   ./src/rds/rds-decoder.cpp \
	   ./src/rds/rds-blocksynchronizer.cpp \
	   ./src/rds/rds-group.cpp \
	   ./src/rds/rds-groupdecoder.cpp \
	   ./src/rds/charsets.cpp \
	   ./input/virtual-input.cpp 
#
# for windows32 we use:
win32 {
DESTDIR	= ../../windows-bin-dab
# includes in mingw differ from the includes in fedora linux
INCLUDEPATH 	+= /usr/i686-w64-mingw32/sys-root/mingw/include
INCLUDEPATH 	+= /usr/i686-w64-mingw32/sys-root/mingw/include/qt5/qwt
LIBS	+= -lfftw3f
LIBS	+= -lportaudio
LIBS	+= -lqwt-qt5
LIBS	+= -lsndfile
LIBS	+= -lsamplerate
LIBS	+= -lole32
LIBS	+= -lwinmm
LIBS 	+= -lstdc++
LIBS	+= -lusb-1.0
LIBS	+= -lwinpthread
}
#
#for fedora and ubuntu  and the rpi we use
unix { 
DESTDIR		= ./linux-bin
INCLUDEPATH 	+= /usr/include/qt4
LIBS		+=  -lusb-1.0 -lrt -lportaudio -lsndfile -lfftw3f -ldl
LIBS		+= -lsamplerate
}

#	the devices:
#	for a "special" rtlsdr library use
dabstick-new {
	TARGET		= wfm-rpi-dabstick-0.996
	DEFINES		+= HAVE_DABSTICK
	FORMS		+= ./input/dabstick-new/dabstick-widget.ui
	INCLUDEPATH	+= ./input/dabstick-new
	DEPENDPATH	+= ./input/dabstick-new
	HEADERS		+= ./input/dabstick-new/dabstick.h \
	                   ./input/dabstick-new/dongleselect.h
	SOURCES		+= ./input/dabstick-new/dabstick.cpp \
	                   ./input/dabstick-new/dongleselect.cpp
	LIBS		+= -lrtlsdr
}
#	the devices
#	for the "standard" rtlsdr library use:
dabstick-osmo {
	TARGET		= wfm-rpi-dabstick-0.996
	DEFINES		+= HAVE_DABSTICK
	FORMS		+= ./input/dabstick-osmo/dabstick-widget.ui
	INCLUDEPATH	+= ./input/dabstick-osmo
	DEPENDPATH	+= ./input/dabstick-osmo
	HEADERS		+= ./input/dabstick-osmo/dabstick.h \
	                   ./input/dabstick-osmo/dongleselect.h
	SOURCES		+= ./input/dabstick-osmo/dabstick.cpp \
	                   ./input/dabstick-osmo/dongleselect.cpp
#	LIBS		+= -lrtlsdr
}
#
#	the SDRplay
#
sdrplay {
	TARGET		= wfm-rpi-sdrplay-0.996
	DEFINES		+= HAVE_SDRPLAY
	INCLUDEPATH	+= ./input/sdrplay
	HEADERS		+= ./input/sdrplay/sdrplay.h \
	                   ./input/sdrplay/sdrplay-loader.h \
	                   ./input/sdrplay/sdrplay-worker.h 
	SOURCES		+= ./input/sdrplay/sdrplay.cpp \
	                   ./input/sdrplay/sdrplay-loader.cpp \
	                   ./input/sdrplay/sdrplay-worker.cpp 
	FORMS		+= ./input/sdrplay/sdrplay-widget.ui
}
#
#	the AIRSPY
#
airspy {
	TARGET		= wfm-rpi-airspy-0.996
	DEFINES		+= HAVE_AIRSPY
	FORMS		+= ./input/airspy/airspy-widget.ui
	DEPENDPATH	+= ./input/airspy
	INCLUDEPATH	+= ./input/airspy \
	                   /usr/local/include/libairspy
	HEADERS		+= ./input/airspy/airspy-handler.h 
	SOURCES		+= ./input/airspy/airspy-handler.cpp 
}
#
streamer	{
	DEFINES		+= HAVE_STREAMER
	HEADERS		+= ./includes/output/tcp-streamer.h
	SOURCES		+= ./src/output/tcp-streamer.cpp
	QT		+= network 
}
#
#	the elad-s1
#
elad_s1 {
	TARGET		= wfm-rpi-elad-0.996
	DEFINES		+= HAVE_ELAD_S1
	FORMS		+= ./input/sw-elad-s1/elad_widget.ui
	DEPENDPATH	+= ./input/sw-elad-s1
	INCLUDEPATH	+= ./input/sw-elad-s1 
	HEADERS		+= ./input/sw-elad-s1/elad-s1.h \
	                   ./input/sw-elad-s1/elad-worker.h \
	                   ./input/sw-elad-s1/elad-loader.h
	SOURCES		+= ./input/sw-elad-s1/elad-s1.cpp \
	                   ./input/sw-elad-s1/elad-worker.cpp \
	                   ./input/sw-elad-s1/elad-loader.cpp
}
#
#	extio dependencies, windows only
#
extio {
	TARGET		= wfm-rpi-extio-0.996
	DEFINES		+= HAVE_EXTIO
	INCLUDEPATH	+= ./input/extio-handler
	HEADERS		+= ./input/extio-handler/extio-handler.h \
	                   ./input/extio-handler/common-readers.h \
	                   ./input/extio-handler/virtual-reader.h 
	SOURCES		+= ./input/extio-handler/extio-handler.cpp \
	                   ./input/extio-handler/common-readers.cpp \
	                   ./input/extio-handler/virtual-reader.cpp 
}

