#include "ee.h"
#include "cpu/pic30/inc/ee_irqstub.h"
#include "e_can1.h"

/* Dma Initialization for ECAN1 Transmission */
void dma2init(void)
{
	 DMACS0=0;
     DMA2CON=0x2020;
	 DMA2PAD=0x0442;	/* ECAN 1 (C1TXD) */
 	 DMA2CNT=0x0007;
	 DMA2REQ=0x0046;	/* ECAN 1 Transmit */
	 DMA2STA=  __builtin_dmaoffset(ecan1msgBuf);
	 DMA2CONbits.CHEN=1;
}


/* Dma Initialization for ECAN1 Reception */
void dma3init(void)
{
	 DMACS0=0;
     DMA3CON=0x0020;
	 DMA3PAD=0x0440;	/* ECAN 1 (C1RXD) */
 	 DMA3CNT=0x0007;
	 DMA3REQ=0x0022;	/* ECAN 1 Receive */
	 DMA3STA= __builtin_dmaoffset(ecan1msgBuf);
	 DMA3CONbits.CHEN=1;
}

void ecan1ClkInit(void)
{
	C1CTRL1bits.CANCKS = 0x1; // FCAN is selected to be FCY, FCAN = FCY = 40MHz
	/*	Bit Time = (Sync Segment + Propagation Delay + Phase Segment 1 +
	+ Phase Segment 2)=20*TQ
	Phase Segment 1 = 8TQ
	Phase Segment 2 = 6Tq
	Propagation Delay = 5Tq
	Sync Segment = 1TQ
	CiCFG1<BRP> =(FCAN /(2 �N�FBAUD))� 1 */

	C1CFG1bits.SJW = 0x3;     // Synchronization Jump Width set to 4 TQ
	C1CFG1bits.BRP = BRP_VAL; // Baud Rate Prescaler
	C1CFG2bits.SEG1PH=0x7;    // Phase Segment 1 time is 8 TQ
	C1CFG2bits.SEG2PHTS = 0x1;// Phase Segment 2 time is set to be programmable
	C1CFG2bits.SEG2PH = 0x5;  // Phase Segment 2 time is 6 TQ
	C1CFG2bits.PRSEG = 0x4;   // Propagation Segment time is 5 TQ
	C1CFG2bits.SAM = 0x1;     // Bus line is sampled three times at the sample point
}

void ecan1Init(void)
{
	C1CTRL1bits.REQOP=4;   // Request Configuration Mode
	while(C1CTRL1bits.OPMODE!=4);

	ecan1ClkInit();

	C1CTRL1bits.REQOP=0;   // Enter Normal Mode
	while(C1CTRL1bits.OPMODE!=0);

}

/*This function configures Acceptance Filter "n"
Inputs:
n-> Filter number [0-15]
identifier-> Bit ordering is given below
Filter Identifier (29-bits) : 0b000f ffff ffff ffff ffff ffff ffff ffff
								   |____________|_____________________|
									  SID10:0           EID17:0
Filter Identifier (11-bits) : 0b0000 0000 0000 0000 0000 0fff ffff ffff
														  |___________|
															  SID10:0
exide -> "0" for standard identifier
		 "1" for Extended identifier
bufPnt -> Message buffer to store filtered message [0-15]
maskSel -> Optinal Masking of identifier bits [0-3]*/

void ecan1WriteRxAcptFilter(int n, long identifier, unsigned int exide,
		unsigned int bufPnt,unsigned int maskSel)
{
	unsigned long sid10_0=0, eid15_0=0, eid17_16=0;
	unsigned int *sidRegAddr,*bufPntRegAddr,*maskSelRegAddr, *fltEnRegAddr;

	C1CTRL1bits.WIN=1;
	/* Obtain the Address of CiRXFnSID, CiBUFPNTn, CiFMSKSELn and CiFEN
	 * register for a given filter number "n" */
	sidRegAddr = (unsigned int *)(&C1RXF0SID + (n << 1));
	bufPntRegAddr = (unsigned int *)(&C1BUFPNT1 + (n >> 2));
	maskSelRegAddr = (unsigned int *)(&C1FMSKSEL1 + (n >> 3));
	fltEnRegAddr = (unsigned int *)(&C1FEN1);
	// Bit-filed manupulation to write to Filter identifier register
	if(exide==1) { 	// Filter Extended Identifier
		eid15_0 = (identifier & 0xFFFF);
		eid17_16= (identifier>>16) & 0x3;
		sid10_0 = (identifier>>18) & 0x7FF;
		*sidRegAddr=(((sid10_0)<<5) + 0x8) + eid17_16; // Write to CiRXFnSID Register
	    *(sidRegAddr+1)= eid15_0; // Write to CiRXFnEID Register
	}else{			// Filter Standard Identifier
		sid10_0 = (identifier & 0x7FF);
		*sidRegAddr=(sid10_0)<<5;// Write to CiRXFnSID Register
		*(sidRegAddr+1)=0;		 // Write to CiRXFnEID Register
	}
   *bufPntRegAddr = (*bufPntRegAddr) & (0xFFFF - (0xF << (4 *(n & 3))));// clear nibble
   *bufPntRegAddr = ((bufPnt << (4 *(n & 3))) | (*bufPntRegAddr)); // Write to C1BUFPNTn Register
   *maskSelRegAddr = (*maskSelRegAddr) & (0xFFFF - (0x3 << ((n & 7) * 2))); // clear 2 bits
   *maskSelRegAddr = ((maskSel << (2 * (n & 7))) | (*maskSelRegAddr)); // Write to C1FMSKSELn Register
   *fltEnRegAddr = ((0x1 << n) | (*fltEnRegAddr)); // Write to C1FEN1 Register
   C1CTRL1bits.WIN=0;
}

/* This function configures Acceptance Filter Mask "m"
Inputs:
m-> Mask number [0-2]
identifier-> Bit ordering is given below
Filter Mask Identifier (29-bits) : 0b000f ffff ffff ffff ffff ffff ffff ffff
								        |____________|_____________________|
									        SID10:0           EID17:0
Filter Mask Identifier (11-bits) : 0b0000 0000 0000 0000 0000 0fff ffff ffff
														       |___________|
															      SID10:0
mide ->  "0"  Match either standard or extended address message if filters match
         "1"  Match only message types that correpond to 'exide' bit in filter */

void ecan1WriteRxAcptMask(int m, long identifier, unsigned int mide,
		unsigned int exide)
{
	unsigned long sid10_0=0, eid15_0=0, eid17_16=0;
	unsigned int *maskRegAddr;

	C1CTRL1bits.WIN=1;
	// Obtain the Address of CiRXMmSID register for given Mask number "m"
	maskRegAddr = (unsigned int *)(&C1RXM0SID + (m << 1));
	// Bit-filed manupulation to write to Filter Mask register
	if(exide==1)
	{ 	// Filter Extended Identifier
		eid15_0 = (identifier & 0xFFFF);
		eid17_16= (identifier>>16) & 0x3;
		sid10_0 = (identifier>>18) & 0x7FF;
		if(mide==1)   // Write to CiRXMnSID Register
			*maskRegAddr=((sid10_0)<<5) + 0x0008 + eid17_16;
		else // Write to CiRXMnSID Register
			*maskRegAddr=((sid10_0)<<5) + eid17_16;
			// Write to CiRXMnEID Register
			*(maskRegAddr+1)= eid15_0;
	}
	else
	{			// Filter Standard Identifier
		sid10_0 = (identifier & 0x7FF);
		if(mide==1) // Write to CiRXMnSID Register
			*maskRegAddr=((sid10_0)<<5) + 0x0008;
		else // Write to CiRXMnSID Register
			*maskRegAddr=(sid10_0)<<5;
			// Write to CiRXMnEID Register
			*(maskRegAddr+1)=0;
	}
	C1CTRL1bits.WIN=0;
}

/* ECAN Transmit Message Buffer Configuration*/

void ecan1WriteTxMsgBufId(unsigned int buf, long txIdentifier,
		unsigned int ide, unsigned int remoteTransmit)
{
	unsigned long word0=0, word1=0, word2=0;
	unsigned long sid10_0=0, eid5_0=0, eid17_6=0;
	if(ide)
	{
		eid5_0  = (txIdentifier & 0x3F);
		eid17_6 = (txIdentifier>>6) & 0xFFF;
		sid10_0 = (txIdentifier>>18) & 0x7FF;
		word1 = eid17_6;
	}
	else
	{
		sid10_0 = (txIdentifier & 0x7FF);
	}
	if(remoteTransmit==1) { 	// Transmit Remote Frame
		word0 = ((sid10_0 << 2) | ide | 0x2);
		word2 = ((eid5_0 << 10)| 0x0200);}
	else {
		word0 = ((sid10_0 << 2) | ide);
		word2 = (eid5_0 << 10);
	}
    if(ide)   //Obtain the Address of Transmit Buffer in DMA RAM
             // for a given Transmit Buffer number
    	ecan1msgBuf[buf][0] = (word0 | 0x0002);
    else
    	ecan1msgBuf[buf][0] = word0;
		ecan1msgBuf[buf][1] = word1;
		ecan1msgBuf[buf][2] = word2;
}

/* ECAN Transmit Data
Inputs :    buf -> Transmit Buffer Number
			dataLength -> Length of Data in Bytes to be transmitted
			data1/data2/data3/data4 ->  Transmit Data Bytes */

void ecan1WriteTxMsgBufData(unsigned int buf, unsigned int dataLength,
	unsigned int data1, unsigned int data2, unsigned int data3,
	unsigned int data4)
{
	ecan1msgBuf[buf][2] = ((ecan1msgBuf[buf][2] & 0xFFF0) + dataLength) ;
	ecan1msgBuf[buf][3] = data1;
	ecan1msgBuf[buf][4] = data2;
	ecan1msgBuf[buf][5] = data3;
	ecan1msgBuf[buf][6] = data4;
}

/* ECAN receive message */

void rxECAN1(mID *message)
{
	unsigned int ide=0;
	unsigned int srr=0;
	unsigned long id=0;

	/*Standard Message Format:
	Word0 : 0bUUUx xxxx xxxx xxxx
			     |____________|||
 					SID10:0   SRR IDE(bit 0)
	Word1 : 0bUUUU xxxx xxxx xxxx
			   	   |____________|
						EID17:6
	Word2 : 0bxxxx xxx0 UUU0 xxxx
			  |_____||	     |__|
			  EID5:0 RTR   	  DLC
	word3-word6: data bytes
	word7: filter hit code bits

	Substitute Remote Request Bit
	SRR->	"0"	 Normal Message
			"1"  Message will request remote transmission
	Extended  Identifier Bit
	IDE-> 	"0"  Message will transmit standard identifier
	   		"1"  Message will transmit extended identifier
	Remote Transmission Request Bit
	RTR-> 	"0"  Message transmitted is a normal message
			"1"  Message transmitted is a remote message */

	ide=ecan1msgBuf[message->buffer][0] & 0x0001; // read word 0 to see the message type
	srr=ecan1msgBuf[message->buffer][0] & 0x0002;
	if(ide==0)  // check to see what type of message it is: standard id
	{
		message->id=(ecan1msgBuf[message->buffer][0] & 0x1FFC) >> 2;
		message->frame_type=CAN_FRAME_STD;
	}
	else       // mesage is extended identifier
	{
		id=ecan1msgBuf[message->buffer][0] & 0x1FFC;
		message->id=id << 16;
		id=ecan1msgBuf[message->buffer][1] & 0x0FFF;
		message->id=message->id+(id << 6);
		id=(ecan1msgBuf[message->buffer][2] & 0xFC00) >> 10;
		message->id=message->id+id;
		message->frame_type=CAN_FRAME_EXT;
	}
	if(srr==1) // check to see what type of message it is: RTR message
	{
		message->message_type=CAN_MSG_RTR;
	}
	else       // normal message
	{
		message->message_type=CAN_MSG_DATA;
		message->data[0]=(unsigned char)ecan1msgBuf[message->buffer][3];
		message->data[1]=(unsigned char)((ecan1msgBuf[message->buffer][3] & 0xFF00) >> 8);
		message->data[2]=(unsigned char)ecan1msgBuf[message->buffer][4];
		message->data[3]=(unsigned char)((ecan1msgBuf[message->buffer][4] & 0xFF00) >> 8);
		message->data[4]=(unsigned char)ecan1msgBuf[message->buffer][5];
		message->data[5]=(unsigned char)((ecan1msgBuf[message->buffer][5] & 0xFF00) >> 8);
		message->data[6]=(unsigned char)ecan1msgBuf[message->buffer][6];
		message->data[7]=(unsigned char)((ecan1msgBuf[message->buffer][6] & 0xFF00) >> 8);
		message->data_length=(unsigned char)(ecan1msgBuf[message->buffer][2] & 0x000F);
	}
}

void clearIntrflags(void)      // Clear Interrupt Flags
{
	IFS0=0;
	IFS1=0;
	IFS2=0;
	IFS3=0;
	IFS4=0;
}

ISR2(_DMA2Interrupt)             // DMA interrupt handler
{
   IFS1bits.DMA2IF = 0;          // Clear the DMA2 Interrupt Flag;
}

ISR2(_DMA3Interrupt)             // DMA interrupt handler
{
   IFS2bits.DMA3IF = 0;          // Clear the DMA3 Interrupt Flag;
}

void ecan1SendMessage(mID *M2S)
{
	ecan1WriteTxMsgBufId((*M2S).buffer,(*M2S).id,(*M2S).frame_type,(*M2S).message_type);
	ecan1WriteTxMsgBufData((*M2S).buffer,
						   (*M2S).data_length,
						   ((*M2S).data[1]<<8) + (*M2S).data[0],
						   ((*M2S).data[3]<<8) + (*M2S).data[2],
						   ((*M2S).data[5]<<8) + (*M2S).data[4],
						   ((*M2S).data[7]<<8) + (*M2S).data[6]);
	C1TR01CONbits.TXREQ0=1;
}

void eCAN1_config(void)
{
		clearIntrflags(); 	// Clear Interrupt Flags
		ecan1Init();        // ECAN1 Initialization
		dma2init();         // Configure DMA Channel 0 for ECAN1 Transmit
		dma3init();         // Configure DMA Channel 2 for ECAN1 Receive
		IEC2bits.C1IE = 1;  // Enable ECAN1 Interrupt
		C1INTEbits.TBIE = 1;
		C1INTEbits.RBIE = 1;
}







