/******************************************************************************
 *  FILE:               : can_encoder.c
 *  DESCRIPTION         : API to encode CAN messages based on RSA protocol
 *  CPU TYPE            : dsPIC33FJ256MC710
 *  AUTHOR              : Jaume Bosch Pons
 *  COMPANY             : STR - FIB - UPC
 *
 *  REVISION HISTORY:
 *            VERSION:    1.0
 *            AUTHOR:     Jaume Bosch Pons
 *            DATE:       19th May 2014
 *            COMMENTS:   MOCK VERSION
 *****************************************************************************/

#ifndef __CAN_ENCODER_MOCK_C__
#define __CAN_ENCODER_MOCK_C__

#include <stdio.h>

//#define CanMessage unsigned int       // PC testing
#define CanMessage unsigned long int    // PIC working


// The following variables will be set up with init_encoding
// This values are to aviod random and non safe values
// Length types for PIC can be found on link http://goo.gl/wjAFHA
static unsigned int personalKey = 0;
static unsigned int exponent = 10;
static unsigned long int base = 10;
static CanMessage msg[2] = {0, 0};

char init_encoding(unsigned int personal_key, CanMessage * start_msg) {
		// Set the variables
		personalKey = personal_key;
		exponent = (unsigned int)start_msg[0];
		base = (unsigned long int)start_msg[1];

		return 1;
}

CanMessage * encode_coins(unsigned char value) {
		// Merge message parts
		msg[0] = (CanMessage)value;
		msg[1] = (CanMessage)rand();

		return (CanMessage *)&msg[0];
}

CanMessage * sign_guess(unsigned int value, unsigned char factor) {
		// Merge message parts
		msg[0] = value;
		msg[1] = rand() << factor;

		return (CanMessage *)(&msg[0]);
}

CanMessage * sign_initial_guess(unsigned int value) {
		return sign_guess(value, 2);
}

CanMessage * sign_final_guess(unsigned int value) {
		return sign_guess(value, 3);
}

char check_croupier_signature(CanMessage * msg, unsigned long msg_id) {
		// Check if signatures matches
		return 1;
}

#endif

