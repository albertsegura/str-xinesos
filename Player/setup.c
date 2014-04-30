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

#include "ee.h"
#include "cpu/pic30/inc/ee_irqstub.h"
#include "setup.h"
#include "e_can1.h"

/* Program the Timer1 peripheral to raise interrupts */
void T1_program(void)
{
	T1CON = 0;			/* Stops the Timer1 and reset control register	*/
	TMR1  = 0;			/* Clear contents of the timer register	*/
	PR1   = 0x9c40;		/* Load the Period register with the value of 1ms	*/
	IPC0bits.T1IP = 5;	/* Set Timer1 priority to 1. Higher values set higher priorities */
	IFS0bits.T1IF = 0;	/* Clear the Timer1 interrupt status flag	*/
	IEC0bits.T1IE = 1;	/* Enable Timer1 interrupts		*/
	T1CONbits.TON = 1;	/* Start Timer1 with prescaler settings at 1:1
						* and clock source set to the internal instruction cycle */
}

/* Clear the Timer1 interrupt status flag */
void T1_clear(void)
{
	IFS0bits.T1IF = 0;
}

/* This is an ISR Type 2 which is attached to the Timer 1 peripheral IRQ pin
 * The ISR simply calls CounterTick to implement the timing reference */
static unsigned int my_time=0;
ISR2(_T1Interrupt)
{
	/* clear the interrupt source */
	T1_clear();

	my_time++;/*Instead of using the TMR8 & TMR9 we use this instruction to get time*/

	/* count the interrupts, waking up expired alarms */
	CounterTick(myCounter);
}

/******************************************************************************
 * Function:	Led_config()
 * Description:	Configures  FLEX FULL orange led (Jumper 4 must be closed)
******************************************************************************/
void Led_config(void)
{
	/* set LED (LEDSYS/RB14) drive state low */
	LATBbits.LATB14 = 0;
	/* set LED pin (LEDSYS/RB14) as output */
	TRISBbits.TRISB14 = 0;
}


void Uart_init(void)
{
    U1MODEbits.UARTEN = 0;   // Stop UART
    U1MODEbits.STSEL = 0;    // 1 Stop bit
    U1MODEbits.PDSEL = 0;    // No Parity, 8 data bits
    U1BRG = (40000000/115200)/4-1; // Baud Rate Genrator Calculation
    U1MODEbits.BRGH = 1;     // 1 = BRG generates 4 clocks per bit period
                             //     (4x baud clock, High-Speed mode)
    U1MODEbits.UARTEN = 1;   // Enable UART
    U1STAbits.UTXEN = 1;     // Enable UART TX
}



/******************************************************************************
 * Function:	EE_Flex_setup()
 * Description:	Configures system clock and initialize devices
******************************************************************************/
void Sys_init(void)
{
	/* Clock setup for 40MIPS */
	CLKDIVbits.DOZEN   = 0;
	CLKDIVbits.PLLPRE  = 0;
	CLKDIVbits.PLLPOST = 0;
	PLLFBDbits.PLLDIV  = 78;

	/* Wait for PLL to lock */
	while(OSCCONbits.LOCK!=1);

	/* Program Timer 1 to raise interrupts */
	T1_program();

  	/* Time init (EDF scheduler) */
  	EE_time_init();

	/*Configure the orange led of the FLEX FULL */
	Led_config();

	/*Configure UART for RS232 tx */
	Uart_init();

	/*Configure enhanced CAN 1 */
	eCAN1_config();

}

