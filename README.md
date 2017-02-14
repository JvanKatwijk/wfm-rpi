#wfm-rpi

A simple FM receiver for use on the Raspberry PI 2

WFM-rpi is software for the decoding of an FM signal, the software can be configured for use with an AIRSPY, SDRplay or a DABstick.
The software allows selecting a frequency through a small - separate - keypath, and
it provides the possibility of saving a selected frequency, together with a user-selected name in a station list.
One can add a station to the list by selecting the frequency and pushing the "FS" button.
Selecting a station (we,, the associated frequency) is by clicking ONCE the line in the station list.
One can remove a line from the station list by clicking TWICE on the line.
The software - obviously - supports decoding of rds.

The software has - just to experiment with - implemented 5 different algorithms for FM demodulation, user selectable.
This version of the FM software was designed such that it runs on a Raspberry PI 2. The - high - input samplerate
is filtered and decimated to a rate around 200000. Note that the
load of processing FM at a rate of 192000 or higher is heavy. It turns out that by selecting the lto option for the
compiler (gcc), the load is manageable and restricted to app 50-60 percent on all 4 cores of the RPI 2.


Define STREAMER in CMake or undefine CONFIG+=streamer in the ".pro" file for installing a (simple) streamer that outputs the audio on port 20040 allowing you to listen from another computer.

For creating an executable: install the required libraries and use
either qmake or CMake

Select ONE of the input devices in the configuration file

#####################################################################
Note
CMake assumes the availability of Qt5
qmake-qt4 and qmake-qt5 should both work

A simple "listener" is available as client.
Use qmake to build the listener, which is the same as for the DAB-RPI


#######################################################################
Since I was bored selecting the frequency of the stations I wanted
to listen to, the 0.993 and up version contains  a
station list mechanims.

One can add a station to the list by selecting the frequency and
pushing the "FS" button.
One can select a station (we,, the associated frequency) by clicking ONCE
the line in the station list.
One can remove a line from the station list by clicking TWICE on the line.

