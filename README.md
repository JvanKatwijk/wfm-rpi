#wfm-rpi

A simple FM receiver for use on the Raspberry PI 2

The FM receiver will operate with one of AIRSPY, SDRplay and DABsticks,
depending on the configuration.

Note that the load of processing FM at a rate of 192000 or higher
is heavy. It turns out that by selecting the lto option for the
compiler (gcc), the load is manageable and restricted to app 50-60
percent of all 4 cores of the RPI 2.

Note that the use of the AIRSPY will cause a rather high load due
to the non-integer rate conversions that have to be done.

Define STREAMER in CMake or undefine CONFIG+=streamer in the ".pro" file
for installing a (simple) streamer that outputs the audio on port 20040
allowing you to listen from another computer.

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

