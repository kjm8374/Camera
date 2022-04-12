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
int SAMPLES = 20;
int count=0;
uint16_t data[30];
uint16_t max_val[30];
double idx[2];
unsigned long MillisecondCounter = 0;

int max(void);
void min(void);

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
	int idx_count = 0;
	unsigned int analogIn = 0;
	int i,maximum =0;
	//double voltage = 0.0;
	double heart_rate;
	if(Timer1RunningFlag)
	{
		analogIn = ADC_In();
		data[count] = analogIn;
		if (count == 30){
			maximum = max();
			for(i=0; i <= 28; i++){
				if (data[i] == maximum){ 
					if ((data[i+1] != maximum) && (data[i+2] != maximum)) {
						idx[idx_count] = i;
						idx_count++;
						if (idx_count == 2){
							idx_count = 0;
							heart_rate = 60.0*(1/((idx[1] - idx[0])/10.0));
							sprintf(temp,"\r\nHeart Rate: %f", heart_rate);
							uart0_put(temp);
						}
					}
				}
			}
			//;

			//uart0_put("\r\nMax value: ");			

			uart0_put("\r\n");
			count = 0;
		}
		//uart0_put("Here\r\n");
//		voltage = (analogIn)*(3.3/16383);
//		sprintf(temp,"\r\nVoltage: %f",voltage);
//		uart0_put(temp);
//		sprintf(temp,"\r\nHexadecimal: %x",analogIn);
//		uart0_put(temp); 
		count++;
	}
}

int max(void){
	int maximum = 0;
	int i = 0;
	
	for(i=0; i <= 30; i++){
		if(data[i] > maximum){
			maximum = data[i];
		}
	}
	return maximum;
}

void Switch1_Interrupt_Init(void)
{
	// disable interrupts
	DisableInterrupts();
	// initialize the Switch as per previous lab
	Switch1_Init();
	
	//7-0 PxIFG RW 0h Port X interrupt flag
	//0b = No interrupt is pending.
	//1b = Interrupt is pending.
	// clear flag1 (reduce possibility of extra interrupt)	
  P1->IFG &= ~BIT1;

	//7-0 PxIE RW 0h Port X interrupt enable
	//0b = Corresponding port interrupt disabled
	//1b = Corresponding port interrupt enabled	
	// arm interrupt on  P1.1	
  P1->IE |= BIT1;  

	//7-0 PxIES RW Undefined Port X interrupt edge select
  //0b = PxIFG flag is set with a low-to-high transition.
  //1b = PxIFG flag is set with a high-to-low transition
	// now set the pin to cause falling edge interrupt event
	// P1.1 is falling edge event
  P1->IES |= BIT1; 
	
	// now set the pin to cause falling edge interrupt event
  NVIC_IPR8 = (NVIC_IPR8 & 0x00FFFFFF)|0x40000000; // priority 2
	
	// enable Port 1 - interrupt 35 in NVIC	
  NVIC_ISER1 = 0x00000008;  
	
	// enable interrupts  (// clear the I bit	)
  EnableInterrupts();              
	
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
	
	Timer32_1_Init(&Timer32_1_ISR, SystemCoreClock/10, T32DIV1);

	

	LED1_Init();
	//LED2_Init();
	Switch1_Interrupt_Init();
	ADC0_InitSWTriggerCh6();
	EnableInterrupts();
  while(1)
	{
		WaitForInterrupt();
		
  }
}

