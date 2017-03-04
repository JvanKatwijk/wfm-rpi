#
/*
 *    Copyright (C) 2015
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Programming
 *
 *    This file is part of the WFM-RPI.
 *    Many of the ideas as implemented in the WFM-RPI are derived from
 *    other work, made available through the GNU general Public License. 
 *    All copyrights of the original authors are recognized.
 *
 *    WFM-RPI is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    WFM-RPI is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with WFM-RPI; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *	Main program
 */
#include	<QApplication>
#include	<QSettings>
#include	<unistd.h>
#include	<QDir>
#include	"fm-constants.h"
#include	"gui.h"

#define	DEFAULT_INI	".wfm-rpi.ini"
#define	STATION_LIST	".wfm-rpi-stations.bin"

int	main (int argc, char **argv) {
QSettings	*ISettings;		/* .ini file	*/
RadioInterface	*MyRadioInterface;
QString iniFile = QDir::homePath ();
QString	stationList	= QDir::homePath ();
	iniFile. append ("/");
	iniFile. append (DEFAULT_INI);
	iniFile	= QDir::toNativeSeparators (iniFile);

	stationList. append ("/");
	stationList. append (STATION_LIST);
	stationList = QDir::toNativeSeparators (stationList);

/*
 *	... and the settings of the "environment"
 */
	ISettings	= new QSettings (iniFile, QSettings::IniFormat);
/*
 *	Before we connect control to the gui, we have to
 *	instantiate
 */
	QApplication a (argc, argv);
#if QT_VERSION >= 0x050600
	QGuiApplication::setAttribute (Qt::AA_EnableHighDpiScaling);
#endif
	MyRadioInterface = new RadioInterface (ISettings, stationList);
	MyRadioInterface -> show ();
	a. exec ();
/*
 *	done:
 */
	fflush (stdout);
	fflush (stderr);
	fprintf (stderr, "It is done\n");
	exit (1);
}

