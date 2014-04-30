/******************************************************************************
 *  FILE		 	: e_can1.h
 *  DESCRIPTION  	: enhanced CAN bus 1 configuration using dma
 *  CPU TYPE     	: dsPIC33FJ256MC710
 *  AUTHOR	     	: Antonio Camacho Santiago
 *  PROJECT	     	: DPI2007-61527
 *  COMPANY	     	: Automatic Control Department,
 *  				  Technical University of Catalonia
 *
 *  REVISION HISTORY:
 *			 VERSION: 0.1
 *     		  AUTHOR: Antonio Camacho Santiago
 * 				DATE: April 2010
 * 			COMMENTS: Adapted from Microchip CE127_ECAN_Crosswire demo
 *****************************************************************************/

#ifndef E_CAN1_H_
#define E_CAN1_H_

#define BAUDRATE_eCAN1 50000UL
//#define BAUDRATE_eCAN1 100000UL
//#define BAUDRATE_eCAN1 250000UL
//#define BAUDRATE_eCAN1 500000UL
//#define BAUDRATE_eCAN1 1000000UL

#define CAN_MSG_DATA	0x01 // message type
#define CAN_MSG_RTR		0x02 // data or RTR
#define CAN_FRAME_EXT	0x03 // Frame type
#define CAN_FRAME_STD	0x04 // extended or standard

/* message structure in RAM */
typedef struct{
	/* keep track of the buffer status */
	unsigned char buffer_status;
	/* RTR message or data message */
	unsigned char message_type;
	/* frame type extended or standard */
	unsigned char frame_type;
	/* buffer being used to reference the message */
	unsigned char buffer;
	/* 29 bit id max of 0x1FFF FFFF
	*  11 bit id max of 0x7FF */
	unsigned long id;
	/* message data */
	unsigned char data[8];
	/* received message data length */
	unsigned char data_length;
}mID;


/* CAN Baud Rate Configuration 		*/
#define FCAN  	40000000L
#ifdef BAUDRATE_eCAN1
	#define BITRATE1 BAUDRATE_eCAN1
#else
	#define BITRATE1 50000L//100000//50000// 1000000
	#warning By default: CAN 1 baudrate set to 50kbps
#endif
#define NTQ 	20L		// 20 Time Quanta in a Bit Time

#define BRP_VAL		(((FCAN/BITRATE1)/(2*NTQ))-1)

/* CAN Message Buffer Configuration */
#define  ECAN1_MSG_BUF_LENGTH 	8//4//2//4
typedef unsigned int ECAN1MSGBUF [ECAN1_MSG_BUF_LENGTH][8];
extern ECAN1MSGBUF  ecan1msgBuf __attribute__((space(dma)));

extern unsigned long ID_PLANT;

void dma2init(void);
void dma3init(void);
void ecan1ClkInit(void);
void ecan1Init(void);
void ecan1WriteRxAcptFilter(int n, long identifier, unsigned int exide, unsigned int bufPnt,unsigned int maskSel);
void ecan1WriteRxAcptMask(int m, long identifier, unsigned int mide, unsigned int exide);
void ecan1WriteTxMsgBufId(unsigned int buf, long txIdentifier, unsigned int ide, unsigned int remoteTransmit);
void ecan1WriteTxMsgBufData(unsigned int buf, unsigned int dataLength, unsigned int data1, unsigned int data2, unsigned int data3, unsigned int data4);

void rxECAN1(mID *message);
void clearIntrflags(void);
void eCAN1_config(void);
void ecan1SendMessage(mID *M2S);


#endif
