/******************************************************************************
 *  FILE:               : can_encoder.h
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

#ifndef __CAN_ENCODER_PUBLIC_H__
#define __CAN_ENCODER_PUBLIC_H__
#include "can_encoder.c"

/**
  * Init the encryption variables
  * @param {int} personal_key Personal value to authenticate the messages (max. 255)
  * @param {CanMessage *} start_msg Pointer to the 8 bytes of start round message
  * @return 1 if the message signature match with croupier signature, -1 otherwise
  */
extern char init_encoding(unsigned int personal_key, CanMessage * start_msg);

/**
  * Generates an encrypted and singed message of value
  * @param {byte} value Number to be encrypted (0 - 3)
  * @return Pointer to the 8 bytes encrypted message
  */
extern CanMessage * encode_coins(unsigned char value);

/**
  * Generates a signed message of value with the personal_key
  * @param {int} value Number to be send (will not be encrypted)
  * @return Pointer to the 8 bytes signed message
  */
extern CanMessage * sign_initial_guess(unsigned int value);

/**
  * Generates a signed message of value with the personal_key
  * @param {int} value Number to be send (will not be encrypted)
  * @return Pointer to the 8 bytes signed message
  */
extern CanMessage * sign_final_guess(unsigned int value);

/**
  * Check if message signature matches with croupier signature
  * @param {CanMessage *} msg Pointer to the 8 bytes message
  * @param {unsigned long} msg_id CAN message identifier
  * @return 1 if the signatures match, -1 otherwise
  */
extern char check_croupier_signature(CanMessage * msg, unsigned long msg_id);

#endif

