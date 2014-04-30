/******************************************************************************
 *  FILE		 	: setup.h
 *  DESCRIPTION  	: FLEX FULL board configuration
 *  CPU TYPE     	: dsPIC33FJ256MC710
 *  AUTHOR	     	: Antonio Camacho Santiago
 *  PROJECT	     	: DPI2007-61527
 *  COMPANY	     	: Automatic Control Department,
 *  				  Technical University of Catalonia
 *
 *  REVISION HISTORY:
 *			 VERSION: 0.1
 *     		  AUTHOR: Antonio Camacho Santiago
 * 				DATE: 27th April 2010
 * 			COMMENTS:
 *****************************************************************************/

#ifndef __INCLUDE_SETUP_H__
#define __INCLUDE_SETUP_H__

extern void T1_program(void);
extern void T1_clear(void);
extern void Led_config(void);
extern void Sys_init(void);

#endif
