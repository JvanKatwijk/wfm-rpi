#
/*
 *    Copyright (C)  2014
 *    Jan van Katwijk (J.vanKatwijk@gmail.com)
 *    Lazy Chair Programming
 *
 *    This part of the FM demodulation software is partly based on
 *    the description of CuteSDR
 *    (c) Moey Wheatly 2011
 *    and 
 *    FMSTACK software
 *    Technical University of Munich, Institute for Integrated Systems (LIS)
 *    FMSTACK Copyright
 *    (C) 2010 Michael Feilen
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
 */
#include	"rds-blocksynchronizer.h"
#include	"gui.h"

uint16_t	H [] = {
	0x2DC,		//10 1101 1100		0
	0x16E,		//01 0110 1110		1
	0x0B7,		//00 1011 0111		2
	0x287,		//10 1000 0111		3
	0x39F,		//11 1001 1111		4
	0x313,		//11 0001 0011		5
	0x355,		//11 0101 0101		6
	0x376,		//11 0111 0110		7
	0x1BB,		//01 1011 1011		8
	0x201,		//10 0000 0001		9
	0x3DC,		//11 1101 1100		10
	0x1EE,		//01 1110 1110		11
	0x0F7,		//00 1111 0111		12
	0x2A7,		//10 1010 0111		13
	0x38F,		//11 1000 1111		14
	0x31B		//11 0001 1011		15
};


	
	
	rdsBlockSynchronizer::rdsBlockSynchronizer (RadioInterface *RI) {
	MyRadioInterface	= RI;
	crcFecEnabled		= true; 
	
	this -> reset ();
	connect (this, SIGNAL (setRDSisSynchronized (bool)),
	         MyRadioInterface, SLOT (setRDSisSynchronized (bool)));
	connect (this, SIGNAL (setbitErrorRate (double)),
	         MyRadioInterface, SLOT (setbitErrorRate (double)));
}

	rdsBlockSynchronizer::~rdsBlockSynchronizer (void) {
	disconnect (this, SIGNAL (setRDSisSynchronized (bool)),
	            MyRadioInterface, SLOT (setRDSisSynchronized (bool)));
	disconnect (this, SIGNAL (setbitErrorRate (double)),
	            MyRadioInterface, SLOT (setbitErrorRate (double)));
}

void	rdsBlockSynchronizer::setFecEnabled (bool b) {
	crcFecEnabled	= b;
}

void	rdsBlockSynchronizer::reset	(void) {
	rdsBitstream		= 0;
	rdsIsSynchronized	= false;
	rdsCurrentBlock		= RDSGroup::BLOCK_A;
	rdsbitErrorRate		= 0;
	rdsBitsinBlock		= 0;
	rdsBitsProcessed	= 0;
	rdsNumofBitErrors	= 0;

	resetCRCErrorCounter	();
	resetResyncErrorCounter	();
	setRDSisSynchronized	(false);
}
//
//	cyclic shift to the next block
void	rdsBlockSynchronizer::setNextBlock	(void) {
	switch (rdsCurrentBlock) {
	   case RDSGroup::BLOCK_A:
	      rdsCurrentBlock	= RDSGroup::BLOCK_B;
	      break;
	   case RDSGroup::BLOCK_B:
	      rdsCurrentBlock	= RDSGroup::BLOCK_C;
	      break;
	   case RDSGroup::BLOCK_C:
	      rdsCurrentBlock	= RDSGroup::BLOCK_D;
	      break;
	   case RDSGroup::BLOCK_D:
	      rdsCurrentBlock	= RDSGroup::BLOCK_A;
	      break;
	}
}

int16_t	rdsBlockSynchronizer::getNumSyncErrors	(void) {
	return rdsNumofSyncErrors;
}

DSPFLOAT	rdsBlockSynchronizer::getBitErrorRate	(void) {
	return rdsbitErrorRate;
}

void	rdsBlockSynchronizer::resync	(void) {
	rdsCurrentBlock		= RDSGroup::BLOCK_A;
	rdsIsSynchronized	= false;
	setRDSisSynchronized (false);
	rdsBitsinBlock		= 0;
}

void	rdsBlockSynchronizer::resetResyncErrorCounter (void) {
	rdsNumofSyncErrors	= 0;
}

int16_t	rdsBlockSynchronizer::getNumCRCErrors		(void) {
	return rdsNumofCRCErrors;
}

void	rdsBlockSynchronizer::resetCRCErrorCounter 	(void) {
	rdsNumofCRCErrors	= 0;
}

uint32_t	rdsBlockSynchronizer::getSyndrome (uint32_t bits,
	                                           uint32_t offsetWord) {
uint32_t	syndrome	= (bits >> 16) & 0x3FF;
int16_t		i;

	bits ^= offsetWord;
	for (i = 0; i < 16; i ++) {
	   if (bits & 0x8000)
	      syndrome ^= H [i];
	   bits <<= 1;
	}
//	Note that due to many "xor"-ing operations
//	the "top" bits of the syndrome are polluted
	return syndrome & 0x3FF;
}

//
bool	rdsBlockSynchronizer::decodeBlock (RDSGroup::RdsBlock b,
	                                   uint32_t bits,
	                                   bool isTypeBGroup) {
uint32_t	offsetWord	= 0;
uint32_t	syndrome	= 0;

	offsetWord = getOffset (b, isTypeBGroup);
//	calculate  crc and get the syndrome
	syndrome	= getSyndrome (bits, offsetWord);

//	During synchronization we allow zero errors,
//	thus we check the syndrome register directly here
	if (!rdsIsSynchronized)
	   return syndrome == 0;

// Increment for BER calculations
	rdsBitsProcessed	+= NUM_BITS_BLOCK_PAYLOAD;

	if (syndrome != 0 && crcFecEnabled) 
	   correctErrors (syndrome);

//	if the syndrome is not equal to zero there was an error in the crc
//	When no fec is used, we mark all bits as erroneous
	else
	   if (syndrome != 0)
	      rdsNumofBitErrors += NUM_BITS_BLOCK_PAYLOAD;

//	calc BER
	rdsbitErrorRate	= (DSPFLOAT)rdsNumofBitErrors / rdsBitsProcessed;
	setbitErrorRate (rdsbitErrorRate);
	if (rdsBitsProcessed >= NUM_BITS_BER_CALC_RESET) {
	   rdsNumofBitErrors = 0;
	   rdsBitsProcessed = 0;
	}

	return syndrome == 0;
}
//
//	error correction for the payload. The algorithm is described in
//	the CuteSDR description of Moe Wheatley (doMeggit)
//	If the msb of the syndrome is "1" and the 5 least significant bits
//	of the syndrome are all zero, then the corresponding bit
//	is corrected by inverting it.

uint32_t rdsBlockSynchronizer::correctErrors (uint32_t syndrome) {
uint32_t	correctedBits	= 0;
uint32_t	correctMask	= (1 << (NUM_BITS_PER_BLOCK - 1));
uint16_t	i;

	for (i = 0; i < NUM_BITS_BLOCK_PAYLOAD; i ++) {
	   if (syndrome & 0x200) {
	      if ((syndrome & 0x1f) == 0) {	// lsb 5 bits
	         rdsBitstream ^= correctMask;
	         correctedBits ++;
	         rdsNumofBitErrors ++;
	      }
	      else {
	         syndrome ^= CRC_POLY;
	      }
	   }
	   syndrome <<= 1;
	   correctMask >>= 1;
	}

//	fprintf (stderr, "%d bits corrected\n", correctedBits);
	return syndrome & 0x3FF;
}

uint32_t	rdsBlockSynchronizer::getOffset (RDSGroup::RdsBlock b,
	                                         bool isTypeBGroup){
	switch (b) {
	   default:
	   case RDSGroup::BLOCK_A:
	      return OFFSET_WORD_BLOCK_A;

	   case RDSGroup::BLOCK_B:
	      return OFFSET_WORD_BLOCK_B;

	   case RDSGroup::BLOCK_C:
	      return isTypeBGroup ? OFFSET_WORD_BLOCK_C2 : OFFSET_WORD_BLOCK_C1;

	   case RDSGroup::BLOCK_D:
	      return OFFSET_WORD_BLOCK_D;
	}
}

rdsBlockSynchronizer::SyncResult
	rdsBlockSynchronizer::pushBit	(bool b, RDSGroup *rdsGrp) {

	if (rdsIsSynchronized)
	   return pushBitSynchronized (b, rdsGrp);

	// not synchronized, distinguish block A from the rest
	if (rdsCurrentBlock == RDSGroup::BLOCK_A)
	   return pushBitinBlockA (b, rdsGrp);
	else
	   return pushBitNotSynchronized (b, rdsGrp);
}

rdsBlockSynchronizer::SyncResult
	rdsBlockSynchronizer::pushBitSynchronized (bool b, RDSGroup *rdsGrp) {
//
//	assert rdsIsSynchronized == true
	rdsBitstream = (rdsBitstream << 1) | (b ? 01 : 00);
	if (++ rdsBitsinBlock < NUM_BITS_PER_BLOCK) 
	   return RDS_BUFFERING;

	rdsBitsinBlock = 0;
// decode bitstream for current block
	if (!decodeBlock (rdsCurrentBlock,
	                  rdsBitstream,
	                  rdsGrp -> isTypeBGroup ())) {
	   rdsNumofCRCErrors ++;
	   return RDS_NO_CRC;
	}

// Extract payload data
	rdsGrp -> setBlock (rdsCurrentBlock,
	                    (uint16_t)(rdsBitstream >> NUM_BITS_CRC));
//
//	check to see whether we are at the end of the group
	SyncResult result = (rdsCurrentBlock == RDSGroup::BLOCK_D)?
	                         RDS_COMPLETE_GROUP : RDS_BUFFERING;
	setNextBlock ();
	return result;
}
//
//	The first block is block-a. We keep shifting bits
//	until we have a valid block a
//
rdsBlockSynchronizer::SyncResult
	rdsBlockSynchronizer::pushBitinBlockA (bool b, RDSGroup *rdsGrp) {
uint32_t	offsetWord	= 0;
uint32_t	syndrome	= 0;

//	assert rdsIsSynchronized != true and rdsCurrentBlock == BLOCK_A
	rdsBitstream	= (rdsBitstream << 1) | (b ? 01 : 00);
	offsetWord	= getOffset (RDSGroup::BLOCK_A,
	                                     rdsGrp ->  isTypeBGroup ());
//	calculate  crc and get the syndrome
	syndrome	= getSyndrome (rdsBitstream & 0x3FFFFFF, offsetWord);

//	During synchronization phase NO errors are allowed
//	in case of error, we continue shifting since we are in BLOCK_A
	if (syndrome != 0)	// still errors
	   return RDS_WAITING_FOR_BLOCK_A;
//
//	syndrome == 0, extract the 16 bit data, we know
//	it is block A
	rdsGrp -> setBlock (RDSGroup::BLOCK_A,
	                    (uint16_t)(rdsBitstream >>
	                                                  NUM_BITS_CRC));
//
//	go for the next block
	rdsBitsinBlock		= 0;
	setNextBlock ();
	return RDS_BUFFERING;
}

rdsBlockSynchronizer::SyncResult
	rdsBlockSynchronizer::pushBitNotSynchronized (bool b, RDSGroup *rdsGrp) {
uint32_t	offsetWord	= 0;
uint32_t	syndrome	= 0;
//
//	assert rdsIsSynchronized != true and rdsCurrentBlock != BLOCK_A
	rdsBitstream = (rdsBitstream << 1) | (b ? 01 : 00);
	if (rdsBitsinBlock < NUM_BITS_PER_BLOCK - 1) {
	   rdsBitsinBlock ++;
	   return RDS_BUFFERING;
	}

	rdsBitsinBlock = 0;

	offsetWord = getOffset (rdsCurrentBlock,
	                        rdsGrp -> isTypeBGroup ());
//	calculate  crc and get the syndrome
	syndrome	= getSyndrome (rdsBitstream, offsetWord);

//	we want zero errors during synchronization
//	thus we check the syndrome register directly here
	if (syndrome != 0) {
//	error during synchronization
	   rdsNumofSyncErrors ++;
	   return RDS_NO_SYNC;
	}

// Extract payload data
	rdsGrp -> setBlock (rdsCurrentBlock,
	                    (uint16_t)(rdsBitstream >> NUM_BITS_CRC));
//
//	Look for next blocks in the group
//
	if (rdsCurrentBlock < SYNC_END_BLOCK) {
	   setNextBlock ();
	   return RDS_BUFFERING;
	}
//
//	if we are here, we reached the end block and
//	are synchronized, show it to the world
	rdsIsSynchronized	= true;
	setRDSisSynchronized	(true);
//
//	check to see whether sync was successful
	SyncResult result = (rdsCurrentBlock == RDSGroup::BLOCK_D)?
	                         RDS_COMPLETE_GROUP : RDS_BUFFERING;
	setNextBlock ();
	return result;
}

