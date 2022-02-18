/*
* Rochester Institute of Technology
* Department of Computer Engineering
* CMPE 460  Interfacing Digital Electronics
* LJBeato
* 1/14/2021
*
* Filename: main_timer_template.c
*/
#include <stdio.h>
#include <stdlib.h>

#include "msp.h"
#include "uart.h"
#include "led.h"
#include "switches.h"
#include "Timer32.h"
#include "CortexM.h"
#include "Common.h"
#include "ADC14.h"
// The sprintf function seemed to cause a hange in the interrupt service routine.
// I think if we increase the HEAP size, it will work
// change to Heap_Size       EQU     0x00000200 in startup_msp432p401r_uvision.s


BOOLEAN Timer1RunningFlag = FALSE;
BOOLEAN Timer2RunningFlag = FALSE;

unsigned long MillisecondCounter = 0;

void PORT1_IRQHandler(void)
{
	// First we check if it came from Switch1 ?
  if(P1->IFG & BIT1)  // we start a timer to toggle the LED1 1 second ON and 1 second OFF
	{
		// acknowledge P1.1 is pressed, by setting BIT1 to zero - remember P1.1 is switch 1
		// clear flag, acknowledge
    P1->IFG &= ~BIT1;  
		Timer1RunningFlag = !Timer1RunningFlag;
	}
}	
// Interrupt Service Routine for Timer32-1
void Timer32_1_ISR(void)
{
	char temp[64];
	unsigned int analogIn = 0;
	if(Timer1RunningFlag)
	{
		analogIn = ADC_In();
		sprintf(temp,"\r\nDecimal: %d",analogIn);
		uart0_put(temp);
	}
}
// Interrupt Service Routine
void Timer32_2_ISR(void)
{

}



// main
int main(void)
{
	//initializations
	uart0_init();
	uart0_put("\r\nLab5 ADC demo\r\n");

	

	LED1_Init();
	//LED2_Init();
	Switch1_Init();
	ADC0_InitSWTriggerCh6();
	EnableInterrupts();
  while(1)
	{
		WaitForInterrupt();
		
  }
}

