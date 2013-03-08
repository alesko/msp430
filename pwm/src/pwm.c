//************************************************************************
// MSP430G2231 Demo - Timer_A, PWM TA1, Up Mode, SMCLK 
//
// Description: This program generates one PWM output on P1.2 using
// Timer_A configured for up mode. The value in CCR0, 1000-1, defines the
// PWM period and the value in CCR1 the PWM duty cycles. Using SMCLK,
// the timer frequenciy is about 1.1kHz with a 25% duty cycle on P1.2.
// Normal operating mode is LPM0.
// MCLK = SMCLK = default DCO (about 1.1MHz).
//
//        MSP430G2231 
//        ----------------- 
//   /|\ |                 | 
//    |  |                 | 
//     --|RST      P1.2/TA1|--> CCR1 - 75% PWM
//       |                 |
//       |                 |
//
// M.Buccini / L. Westlund
// Texas Instruments, Inc
// October 2005
// Built with CCE Version: 3.2.0 and IAR Embedded Workbench Version: 3.40A
//
// Modified by NJC for MSP430LaunchPad.com - July 2010
//
//************************************************************************

//#include <msp430g2553.h> 
#include <msp430g2418.h>

//#include "msp430G2231.h"
/* 
   void main(void)
   {
   WDTCTL = WDTPW + WDTHOLD;  // Stop WDT
 
   P1DIR |= BIT2;             // P1.2 to output
   P1SEL |= BIT2;             // P1.2 to TA0.1
 
   CCR0 = 1000-1;             // PWM Period
   CCTL1 = OUTMOD_7;          // CCR1 reset/set
   CCR1 = 900; //250;                // CCR1 PWM duty cycle
   TACTL = TASSEL_2 + MC_1;   // SMCLK, up mode
 
   _BIS_SR(LPM0_bits);        // Enter LPM0
   }
*/

 
// Connect the servo SIGNAL wire to P1.2 through a 1K resistor.
 
#define MCU_CLOCK			1000000
#define PWM_FREQUENCY		50		// In Hertz, ideally 50Hz.
 
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
     /*
     TBCCTL1	= OUTMOD_7;            // TACCR1 reset/set
     TBCTL	= TBSSEL_2 + MC_1;     // SMCLK, upmode
     TBCCR0	= PWM_Period-1;        // PWM Period
     TBCCR1	= PWM_Duty;            // TACCR1 PWM Duty Cycle
     P1DIR	|= BIT2;               // P1.2 = output
     P1SEL	|= BIT2;               // P1.2 = TA1 output
     */
}
 

void main (void)
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
}
 

