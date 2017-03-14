#wfm-rpi

A simple FM receiver for use on the Raspberry PI 2
---------------------------------------------------------------------------

WFM-rpi is software for the decoding of an FM signal.
The software allows selecting a frequency through a small - separate - keypath, and
it provides the possibility of saving a selected frequency, together with a user-selected name in a station list.
One can add a station to the  station list by selecting the frequency and pushing the "FS" button.
Selecting a station (with the associated frequency) is by clicking ONCE the line in the station list.
One can remove a line from the station list by clicking TWICE on the line.
The software - obviously - supports decoding of rds.

----------------------------------------------------------------------------

The software has - just to experiment with - implemented 5 different algorithms for FM demodulation, user selectable.

----------------------------------------------------------------------------

The software can be configured for use with an AIRSPY, SDRplay or a DABstick.
This version of the FM software was designed such that it
runs on a Raspberry PI 2. The - high - input samplerate
is filtered and decimated to a rate around 192000. Note that the
load of processing FM at a rate of 192000 or higher is heavy.
It turns out that by selecting the lto option for the compiler (gcc),
the load is manageable and restricted to app 50-60 percent
on all 4 cores of the RPI 2.

---------------------------------------------------------------------------

An option is to control the WFM-rpi software through an ssh connection
from another computer (I use that since my Raspberry is not near my chair).
To enable listening, a similar option as with the Qt-DAB software is
implemented: the sound output can be send to an IP port.

Define STREAMER in CMake or undefine CONFIG+=streamer in the ".pro" file
for installing a (simple) streamer that outputs the audio
on port 20040 allowing you to listen from another computer.
A simple "listener" is available as client, 
use qmake to build the listener, which is the same as for the Qt-DAB

---------------------------------------------------------------------------

For creating an executable: install the required libraries and use
either qmake or CMake

The configuration file allows selection and incorporation of support
of one of the mentioned devices. Note that selecting more than one
of these devices in the configuration file lead to unexpected results.

The CMakeLists.txt file is written such that Qt5 is used.
The "wfm-rpi.pro" file is less specific, both qmake-qt4 (for Qt4) and qmake-qt5 (for Qt5) gave good results.

--------------------------------------------------------------------------

The software compiles on the RPI 2, tested with Arch Linux and
Jessie.

There might be an issue with sound on the RPI. Sound might be a
little stuttering when the wrong buffersizes are selected.

Select buffersizes with "latency=X" in the "ini" file ($HOME/.wfm-rpi.ini)
My favorite setting is 4.

After a first run, there will be an "ini" file with settings,
just add a line
latency=4
to test and adapt the value when there is stuttering


------------------------------------------------------------------------
Note on DABSTICks

There has been quite some developments on the code for supporting dabsticks.
This software assumes that there is a library "rtlsdr.." available.
The easiest is to take the library from the osmocom site, however,
there are other versions as well.
You might want to try the version from Leif Albrink
http://www.sm5bsz.com/linuxdsp/hware/rtlsdr/rtlsdr.htm

A good alternative (for me) was the one from
http://www.rtl-sdr.com/new-experimental-r820t-rtl-sdr-driver-tunes-13-mhz-lower/

Selecting in the ".pro" file is by commenting or uncommenting lines

	CONFIG+=dabstick

Similarly, comment or uncomment the definition lines in the top of the
CMakeLists.txt file if you use cmake.

-----------------------------------------------------------------------------
Note on SDRplay

The software is compiled against the 2.09 version of the SDRplay library
