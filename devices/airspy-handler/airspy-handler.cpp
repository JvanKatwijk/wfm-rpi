
/**
 *  IW0HDV Extio
 *
 *  Copyright 2015 by Andrea Montefusco IW0HDV
 *
 *  Licensed under GNU General Public License 3.0 or later. 
 *  Some rights reserved. See COPYING, AUTHORS.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 *
 *	recoding, taking parts and extending for the airspyHandler interface
 *	for the SDR-J-DAB receiver.
 *	jan van Katwijk
 *	Lazy Chair Computing
 */

#ifdef	__MINGW32__
#define	GETPROCADDRESS	GetProcAddress
#else
#define	GETPROCADDRESS	dlsym
#endif

#include "airspy-handler.h"

static
const	int	EXTIO_NS	=  8192;
static
const	int	EXTIO_BASE_TYPE_SIZE = sizeof (float);

	airspyHandler::airspyHandler (QSettings *s) {
int	result;
QString	h;
uint32_t buffer [20];
uint	nr_rates;
uint16_t	i;

	this	-> airspySettings	= s;
	deviceOK		= false;
	this	-> myFrame	= new QFrame (NULL);
	setupUi (this -> myFrame);
	this	-> myFrame	-> show ();

	device			= 0;

	airspySettings	-> beginGroup ("airspyHandler");
	int16_t temp 		= airspySettings -> value ("linearity", 10).
	                                                          toInt ();
	linearitySlider		-> setValue (temp);
	linearityDisplay	-> display  (temp);
	temp			= airspySettings -> value ("sensitivity", 10).
	                                                          toInt ();
	sensitivitySlider	-> setValue (temp);
	sensitivityDisplay	-> display (temp);
	vgaGain			= airspySettings -> value ("vga", 5).toInt ();
	vgaSlider		-> setValue (vgaGain);
	vgaDisplay		-> display (vgaGain);
	mixerGain		= airspySettings -> value ("mixer", 10). toInt ();
	mixerSlider		-> setValue (mixerGain);
	mixerDisplay		-> display (mixerGain);
	mixer_agc		= false;
	lnaGain			= airspySettings -> value ("lna", 5). toInt ();
	lnaSlider		-> setValue (lnaGain);
	lnaDisplay		-> display  (lnaGain);
	mixer_agc		= false;
	lna_agc			= false;
	rf_bias			= false;
	airspySettings		-> endGroup ();

	serialNumber		= 0;
	theBuffer		= NULL;
#ifdef	__MINGW32__
	const char *libraryString = "airspy.dll";
	Handle		= LoadLibrary ((wchar_t *)L"airspy.dll");
#else
	const char *libraryString = "libairspy.so";
	Handle		= dlopen ("libusb-1.0.so", RTLD_NOW | RTLD_GLOBAL);
	if (Handle == NULL) {
	   fprintf (stderr, "libusb cannot be loaded\n");
	   throw (21);
	}
	   
	Handle		= dlopen ("libairspy.so", RTLD_LAZY);
#endif

	if (Handle == NULL) {
	   fprintf (stderr, "failed to open %s\n", libraryString);
#ifndef	__MINGW32__
	   fprintf (stderr, "Error = %s\n", dlerror ());
#endif
	   throw (22);
	}
	libraryLoaded	= true;

	if (!load_airspyFunctions ()) {
	   fprintf (stderr, "problem in loading functions\n");
	   throw (23);
	}
//
	strcpy (serial,"");
	result = this -> my_airspy_init ();
	if (result != AIRSPY_SUCCESS) {
	   printf("my_airspy_init () failed: %s (%d)\n",
	             my_airspy_error_name((airspy_error)result), result);
	   throw (24);
	}
	
	result = my_airspy_open (&device);
	if (result != AIRSPY_SUCCESS) {
	   printf ("my_airpsy_open () failed: %s (%d)\n",
	             my_airspy_error_name ((airspy_error)result), result);
	   throw (25);
	}

	result = my_airspy_set_sample_type (device, AIRSPY_SAMPLE_FLOAT32_IQ);
	if (result != AIRSPY_SUCCESS) {
	   printf ("my_airspy_set_sample_type () failed: %s (%d)\n",
	            my_airspy_error_name ((airspy_error)result), result);
	   throw (26);
	}
//
//	poll for the frequencies supported
	(void)my_airspy_get_samplerates (device, &nr_rates, 0);
	fprintf (stderr, "%d different rates supported\n", nr_rates);
	(void)my_airspy_get_samplerates (device, buffer, nr_rates);
	inputRate	= 20000000;
	for (i = 0; i < nr_rates; i ++) {
	   fprintf (stderr, "%d\n", buffer [i]);
	   if (buffer [i] < inputRate) 
	      inputRate = buffer [i];
	}

	if (inputRate == 0) {
	   fprintf (stderr, "no valid inputrate found\n");
	   my_airspy_close (device);
#ifdef __MINGW32__
	   FreeLibrary (Handle);
#else
	   if (Handle != NULL)
	      dlclose (Handle);
#endif
	   Handle		= NULL;
	   throw (27);
	}
	show_tab (0);
//
	result = my_airspy_set_samplerate (device, inputRate);
	if (result != AIRSPY_SUCCESS) {
	   printf ("airspy_set_samplerate() failed: %s (%d)\n",
	            my_airspy_error_name ((airspy_error)result), result);
	   throw (28);
	} 

	airspy_rate	-> display ((int)inputRate);
//	To make our life easier we just convert the inputRate / 1000 to
//	a multiple of 1920, i.e. 1920.
	for (i = 0; i < 1920; i ++) {
	   mapTable_int [i] = int (floor (i * ((inputRate / 1000)  / 1920.0)));
	   mapTable_float [i] = i * ((inputRate / 1000) / 1920.0) - mapTable_int [i];
	}

	convBufferSize	= inputRate / 1000;
	convIndex	= 0;
	convBuffer	= new DSPCOMPLEX [convBufferSize + 1];
	fprintf (stderr, "convBuffersize = %d\n", convBufferSize);
	theBuffer		= new RingBuffer<DSPCOMPLEX> (1024 *1024);
	tabWidget	-> setCurrentIndex (0);
	connect (linearitySlider, SIGNAL (valueChanged (int)),
                 this, SLOT (set_linearity (int)));
        connect (sensitivitySlider, SIGNAL (valueChanged (int)),
                 this, SLOT (set_sensitivity (int)));

	connect (lnaSlider, SIGNAL (valueChanged (int)),
	         this, SLOT (set_lna_gain (int)));
	connect (vgaSlider, SIGNAL (valueChanged (int)),
	         this, SLOT (set_vga_gain (int)));
	connect (mixerSlider, SIGNAL (valueChanged (int)),
	         this, SLOT (set_mixer_gain (int)));
	connect (lnaButton, SIGNAL (clicked (void)),
	         this, SLOT (set_lna_agc (void)));
	connect (mixerButton, SIGNAL (clicked (void)),
	         this, SLOT (set_mixer_agc (void)));
	connect (biasButton, SIGNAL (clicked (void)),
	         this, SLOT (set_rf_bias (void)));
	displaySerial	-> setText (getSerial ());
	running		= false;
	deviceOK	= true;
	return;
}

	airspyHandler::~airspyHandler (void) {
	airspySettings	-> beginGroup ("airspyHandler");
	airspySettings -> setValue ("linearity", linearitySlider -> value ());
	airspySettings -> setValue ("sensitivity", sensitivitySlider -> value ());
	airspySettings -> setValue ("vga", vgaGain);
	airspySettings -> setValue ("mixer", mixerGain);
	airspySettings -> setValue ("lna", lnaGain);
	airspySettings	-> endGroup ();
	myFrame	-> hide ();
	if (Handle == NULL)
	   goto err;
	if (device) {
	   int result = my_airspy_stop_rx (device);
	   if (result != AIRSPY_SUCCESS) {
	      printf ("my_airspy_stop_rx () failed: %s (%d)\n",
	             my_airspy_error_name((airspy_error)result), result);
	   }

	   if (rf_bias)
	      set_rf_bias ();
	   result = my_airspy_close (device);
	   if (result != AIRSPY_SUCCESS) {
	      printf ("airspy_close () failed: %s (%d)\n",
	             my_airspy_error_name((airspy_error)result), result);
	   }
	}
	my_airspy_exit ();
	if (Handle != NULL) 
#ifdef __MINGW32__
	   FreeLibrary (Handle);
#else
	   dlclose (Handle);
#endif
err:
	delete	myFrame;
	delete[] convBuffer;
	if (theBuffer != NULL)
	   delete theBuffer;
}

void	airspyHandler::setVFOFrequency (int32_t nf) {
int	result;
	if (!deviceOK)
	   return;
	result = my_airspy_set_freq (device, lastFrequency = nf);

	if (result != AIRSPY_SUCCESS) {
	   printf ("my_airspy_set_freq() failed: %s (%d)\n",
	            my_airspy_error_name((airspy_error)result), result);
	}
}

int32_t	airspyHandler::getVFOFrequency (void) {
	return lastFrequency;
}

bool	airspyHandler::legalFrequency (int32_t f) {
	return Khz (24000) <= f && f <= Khz (1750000);
}

int32_t	airspyHandler::defaultFrequency (void) {
	return Khz (94700);
}

bool	airspyHandler::restartReader	(void) {
int	result;
int32_t	bufSize	= EXTIO_NS * EXTIO_BASE_TYPE_SIZE * 2;
	if (running || !deviceOK)
	   return true;

	theBuffer	-> FlushRingBuffer ();

	if (currentTab == 0)
           set_sensitivity      (sensitivitySlider -> value ());
        else
        if (currentTab == 1)
           set_linearity        (linearitySlider -> value ());
        else {
           set_vga_gain         (vgaGain);
           set_mixer_gain       (mixerGain);
           set_lna_gain         (lnaGain);
        }

	set_vga_gain	(vgaGain);
	set_mixer_gain	(mixerGain);
	set_lna_gain	(lnaGain);
	
	result = my_airspy_start_rx (device,
	            (airspy_sample_block_cb_fn)callback, this);
	if (result != AIRSPY_SUCCESS) {
	   printf ("my_airspy_start_rx () failed: %s (%d)\n",
	         my_airspy_error_name((airspy_error)result), result);
	   return false;
	}
//
//	finally:
	buffer = new uint8_t [bufSize];
	bs_ = bufSize;
	bl_ = 0;
	running	= true;
	return true;
}

void	airspyHandler::stopReader (void) {
	if (!running || !deviceOK)
	   return;
int result = my_airspy_stop_rx (device);

	if (result != AIRSPY_SUCCESS ) {
	   printf ("my_airspy_stop_rx() failed: %s (%d)\n",
	          my_airspy_error_name ((airspy_error)result), result);
	} else {
	   delete [] buffer;
	   bs_ = bl_ = 0 ;
	}
	running	= false;
}
//
//	Directly copied from the airspy extio dll from Andrea Montefusco
int airspyHandler::callback (airspy_transfer* transfer) {
airspyHandler *p;

	if (!transfer)
	   return 0;		// should not happen
	p = static_cast<airspyHandler *> (transfer -> ctx);

// AIRSPY_SAMPLE_FLOAT32_IQ:
	uint32_t bytes_to_write = transfer -> sample_count * 4 * 2; 
	uint8_t *pt_rx_buffer   = (uint8_t *)transfer->samples;
	
	while (bytes_to_write > 0) {
	   int spaceleft = p -> bs_ - p -> bl_ ;
	   int to_copy = std::min ((int)spaceleft, (int)bytes_to_write);
	   ::memcpy (p -> buffer + p -> bl_, pt_rx_buffer, to_copy);
	   bytes_to_write -= to_copy;
	   pt_rx_buffer   += to_copy;
//
//	   bs (i.e. buffersize) in bytes
	   if (p -> bl_ == p -> bs_) {
	      p -> data_available ((void *)p -> buffer, p -> bl_);
	      p->bl_ = 0;
	   }
	   p -> bl_ += to_copy;
	}
	return 0;
}

//	this method is declared in airspyHandler class
//	The buffer received from hardware contains
//	32-bit floating point IQ samples (8 bytes per sample)
//
//	recoded for the sdr-j framework
//	4*2 = 8 bytes for sample, as per AirSpy USB data stream format
//	we do the rate conversion here, read in groups of 625 samples
//	and transform them into groups of 480 samples
int 	airspyHandler::data_available (void *buf, int buf_size) {	
float	*sbuf	= (float *)buf;
int nSamples	= buf_size / (sizeof (float) * 2);
DSPCOMPLEX temp [1920];
int32_t  i, j;

	for (i = 0; i < nSamples; i ++) {
	   convBuffer [convIndex ++] = DSPCOMPLEX (sbuf [2 * i],
	                                           sbuf [2 * i + 1]);
	   if (convIndex > convBufferSize) {
	      for (j = 0; j < 1920; j ++) {
	         int16_t inpBase	= mapTable_int [j];
	         float   inpRatio	= mapTable_float [j];
	         temp [j] = cmul (convBuffer [inpBase + 1], inpRatio) +
	                    cmul (convBuffer [inpBase], 1 - inpRatio);
	      }
	      theBuffer	-> putDataIntoBuffer (temp, 1920);
//      shift the sample at the end to the beginning, it is needed
//      as the starting sample for the next time
              convBuffer [0] = convBuffer [convBufferSize];
              convIndex = 1;
           }
        }

	return 0;
}
//
//	Not used here
bool	airspyHandler::status (void) {
	return true;
}

const char *airspyHandler::getSerial (void) {
airspy_read_partid_serialno_t read_partid_serialno;
int result = my_airspy_board_partid_serialno_read (device,
	                                          &read_partid_serialno);
	if (result != AIRSPY_SUCCESS) {
	   printf ("failed: %s (%d)\n",
	         my_airspy_error_name ((airspy_error)result), result);
	   return "UNKNOWN";
	} else {
	   snprintf (serial, sizeof(serial), "%08X%08X", 
	             read_partid_serialno. serial_no [2],
	             read_partid_serialno. serial_no [3]);
	}
	return serial;
}
//
//	not used here
int	airspyHandler::open (void) {
int result = my_airspy_open (&device);

	if (result != AIRSPY_SUCCESS) {
	   printf ("airspy_open() failed: %s (%d)\n",
	          my_airspy_error_name((airspy_error)result), result);
	   return -1;
	} else {
	   return 0;
	}
}

//
//	These functions are added for the SDR-J interface
void	airspyHandler::resetBuffer (void) {
	theBuffer	-> FlushRingBuffer ();
}

int16_t	airspyHandler::bitDepth (void) {
	return 12;
}

int32_t	airspyHandler::getRate (void) {
	return 1920000;
}

int32_t	airspyHandler::getSamples (DSPCOMPLEX *v, int32_t size) {
	return theBuffer	-> getDataFromBuffer (v, size);
}

int32_t	airspyHandler::Samples	(void) {
	return theBuffer	-> GetRingBufferReadAvailable ();
}
//
uint8_t	airspyHandler::myIdentity		(void) {
	return AIRSPY;
}
//

/* Parameter value shall be between 0 and 15 */
void	airspyHandler::set_lna_gain (int value) {
int result = my_airspy_set_lna_gain (device, lnaGain = value);

	if (result != AIRSPY_SUCCESS) {
	   printf ("airspy_set_lna_gain () failed: %s (%d)\n",
	          my_airspy_error_name((airspy_error)result), result);
	}
	else
	   lnaDisplay	-> display (value);
}

/* Parameter value shall be between 0 and 15 */
void	airspyHandler::set_mixer_gain (int value) {
int result = my_airspy_set_mixer_gain(device, mixerGain = value);

	if (result != AIRSPY_SUCCESS) {
	   printf("airspy_set_mixer_gain() failed: %s (%d)\n",
	         my_airspy_error_name((airspy_error)result), result);
	}
	else
	   mixerDisplay	-> display (value);
}

/* Parameter value shall be between 0 and 15 */
void	airspyHandler::set_vga_gain (int value) {
int result = my_airspy_set_vga_gain(device, vgaGain = value);

	if (result != AIRSPY_SUCCESS) {
	   printf ("airspy_set_vga_gain () failed: %s (%d)\n",
	          my_airspy_error_name ((airspy_error)result), result);
	}
	else
	   vgaDisplay	-> display (value);
}
//
//
//	agc's
/* Parameter value:
	0=Disable LNA Automatic Gain Control
	1=Enable LNA Automatic Gain Control
*/
void	airspyHandler::set_lna_agc (void) {
	lna_agc	= !lna_agc;
int result = my_airspy_set_lna_agc (device, lna_agc ? 1 : 0);

	if (result != AIRSPY_SUCCESS) {
	   printf ("airspy_set_lna_agc() failed: %s (%d)\n",
	            my_airspy_error_name ((airspy_error)result), result);
	}
}

/* Parameter value:
	0=Disable MIXER Automatic Gain Control
	1=Enable MIXER Automatic Gain Control
*/
void	airspyHandler::set_mixer_agc (void) {
	mixer_agc	= !mixer_agc;

int result = my_airspy_set_mixer_agc (device, mixer_agc ? 1 : 0);

	if (result != AIRSPY_SUCCESS) {
	   printf ("airspy_set_mixer_agc () failed: %s (%d)\n",
	            my_airspy_error_name ((airspy_error)result), result);
	}
}


/* Parameter value shall be 0=Disable BiasT or 1=Enable BiasT */
void	airspyHandler::set_rf_bias (void) {
	rf_bias	= !rf_bias;
int result = my_airspy_set_rf_bias (device, rf_bias ? 1 : 0);

	if (result != AIRSPY_SUCCESS) {
	   printf("airspy_set_rf_bias() failed: %s (%d)\n",
	           my_airspy_error_name ((airspy_error)result), result);
	}
}


const char* airspyHandler::board_id_name (void) {
uint8_t bid;

	if (my_airspy_board_id_read (device, &bid) == AIRSPY_SUCCESS)
	   return my_airspy_board_id_name ((airspy_board_id)bid);
	else
	   return "UNKNOWN";
}
//
//
bool	airspyHandler::load_airspyFunctions (void) {
//
//	link the required procedures
	my_airspy_init	= (pfn_airspy_init)
	                       GETPROCADDRESS (Handle, "airspy_init");
	if (my_airspy_init == NULL) {
	   fprintf (stderr, "Could not find airspy_init\n");
	   return false;
	}

	my_airspy_exit	= (pfn_airspy_exit)
	                       GETPROCADDRESS (Handle, "airspy_exit");
	if (my_airspy_exit == NULL) {
	   fprintf (stderr, "Could not find airspy_exit\n");
	   return false;
	}

	my_airspy_open	= (pfn_airspy_open)
	                       GETPROCADDRESS (Handle, "airspy_open");
	if (my_airspy_open == NULL) {
	   fprintf (stderr, "Could not find airspy_open\n");
	   return false;
	}

	my_airspy_close	= (pfn_airspy_close)
	                       GETPROCADDRESS (Handle, "airspy_close");
	if (my_airspy_close == NULL) {
	   fprintf (stderr, "Could not find airspy_close\n");
	   return false;
	}

	my_airspy_get_samplerates	= (pfn_airspy_get_samplerates)
	                       GETPROCADDRESS (Handle, "airspy_get_samplerates");
	if (my_airspy_get_samplerates == NULL) {
	   fprintf (stderr, "Could not find airspy_get_samplerates\n");
	   return false;
	}

	my_airspy_set_samplerate	= (pfn_airspy_set_samplerate)
	                       GETPROCADDRESS (Handle, "airspy_set_samplerate");
	if (my_airspy_set_samplerate == NULL) {
	   fprintf (stderr, "Could not find airspy_set_samplerate\n");
	   return false;
	}

	my_airspy_start_rx	= (pfn_airspy_start_rx)
	                       GETPROCADDRESS (Handle, "airspy_start_rx");
	if (my_airspy_start_rx == NULL) {
	   fprintf (stderr, "Could not find airspy_start_rx\n");
	   return false;
	}

	my_airspy_stop_rx	= (pfn_airspy_stop_rx)
	                       GETPROCADDRESS (Handle, "airspy_stop_rx");
	if (my_airspy_stop_rx == NULL) {
	   fprintf (stderr, "Could not find airspy_stop_rx\n");
	   return false;
	}

	my_airspy_set_sample_type	= (pfn_airspy_set_sample_type)
	                       GETPROCADDRESS (Handle, "airspy_set_sample_type");
	if (my_airspy_set_sample_type == NULL) {
	   fprintf (stderr, "Could not find airspy_set_sample_type\n");
	   return false;
	}

	my_airspy_set_freq	= (pfn_airspy_set_freq)
	                       GETPROCADDRESS (Handle, "airspy_set_freq");
	if (my_airspy_set_freq == NULL) {
	   fprintf (stderr, "Could not find airspy_set_freq\n");
	   return false;
	}

	my_airspy_set_lna_gain	= (pfn_airspy_set_lna_gain)
	                       GETPROCADDRESS (Handle, "airspy_set_lna_gain");
	if (my_airspy_set_lna_gain == NULL) {
	   fprintf (stderr, "Could not find airspy_set_lna_gain\n");
	   return false;
	}

	my_airspy_set_mixer_gain	= (pfn_airspy_set_mixer_gain)
	                       GETPROCADDRESS (Handle, "airspy_set_mixer_gain");
	if (my_airspy_set_mixer_gain == NULL) {
	   fprintf (stderr, "Could not find airspy_set_mixer_gain\n");
	   return false;
	}

	my_airspy_set_vga_gain	= (pfn_airspy_set_vga_gain)
	                       GETPROCADDRESS (Handle, "airspy_set_vga_gain");
	if (my_airspy_set_vga_gain == NULL) {
	   fprintf (stderr, "Could not find airspy_set_vga_gain\n");
	   return false;
	}

	my_airspy_set_linearity_gain = (pfn_airspy_set_linearity_gain)
	                       GETPROCADDRESS (Handle, "airspy_set_linearity_gain");
	if (my_airspy_set_linearity_gain == NULL) {
	   fprintf (stderr, "Could not find airspy_set_linearity_gain\n");
	   fprintf (stderr, "You probably did not install 1.0.7 yet\n");
	   return false;
	}

	my_airspy_set_sensitivity_gain = (pfn_airspy_set_sensitivity_gain)
	                       GETPROCADDRESS (Handle, "airspy_set_sensitivity_gain");
	if (my_airspy_set_sensitivity_gain == NULL) {
	   fprintf (stderr, "Could not find airspy_set_sensitivity_gain\n");
	   fprintf (stderr, "You probably did not install 1.0.7 yet\n");
	   return false;
	}


	my_airspy_set_lna_agc	= (pfn_airspy_set_lna_agc)
	                       GETPROCADDRESS (Handle, "airspy_set_lna_agc");
	if (my_airspy_set_lna_agc == NULL) {
	   fprintf (stderr, "Could not find airspy_set_lna_agc\n");
	   return false;
	}

	my_airspy_set_mixer_agc	= (pfn_airspy_set_mixer_agc)
	                       GETPROCADDRESS (Handle, "airspy_set_mixer_agc");
	if (my_airspy_set_mixer_agc == NULL) {
	   fprintf (stderr, "Could not find airspy_set_mixer_agc\n");
	   return false;
	}

	my_airspy_set_rf_bias	= (pfn_airspy_set_rf_bias)
	                       GETPROCADDRESS (Handle, "airspy_set_rf_bias");
	if (my_airspy_set_rf_bias == NULL) {
	   fprintf (stderr, "Could not find airspy_set_rf_bias\n");
	   return false;
	}

	my_airspy_error_name	= (pfn_airspy_error_name)
	                       GETPROCADDRESS (Handle, "airspy_error_name");
	if (my_airspy_error_name == NULL) {
	   fprintf (stderr, "Could not find airspy_error_name\n");
	   return false;
	}

	my_airspy_board_id_read	= (pfn_airspy_board_id_read)
	                       GETPROCADDRESS (Handle, "airspy_board_id_read");
	if (my_airspy_board_id_read == NULL) {
	   fprintf (stderr, "Could not find airspy_board_id_read\n");
	   return false;
	}

	my_airspy_board_id_name	= (pfn_airspy_board_id_name)
	                       GETPROCADDRESS (Handle, "airspy_board_id_name");
	if (my_airspy_board_id_name == NULL) {
	   fprintf (stderr, "Could not find airspy_board_id_name\n");
	   return false;
	}

	my_airspy_board_partid_serialno_read	=
	                (pfn_airspy_board_partid_serialno_read)
	                       GETPROCADDRESS (Handle, "airspy_board_partid_serialno_read");
	if (my_airspy_board_partid_serialno_read == NULL) {
	   fprintf (stderr, "Could not find airspy_board_partid_serialno_read\n");
	   return false;
	}

	return true;
}

#define GAIN_COUNT (22)

uint8_t airspy_linearity_vga_gains[GAIN_COUNT] = { 13, 12, 11, 11, 11, 11, 11, 10, 10, 10, 10, 10, 10, 10, 10, 10, 9, 8, 7, 6, 5, 4 };
uint8_t airspy_linearity_mixer_gains[GAIN_COUNT] = { 12, 12, 11, 9, 8, 7, 6, 6, 5, 0, 0, 1, 0, 0, 2, 2, 1, 1, 1, 1, 0, 0 };
uint8_t airspy_linearity_lna_gains[GAIN_COUNT] = { 14, 14, 14, 13, 12, 10, 9, 9, 8, 9, 8, 6, 5, 3, 1, 0, 0, 0, 0, 0, 0, 0 };
uint8_t airspy_sensitivity_vga_gains[GAIN_COUNT] = { 13, 12, 11, 10, 9, 8, 7, 6, 5, 5, 5, 5, 5, 4, 4, 4, 4, 4, 4, 4, 4, 4 };
uint8_t airspy_sensitivity_mixer_gains[GAIN_COUNT] = { 12, 12, 12, 12, 11, 10, 10, 9, 9, 8, 7, 4, 4, 4, 3, 2, 2, 1, 0, 0, 0, 0 };
uint8_t airspy_sensitivity_lna_gains[GAIN_COUNT] = { 14, 14, 14, 14, 14, 14, 14, 14, 14, 13, 12, 12, 9, 9, 8, 7, 6, 5, 3, 2, 1, 0 };

void	airspyHandler::set_linearity (int value) {
int	result = my_airspy_set_linearity_gain (device, value);
int	temp;
	if (result != AIRSPY_SUCCESS) {
	   printf ("airspy_set_lna_gain () failed: %s (%d)\n",
	            my_airspy_error_name ((airspy_error)result), result);
	   return;
	}
	linearityDisplay	-> display (value);
	temp	= airspy_linearity_lna_gains [GAIN_COUNT - 1 - value];
	linearity_lnaDisplay	-> display (temp);
	temp	= airspy_linearity_mixer_gains [GAIN_COUNT - 1 - value];
	linearity_mixerDisplay	-> display (temp);
	temp	= airspy_linearity_vga_gains [GAIN_COUNT - 1 - value];
	linearity_vgaDisplay	-> display (temp);
}

void	airspyHandler::set_sensitivity (int value) {
int	result = my_airspy_set_sensitivity_gain (device, value);
int	temp;
	if (result != AIRSPY_SUCCESS) {
	   printf ("airspy_set_mixer_gain() failed: %s (%d)\n",
	            my_airspy_error_name ((airspy_error)result), result);
	   return;
	}
	sensitivityDisplay	-> display (value);
	temp	= airspy_sensitivity_lna_gains [GAIN_COUNT - 1 - value];
	sensitivity_lnaDisplay	-> display (temp);
	temp	= airspy_sensitivity_mixer_gains [GAIN_COUNT - 1 - value];
	sensitivity_mixerDisplay	-> display (temp);
	temp	= airspy_sensitivity_vga_gains [GAIN_COUNT - 1 - value];
	sensitivity_vgaDisplay	-> display (temp);
}

void	airspyHandler::show_tab (int t) {
	if (t == 0)		// sensitivity
	   set_sensitivity	(sensitivitySlider -> value ());
	else
	if (t == 1)		// linearity
	   set_linearity	(linearitySlider -> value ());
	else {			// classic view
	   set_vga_gain		(vgaGain);
	   set_mixer_gain	(mixerGain);
	   set_lna_gain		(lnaGain);
	}
	currentTab	= t;
}

