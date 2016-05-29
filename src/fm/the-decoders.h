#
#include	<stdint.h>
#include	"Xtan2.h"
#include	"pllC.h"

class	decoderBase {
public:
		decoderBase	(void) {
}

virtual		~decoderBase	(void) {
}

virtual	float	decode		(DSPCOMPLEX z) {
	(void)z;
	return 0.0;
}
};

//
//	Different based demodulator
class	decoder_1: public decoderBase {
private:
	float	Imin1;
	float	Qmin1;
	float	Imin2;
	float	Qmin2;
public:
		decoder_1 	(void) {
	Imin1	= 1;
	Qmin1	= 1;
	Imin2	= 1;
	Qmin2	= 1;
}

		~decoder_1	(void) {
}

float		decode		(DSPCOMPLEX z) {
float	res;
float	I, Q;

	if (jan_abs (z) <= 0.001)
	   I = Q = 0.001;	// do not make these 0 too often
	else { 
	   I = real (z) / abs (z);
	   Q = imag (z) / abs (z);
	}

	res	= Imin1 * (Q - Qmin2) - Qmin1 * (I - Imin2);
	res	/= Imin1 * Imin1 + Qmin1 * Qmin1;
	Imin2	= Imin1;
	Qmin2	= Qmin1;
	Imin1	= I;
	Qmin1	= Q;
	return res;
}
};

//
//	Complex baseband delay demodulator

class	decoder_2:public decoderBase {
private:
	float		Imin1;
	float		Qmin1;
	DSPCOMPLEX	old_z;
public:
		decoder_2 	(void) {
}

		~decoder_2	(void) {
}

float		decode		(DSPCOMPLEX z) {
float	res;
float	I, Q;

	if (jan_abs (z) <= 0.001)
	   I = Q = 0.001;	// do not make these 0 too often
	else { 
	   I = real (z) / abs (z);
	   Q = imag (z) / abs (z);
	}

	z	= DSPCOMPLEX (I, Q);
	res	= arg (z * conj (old_z));
	old_z	= z;
	return res;
}
};
//
//	Mixed demodulator
class	decoder_3:public decoderBase {
private:
	float		Imin1;
	float		Qmin1;
	compAtan	myAtan;
public:
		decoder_3 	(void) {
	Imin1	= 1;
	Qmin1	= 1;
}

		~decoder_3	(void) {
}

float		decode		(DSPCOMPLEX z) {
float	res;
float	I, Q;

	if (jan_abs (z) <= 0.001)
	   I = Q = 0.001;	// do not make these 0 too often
	else { 
	   I = real (z) / abs (z);
	   Q = imag (z) / abs (z);
	}

	res	= myAtan. atan2 (Q * Imin1- I * Qmin1,
	                                 I * Imin1 + Q * Qmin1);
	Imin1	= I;
	Qmin1	= Q;
	return res;
}
};
//
//	PLL based decoder
//
class	decoder_4:public decoderBase {
private:
	pllC	myfm_pll;
public:
		decoder_4 	(int32_t rateIn,
	                         SinCos	*mySinCos):
	                              myfm_pll (rateIn,
	                                        0, 
	                            -0.95 * (0.5 * rateIn),
	                             0.95 * (0.5 * rateIn),
	                             0.85 * rateIn,
	                             mySinCos) {
}

		~decoder_4	(void) {
}

float		decode		(DSPCOMPLEX z) {
float	I, Q;

	if (jan_abs (z) <= 0.001)
	   I = Q = 0.001;	// do not make these 0 too often
	else { 
	   I = real (z) / abs (z);
	   Q = imag (z) / abs (z);
	}

	z	= DSPCOMPLEX (I, Q);
	myfm_pll. do_pll (z);
	return myfm_pll. getPhaseIncr ();
}
};

class	decoder_5:public decoderBase {
private:
	float		Imin1;
	float		Qmin1;
	int32_t		ArcsineSize;
	float		*Arcsine;
public:
		decoder_5 	(void) {
int32_t	i;
	Imin1		= 1;
	Qmin1		= 1;
	ArcsineSize	= 4 * 8192;
	Arcsine		= new float [ArcsineSize];
	for (i = 0; i < ArcsineSize; i ++)
	   Arcsine [i] = asin (2.0 * i / ArcsineSize - 1.0) / 2.0;
}

		~decoder_5	(void) {
}

float		decode		(DSPCOMPLEX z) {
float	res;
float	I, Q;

	if (jan_abs (z) <= 0.001)
	   I = Q = 0.001;	// do not make these 0 too often
	else { 
	   I = real (z) / abs (z);
	   Q = imag (z) / abs (z);
	}

	res	= (Imin1 * Q - Qmin1 * I + 1.0) / 2.0;
	res	= Arcsine [(int)(res * ArcsineSize)];
	Imin1	= I;
	Qmin1	= Q;
	return res;
}
};


