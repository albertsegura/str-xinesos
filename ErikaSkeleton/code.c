/* single task RS232 Communication using DMA 8/4/14*/

#include "ee.h"
#include "cpu/pic30/inc/ee_irqstub.h"
#include "uart_dma.h"

_FOSCSEL(FNOSC_PRIPLL);	// Primary (XT, HS, EC) Oscillator with PLL
_FOSC(OSCIOFNC_ON & POSCMD_XT); // OSC2 Pin Function: OSC2 is Clock Output
								// Primary Oscillator Mode: XT Crystal
_FWDT(FWDTEN_OFF);		// Watchdog Timer Enabled/disabled by user software
_FGS(GCP_OFF);			// Disable Code Protection

/*	Variables	*/
static unsigned long sys_time=0;
static volatile unsigned int my_time=0;
static unsigned long int activeTask=0;
static unsigned char *p_activeTask = (unsigned char *)&activeTask;
const unsigned int C_Task1 = 150;
const unsigned int C_Task2 = 200;
const unsigned int C_Task3 = 150;
const unsigned int C_Task4 = 100;
static volatile unsigned char pila[4];
static volatile unsigned char toppila=0;

/* Program the Timer1 peripheral to raise interrupts */
void T1_program(void)
{
	T1CON = 0;		/* Stops the Timer1 and reset control register	*/
	TMR1  = 0;		/* Clear contents of the timer register	*/
	PR1   = 0x9c40;		/* Load the Period register with the value of 1ms	*/
	IPC0bits.T1IP = 5;	/* Set Timer1 priority to 1. Higher values set higher priorities */
	IFS0bits.T1IF = 0;	/* Clear the Timer1 interrupt status flag	*/
	IEC0bits.T1IE = 1;	/* Enable Timer1 interrupts		*/
	T1CONbits.TON = 1;	/* Start Timer1 with prescaler settings at 1:1
				   and clock source set to the internal instruction cycle */
}

/* This is an ISR Type 2 which is attached to the Timer 1 peripheral IRQ pin
 * The ISR simply calls CounterTick to implement the timing reference */
ISR2(_T1Interrupt)
{	
	IFS0bits.T1IF = 0;     	/* Clear the Timer1 interrupt status flag  */
	my_time++;		/* Instead of using the TMR8 & TMR9 
                                   we use this instruction to get time*/
	CounterTick(myCounter); /* count the interrupts, waking up expired alarms */
	CounterTick(counterTask1);
	CounterTick(counterTask2);
	CounterTick(counterTask3);
	CounterTick(counterTask4);
}

/* Configures  FLEX FULL orange led (Jumper 4 must be closed) */
void Led_config(void)
{
	LATBbits.LATB14 = 0;	/* set LED (LEDSYS/RB14) drive state low */
	TRISBbits.TRISB14 = 0;	/* set LED pin (LEDSYS/RB14) as output */
}

/* Periodic task sending fdata over RS232 via DMA */

void send_uart() {
		sys_time=my_time;
		//activeTask =  activeTask+('0'<<8)+'0';
		OutBuffer[0]=0x01;//header;
		OutBuffer[1]=(unsigned char)(sys_time>>24);//4th byte of unsigned long
		OutBuffer[2]=(unsigned char)(sys_time>>16);//3rd byte of unsigned long
		OutBuffer[3]=(unsigned char)(sys_time>>8); //2nd byte of unsigned long
		OutBuffer[4]=(unsigned char)sys_time;      //1st byte of unsigned long
		OutBuffer[5]=*p_activeTask;    //4th byte of float (32bits)
		OutBuffer[6]=*(p_activeTask+1);//3rd byte of float (32bits)
		OutBuffer[7]=*(p_activeTask+2);//2nd byte of float (32bits)
		OutBuffer[8]=*(p_activeTask+3);//1st byte of float (32bits)
		//Force sending data
		DMA4CONbits.CHEN  = 1;			// Re-enable DMA4 Channel
		DMA4REQbits.FORCE = 1;			// Manual mode: Kick-start the first transfer
		LATBbits.LATB14 ^= 1;//Orange led blink
}

TASK(task1)
{
	unsigned int aux = my_time;

	if(toppila!=0){
		activeTask=0x0100+pila[toppila-1];
		send_uart();		
	}
	pila[toppila]=0x01;
	toppila++;
	activeTask = 0x0201;
	send_uart();

	while (aux+C_Task1>my_time);

	activeTask = 0x01;
	send_uart();
	toppila--;
	if(toppila!=0){
		activeTask = 0x0200 + pila[toppila-1];
		send_uart();
	}
}

TASK(task2)
{
	unsigned int aux = my_time;

	if(toppila!=0){
		activeTask=0x0100+pila[toppila-1];
		send_uart();		
	}
	pila[toppila]=0x02;
	toppila++;
	activeTask = 0x0202;
	send_uart();

	while (aux+C_Task2>my_time);

	activeTask = 0x02;
	send_uart();
	toppila--;
	if(toppila!=0){
		activeTask = 0x0200 + pila[toppila-1];
		send_uart();
	}
}

TASK(task3)
{
	unsigned int aux = my_time;

	if(toppila!=0){
		activeTask=0x0100+pila[toppila-1];
		send_uart();		
	}
	pila[toppila]=0x03;
	toppila++;
	activeTask = 0x0203;
	send_uart();

	while (aux+C_Task3>my_time);

	activeTask = 0x03;
	send_uart();
	toppila--;
	if(toppila!=0){
		activeTask = 0x0200 + pila[toppila-1];
		send_uart();
	}
}

TASK(task4)
{
	unsigned int aux = my_time;

	if(toppila!=0){
		activeTask=0x0100+pila[toppila-1];
		send_uart();		
	}
	pila[toppila]=0x04;
	toppila++;
	activeTask = 0x0204;
	send_uart();

	while (aux+C_Task4>my_time);

	activeTask = 0x04;
	send_uart();
	toppila--;
	if(toppila!=0){
		activeTask = 0x0200 + pila[toppila-1];
		send_uart();
	}
}


/* main function, only to initialize software and hardware,
 * fire alarms, and implement background activities */
int main(void)
{
	CLKDIVbits.DOZEN   = 0;		/* Clock setup for 40MIPS */
	CLKDIVbits.PLLPRE  = 0;
	CLKDIVbits.PLLPOST = 0;
	PLLFBDbits.PLLDIV  = 78;
	while(OSCCONbits.LOCK!=1);	/* Wait for PLL to lock */
	T1_program();			/* Program Timer 1 to raise interrupts */
	Led_config();			/*Configure the orange led of the FLEX FULL */
	UART1_DMA_config();		/*Configure UART1 in DMA mode */
	/*Program cyclic alarms which will fire after an offset of 1000
	* counter ticks, and after that periodically every 500 ticks */
	//SetRelAlarm(AlarmMonitor, 1000, 50);//Data is sent to the PC every 0.5s
	/* Forever loop: background activities (if any) should go here */

	toppila = 0;
	SetRelAlarm(alarmTask1, 1000, 450);
	SetRelAlarm(alarmTask2, 1000, 600);
	SetRelAlarm(alarmTask3, 1000, 1000);
	SetRelAlarm(alarmTask4, 1000, 1800);

	//SetRelAlarm(TaskMonitor, 1000, 50);

	for (;;);

	return 0;
}

