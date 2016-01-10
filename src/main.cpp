#
/*
 *
 *    Copyright (C) 2014
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Programming
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
 *	Main program
 */
#include	<QApplication>
#include	<QSettings>
#include	<unistd.h>
#include	<QDir>
#include	"fm-constants.h"
#include	"gui.h"

#define	DEFAULT_INI	".jsdr-fm-mini.ini"
#define	STATION_LIST	".jsdr-fm-stations.bin"

int	main (int argc, char **argv) {
int32_t		opt;
/*
 *	The default values
 */
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
	MyRadioInterface = new RadioInterface (ISettings, stationList);
	MyRadioInterface -> show ();
	a. exec ();
/*
 *	done:
 */
	fflush (stdout);
	fflush (stderr);
	qDebug ("It is done\n");
	delete MyRadioInterface;
	delete ISettings;
	exit (1);
}

