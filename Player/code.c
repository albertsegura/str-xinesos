/******************************************************************************
 *  Node A
 *****************************************************************************/

#include "ee.h"
#include "cpu/pic30/inc/ee_irqstub.h"
#include "setup.h"
#include "e_can1.h"
#include <stdio.h>
#include <stdlib.h>
#include <can_encoder.h>

_FOSCSEL(FNOSC_PRIPLL);	// Primary (XT, HS, EC) Oscillator with PLL
_FOSC(OSCIOFNC_ON & POSCMD_XT); // OSC2 Pin Function: OSC2 is Clock Output
								// Primary Oscillator Mode: XT Crystal
_FWDT(FWDTEN_OFF);		// Watchdog Timer Enabled/disabled by user software
_FGS(GCP_OFF);			// Disable Code Protection


#define C_MSG_ERROR 	0
#define C_MSG_START 	1
#define C_MSG_FB 		2
#define C_MSG_SB 		3
#define C_MSG_RESULT 	4

unsigned long int personal_key=0xdeadbeef;
unsigned int game_stage = 0;


// Define ECAN Message Buffers
ECAN1MSGBUF ecan1msgBuf __attribute__((space(dma),aligned(ECAN1_MSG_BUF_LENGTH*16)));
// CAN Messages in RAM
mID tx_ecan1message; //TX Transmission message
mID rx_ecanPmessage; //RX Players Reception message
mID rx_ecanCmessage; //RX Croupier Reception message

static float counter[2]={0,0};
static float *p_result1 = (float *)&rx_ecanPmessage.data[0];
static unsigned char *p_data= NULL;

void CAN_Mask_Filter_Config(void) {
	/*	Mask Configuration
		ecan1WriteRxAcptMask(int m, long identifierMask, unsigned int mide,
		unsigned int exide)
		m = 0 to 2 -> Mask Number
		identifier -> SID <10:0> : EID <17:0>
		mide = 0 -> Match either standard or extended address message		if filters match
		mide = 1 -> Match only message types that correpond to 'exide' bit in filter
		exide = 0 -> Match messages with standard identifier addresses
		exide = 1 -> Match messages with extended identifier addresses
	*/
	ecan1WriteRxAcptMask(0x0,0x1FFFFFF8, 1,0x1);
	/*	Filter Configuration
		ecan1WriteRxAcptFilter(int n, long identifier, unsigned int exide,
		unsigned int bufPnt,unsigned int maskSel)
		n = 0 to 15 -> Filter number
		identifier -> SID <10:0> : EID <17:0>
		exide = 0 -> Match messages with standard identifier addresses
		exide = 1 -> Match messages with extended identifier addresses
		bufPnt = 0 to 14  -> RX Buffer 0 to 14
		bufPnt = 15 -> RX FIFO Buffer
		maskSel = 0	->	Acceptance Mask 0 register contains mask
		maskSel = 1	->	Acceptance Mask 1 register contains mask
		maskSel = 2	->	Acceptance Mask 2 register contains mask
		maskSel = 3	->	No Mask Selection
	*/
	ecan1WriteRxAcptFilter(0x0,0x0,0x1,0x2,0x0);		//id=Croupier messages to buffer 2
	ecan1WriteRxAcptFilter(0x1,0x1FFFFFF8,0x1,0x3,0x0);	//id=Players  messages to buffer 3
}

/* CAN bus 1 Interrupt , ISR2 type */
ISR2(_C1Interrupt) {
    IFS2bits.C1IF = 0; // clear interrupt flag
	/* Transmission interrupt (nothing to be done but clear flag) */
    if(C1INTFbits.TBIF) C1INTFbits.TBIF = 0;
	/*Reception interrupt, different code for different filtered id's if required */
    if(C1INTFbits.RBIF) {
		if(C1RXFUL1bits.RXFUL2==1) //Filter 2
	    {
			/*Tells rxECAN1 the buffer to pass from DMA to RAM */
	    	rx_ecanCmessage.buffer=2;
	    	C1RXFUL1bits.RXFUL2=0;
		    rxECAN1(&rx_ecanCmessage);
		    C1INTFbits.RBIF = 0;
		    ActivateTask(ReceiverCroupierTask);
	    }
		if(C1RXFUL1bits.RXFUL3==1)//Filter 3
	    {
			/*Tells rxECAN1 the buffer to pass from DMA to RAM */
	    	rx_ecanPmessage.buffer=3;
	    	C1RXFUL1bits.RXFUL3=0;
		    rxECAN1(&rx_ecanPmessage);
			C1INTFbits.RBIF = 0;
			ActivateTask(ReceiverPlayerTask);
	    }
   }
}

void Send_message_to_node_B(float *data) {
	p_data=(unsigned char *)data;
	C1CTRL1bits.ABAT = 1;
	while(C1TR01CONbits.TXREQ0){};
	tx_ecan1message.buffer=0;//Buffer number
	tx_ecan1message.frame_type=1;//0->Std Id, 1->Ext Id
	tx_ecan1message.id=1;//Identifier;
	tx_ecan1message.message_type=0;//0->Normal, 1->Remote Transmit
	tx_ecan1message.data_length=8;//Length of data (0 to 8 bytes)
	tx_ecan1message.data[0]= *p_data;
	tx_ecan1message.data[1]=*(p_data+1);
	tx_ecan1message.data[2]=*(p_data+2);
	tx_ecan1message.data[3]=*(p_data+3);
	tx_ecan1message.data[4]=*(p_data+4);
	tx_ecan1message.data[5]=*(p_data+5);
	tx_ecan1message.data[6]=*(p_data+6);
	tx_ecan1message.data[7]=*(p_data+7);
	ecan1SendMessage(&tx_ecan1message);
	while(C1TR01CONbits.TXREQ0){};
}

TASK(SenderTask) {
	LATBbits.LATB14=!LATBbits.LATB14;
	counter[0]=counter[0]+1;
	if (counter[0]==100) counter[0]=0;
	Send_message_to_node_B(&counter[0]);
}

TASK(ReceiverCroupierTask) {
	CanMessage * msg;
	char id = (char)rx_ecanCmessage.id;

	if (id == C_MSG_START) {
		unsigned char aposta = 0; // De 0-3
		if (1 == init_encoding(personal_key, (CanMessage *)&rx_ecanCmessage.data[0])) {
			msg = encode_coins(aposta);
			// send encode coins message
		}
	}
	else if (id == C_MSG_FB) {
		unsigned int guess = 0;
		if (1 == check_croupier_signature((CanMessage *)&rx_ecanCmessage.data[0], 2)) {
			msg = sign_initial_guess(guess);
			// cast to tx_ecanmessage
			// send
		}
	}
	else if (id == C_MSG_SB) {
		unsigned int guess = 0;
		if (1 == check_croupier_signature((CanMessage *)&rx_ecanCmessage.data[0], 3)) {
			msg = sign_final_guess(guess);
			// cast to tx_ecanmessage
			// send
		}
	}
	else if (id == C_MSG_RESULT) {
		unsigned int result  = 0;
		if (1 == check_croupier_signature((CanMessage *)&rx_ecanCmessage.data[0], 3)) {
			// missatge rebut conté: valor amb suma de les apostes i un bitmask amb els players guanyadors
			// suma	16 bits del bit 16 al 32.
			// guanyadors: 8 bits de menys pes.
		}
	}
	else if (id == C_MSG_ERROR) ; // LOL
}


TASK(ReceiverPlayerTask) {
	char id = (char)rx_ecanPmessage.id;
    //printf("%f multiplied by 2 gives %f \n\r",(*p_result1),(*p_result2));
}

int main(void)
{
	char str [80];
	int i;

	Sys_init();//Initialize clock, devices and periphericals
	CAN_Mask_Filter_Config();
	SetRelAlarm(AlarmSender,1000,500);//Sender activates every 0.5s
	printf("Player started!\n");
	EE_UART1_Send('a');
	printf ("Enter your family name: ");
	scanf ("%79s",str);
	printf ("Enter your age: ");
	scanf ("%d",&i);
	printf ("Mr. %s , %d years old.\n",str,i);

	for (;;);
	return 0;
}








