/******************************************************************************
 *  FILE		 	: uart_dma.c
 *  DESCRIPTION  	: uart(RS-232) configuration using dma
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
 * 			COMMENTS: Adapted from Microchip CE214_UART_loopback demo
 *****************************************************************************/

#include "ee.h"
#include "cpu/pic30/inc/ee_irqstub.h"
#include "uart_dma.h"

//  Allocate two buffers for DMA transfers
unsigned char OutBuffer[OUTPUT_DATA_SIZE] __attribute__((space(dma)));
unsigned char InBufferA[INPUT_DATA_SIZE] __attribute__((space(dma)));
//unsigned char InBufferB[INPUT_DATA_SIZE] __attribute__((space(dma)));

// UART1 Configuration
void cfgUart1(void)
{
	U1MODEbits.STSEL = 0;// 1-stop bit
	U1MODEbits.PDSEL = 0;// No Parity, 8-data bits
	U1MODEbits.ABAUD = 0;// Autobaud Disabled

	U1MODEbits.BRGH=0;/* 1 = BRG generates 4 clocks per bit period (4x baud clock,
					   * High-Speed mode)
	                   * 0 = BRG generates 16 clocks per bit period (16x baud clock,
	                   * Standard mode)*/

	U1BRG = BRGVAL;	 //BRGVAL=((FCY/BITRATE1)/16)-1 //With BRGH=0-->Slow Bitrate
	                 //BRGVAL=((FCY/BITRATE1)/4)-1  //With BRGH=1-->Fast Bitrate

	// Configure UART for DMA transfers
	U1STAbits.UTXISEL0 = 1;/* UTXISEL<1:0>: Transmission Interrupt Mode Selection bits
							* 11 =Reserved; do not use
						    * 10 =Interrupt when a character is transferred to the
						    * Transmit Shift Register, and as a result, the transmit
						    * buffer becomes empty
							* 01 =Interrupt when the last character is shifted out
							* of the Transmit Shift Register; all transmit	operations
							* are completed
							* 00 =Interrupt when a character is transferred to the
							* Transmit Shift Register (this implies there is at least
							* one character open in the transmit buffer)*/

	U1STAbits.URXISEL  = 1;/* 11 =Interrupt is set on UxRSR transfer making the
							* receive buffer full (i.e., has 4 data characters)
							* 10 =Interrupt is set on UxRSR transfer making the
							* receive buffer 3/4 full (i.e., has 3 data characters)
							* 0x =Interrupt is set when any character is received
							* and transferred from the UxRSR to the receive buffer.
							* Receive buffer has one or more characters.*/

	// Enable UART Rx and Tx
	U1MODEbits.UARTEN   = 1;// Enable UART
	U1STAbits.UTXEN     = 1;// Enable UART Tx

	IEC4bits.U1EIE = 0; /* UART1 Error Interrupt Enable bit
						 * 1 = Interrupt request has occurred
						 * 0 = Interrupt request has not occurred*/
}

// DMA4 configuration
void cfgDma4UartTx(void)
{
	// Associate DMA Channel 4 with UART Tx
	DMA4REQ = 0x000C; // Select UART1 Transmitter
	DMA4PAD = (volatile unsigned int) &U1TXREG; /* DMA channel 4 peripheral address
												 * register*/

	// Configure DMA Channel 4 to:
	// Transfer data from RAM to UART (DIR)
	// One-Shot mode (MODE)
	// Register Indirect with Post-Increment (AMODE)
	// Using single buffer (DMA4CNT)
	// #OUTPUT_DATA_SIZE transfers per buffer (CNT)
	// Transfer bytes (SIZE)
	DMA4CONbits.AMODE = 0;/* DMA Channel Operating Mode Select bits
						   * 11 = Reserved (will act as Peripheral Indirect
						   * Addressing mode)
						   * 10 = Peripheral Indirect Addressing mode
						   * 01 = Register Indirect without Post-Increment mode
						   * 00 = Register Indirect with Post-Increment mode */

	DMA4CONbits.MODE  = 1;/*MODE<1:0>: DMA Channel Operating Mode Select bits
						   * 11 = One-Shot, Ping-Pong modes enabled (one block
						   * transfer from/to each DMA RAM buffer)
						   * 10 = Continuous, Ping-Pong modes enabled
						   * 01 = One-Shot, Ping-Pong modes disabled
						   * 00 = Continuous, Ping-Pong modes disabled */

	DMA4CONbits.DIR = 1;/*1 = Read from DMA RAM address, write to peripheral address
						 *0 = Read from peripheral address, write to DMA RAM address*/

	DMA4CONbits.SIZE  = 1;/* Data Transfer Size bit
						   * 1 = Byte
						   * 0 = Word */

	DMA4CNT = OUTPUT_DATA_SIZE-1; //Number of DMA transfers = CNT<9:0> + 1.


	// Associate one buffer with Channel 0 for one-shot operation
	DMA4STA = __builtin_dmaoffset(OutBuffer);//DMA Channel 4 RAM start address register A

	// Enable DMA Interrupts
	IFS2bits.DMA4IF = 0; // Clear DMA Interrupt Flag
	//IEC2bits.DMA4IE  = 1; // Enable DMA interrupt
	IEC2bits.DMA4IE = 0; // Disable DMA interrupt
}

// DMA1 configuration
void cfgDma5UartRx(void)
{
	// Associate DMA Channel 1 with UART Rx
	DMA5REQ = 0x000B;// Select UART1 Receiver
	DMA5PAD = (volatile unsigned int) &U1RXREG;/* DMA channel 5 peripheral address
											    * register*/

	// Configure DMA Channel 5 to:
	// Transfer data from UART to DMA (DIR)
	// Continuous, no Ping-Pong (MODE)
	// Register Indirect with Post-Increment (AMODE)
	// Using single buffer (DMA4CNT)
	// #INPUT_DATA_SIZE transfers per buffer (CNT)
	// Transfer bytes (SIZE)
	DMA5CONbits.AMODE = 0;/* DMA Channel Operating Mode Select bits
						   * 11 = Reserved (will act as Peripheral Indirect
						   * Addressing mode)
						   * 10 = Peripheral Indirect Addressing mode
						   * 01 = Register Indirect without Post-Increment mode
						   * 00 = Register Indirect with Post-Increment mode */

	DMA5CONbits.MODE  = 0;/*MODE<1:0>: DMA Channel Operating Mode Select bits
						   * 11 = One-Shot, Ping-Pong modes enabled (one block
						   * transfer from/to each DMA RAM buffer)
						   * 10 = Continuous, Ping-Pong modes enabled
						   * 01 = One-Shot, Ping-Pong modes disabled
						   * 00 = Continuous, Ping-Pong modes disabled */

	DMA5CONbits.DIR = 0;/*1 = Read from DMA RAM address, write to peripheral address
						 *0 = Read from peripheral address, write to DMA RAM address*/

	DMA5CONbits.SIZE = 1;/* Data Transfer Size bit
						  * 1 = Byte
						  * 0 = Word */

	DMA5CNT = INPUT_DATA_SIZE-1;//Number of DMA transfers = CNT<9:0> + 1.

	// Associate buffer with Channel 5
	DMA5STA = __builtin_dmaoffset(InBufferA);

	// Enable DMA Interrupts
	IFS3bits.DMA5IF = 0; // Clear DMA5 interrupt

	IEC3bits.DMA5IE  = 1; // Enable DMA5 interrupt
	//IEC3bits.DMA5IE = 0; // Disable DMA5 interrupt

	// Enable DMA Channel 5 to receive UART data
	DMA5CONbits.CHEN = 1; // Enable DMA Channel

}


//	Setup DMA interrupt handlers
ISR2(_DMA4Interrupt) //NOTE: Disabled when IEC2bits.DMA4IE = 0;
{
	IFS2bits.DMA4IF = 0;// Clear the DMA4 Interrupt Flag;
}


float k_updated[2]={0,0};

ISR2(_DMA5Interrupt) //NOTE: Disabled when IEC3bits.DMA5IE = 0;
{
	float *p_k0=(float *)&InBufferA[0];

	/*Code to update received data*/
	k_updated[0]= *(p_k0);
	k_updated[1]= *(p_k0+1);

	IFS3bits.DMA5IF = 0; // Clear the DMA1 Interrupt Flag
}

ISR2(_U1ErrInterrupt)
{
	IFS4bits.U1EIF = 0; // Clear the UART1 Error Interrupt Flag
}

void UART1_DMA_config(void)
{
	cfgDma4UartTx();// This routine Configures DMAchannel 4 for transmission.
	cfgDma5UartRx();// This routine Configures DMAchannel 5 for reception.
	cfgUart1();     // UART1 Configurations
}

