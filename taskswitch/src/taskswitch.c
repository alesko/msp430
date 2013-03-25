/******************************************************************************
* Copyright (c) 2013, Alexander Skoglund, ÅF Group
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*    * Redistributions of source code must retain the above copyright
*      notice, this list of conditions and the following disclaimer.
*    * Redistributions in binary form must reproduce the above copyright
*      notice, this list of conditions and the following disclaimer in the
*      documentation and/or other materials provided with the distribution.
*    * Neither the name of the ÅF Group nor the names of its contributors
*      may be used to endorse or promote products derived from this software
*      without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
* THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
* PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE MSS PROJECT OR ITS
* CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
* EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
* PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
* OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
* WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
* OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
* ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
******************************************************************************/

/**************************************************************************//**
* 
* @file     taskswitch.c
* 
* @brief    main source file for taskswitch
*
* @version  0.0.1
*
* @author   Alexander Skoglund
* 
* @remark   Built with msp430-gcc for the MSP430LaunchPad from Texas
*           Instruments, Inc
* 
******************************************************************************/

//*****************************************************************************
// Include section
//*****************************************************************************

#include <msp430.h> 
//#include <msp430g2553.h>
#define interrupt(x) void __attribute__((interrupt (x)))
//#include <time.h>
//#include <msp430g2231.h>
//#include <signal.h>

void func_0();
void func_1();
void func_2();

#define NUM_TASKS 3
#define TASK_TABLE            \
        ENTRY(TASK_0, func_0) \
        ENTRY(TASK_1, func_1) \
        ENTRY(TASK_2, func_2) 

/* declare an enumeration of task codes */ 
enum{
#define ENTRY(a,b) a,
    TASK_TABLE
#undef ENTRY
    NUM_STATES
};

struct Task;
typedef void (*TaskCallback)(struct Task*);

/* declare a table of function pointers */ 
TaskCallback jumptable[NUM_STATES] = {
#define ENTRY(a,b) b,
    TASK_TABLE
#undef ENTRY
};


typedef struct Task{
//	tTime runNextAt;
//	tTime timeBetweenRuns;
	TaskCallback callback;	
	int enabled;
	struct Task* pNextTask;
} tTask;

#define LED0 BIT0
unsigned char count = 0;

void func_0()
{
}

void func_1()
{
}

void func_2()
{
}

void main(void)
{

	WDTCTL = WDTPW + WDTHOLD;		// Stop WDT
	P1DIR |= 0xFF;			//All outputs
	P1OUT = 0;
	BCSCTL3 = LFXT1S1;			//Select VLOCLK for ACLCK (i.e. 12khz intosc)
	P1OUT |= LED0;
	TACCTL0 = OUTMOD_2 + CCIE;		// TACCR0 interrupt enabled
	TACCR0 = 32768;	//Compare to Timer_A register (approx. 25 sec for VLOCLK & 1/8 divider)
	TACTL = TASSEL_1 + ID1 + ID0 + MC_1;    // ACLCK, 1/8 DIVIDER, upmode to TCCR0 value
	
	for(;;){
		__bis_SR_register(LPM3_bits + GIE);    //Enter LPM3 w/interrupt
		__no_operation();		// For debugger, executes next instruction just like a PIC
	}
	
}


//#pragma vector=TIMERA0_VECTOR		// Timer A0 interrupt service routine
//__interrupt void Timer_A (void)
interrupt(TIMER0_A0_VECTOR) Timer_A (void)
{
	count += 1;
	TACCR0 = 32768;			// reset TACCR0 register
	if (count==1){			//24 * 25sec is approx. 10 minutes
		P1OUT ^= LED0;		//Toggle LED
		count = 0;
	}
}


// Initialse the task list
//static tTask* pTaskList = NULL;

/*void AddTask(tTask TaskFunc)
{
	tTask* pNewTask;
	tTask* pActiveTask;
	// Create a new task record 
	pNewTask = malloc(sizeof(tTask));	
	// Assign the task 
	tTask->callback = TaskFunc;
	tTask->enable = 1;
	tTask->pNextTask = NULL;
	
	if(pTaskList == NULL){
		// Task list is empty
		pTaskList = pNewTask;
	}
	else{
		// Move to the last task in the list 
		pActiveTask = pTaskList;
		while(pActiveTask->pNextTask != NULL)
			{
				pActiveTask = pActiveTask->pNextTask;
			}
		
		// Add the new task to the end of the list
		pActiveTask->pNext = pNewTask;	
	}
}
*/



 
// Connect the servo SIGNAL wire to P1.2 through a 1K resistor.
 
#define MCU_CLOCK			1000000
#define PWM_FREQUENCY		46		// In Hertz, ideally 50Hz.
 
#define SERVO_STEPS			180		// Maximum amount of steps in degrees (180 is common)
#define SERVO_MIN			650		// The minimum duty cycle for this servo
#define SERVO_MAX			2700	// The maximum duty cycle
 
unsigned int PWM_Period		= (MCU_CLOCK / PWM_FREQUENCY);	// PWM Period
unsigned int PWM_Duty		= 0;							// %


void setup_pwm(void)
{

     // Setup the PWM, etc.
     WDTCTL	= WDTPW + WDTHOLD;     // Kill watchdog timer
     TACCTL1	= OUTMOD_7;            // TACCR1 reset/set
     TACTL	= TASSEL_2 + MC_1;     // SMCLK, upmode
     TACCR0	= PWM_Period-1;        // PWM Period
     TACCR1	= PWM_Duty;            // TACCR1 PWM Duty Cycle
     P1DIR	|= BIT2;               // P1.2 = output
     P1SEL	|= BIT2;               // P1.2 = TA1 output
	
}
 

int main1 (void)
{	
     unsigned int servo_stepval, servo_stepnow;
     unsigned int servo_lut[ SERVO_STEPS+1 ];
     unsigned int i;
     
     // Calculate the step value and define the current step, defaults to minimum.
     servo_stepval 	= ( (SERVO_MAX - SERVO_MIN) / SERVO_STEPS );
     servo_stepnow	= SERVO_MIN;
 
     // Fill up the LUT
     for (i = 0; i < SERVO_STEPS; i++) {
	  servo_stepnow += servo_stepval;
	  servo_lut[i] = servo_stepnow;
     }
     
     setup_pwm();
     
     // Main loop
     while (1){
 
	  // Go to 0
	  TACCR1 = servo_lut[0];
	  __delay_cycles(1000000);
 
	  // Go to 45
	  TACCR1 = servo_lut[45];
	  __delay_cycles(1000000);
 
	  // Go to 90
	  TACCR1 = servo_lut[90];
	  __delay_cycles(1000000);
 
	  // Go to 180
	  TACCR1 = servo_lut[179];
	  __delay_cycles(1000000);
 
	  // Move forward toward the maximum step value
	  for (i = 0; i < SERVO_STEPS; i++) { 			
	       TACCR1 = servo_lut[i]; 			
	       __delay_cycles(20000); 		
	  } 		 		
	  
          // Move backward toward the minimum step value 		
	  for (i = SERVO_STEPS; i > 0; i--) {
	       TACCR1 = servo_lut[i];
	       __delay_cycles(20000);
	  }
     }
     return 0;
}
 

