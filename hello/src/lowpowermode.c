/** 
 * \file lowpowermode.c
 * \brief  Testing the interupt to contol powerconsuption,
 * \author Alex Skoglund
 * \author Alexander Skoglund
 * \version 0.02
 * \copyright GNU Public License.
 * 
 * Testing the low power mode on the msp430 using an interupt.
 * Another meaningless line...
 */

#include <msp430.h>
#include <stdio.h>
#include <signal.h>

void configureClocks();
volatile unsigned int i;  /*!< volatile to prevent optimization */

/** \brief Main loop, stop watchdog, set P1 and enter low power mode
 * 
 * The setup in main does the following 
 * Stop watchdog timer
 * Configure LED on P1.0 
 * Configure Switch on P1.2
 * Enter Low Power Mode 4 (LPM4) w/interrupt, i.e., go to sleep
 * 
 * \todo{ A useful to do list can go here!}
 * 
 */
void main(void)
{
  // Stop watchdog timer
  WDTCTL = WDTPW + WDTHOLD;
  configureClocks();

  // Configure LED on P1.0 
  P1DIR = BIT0;     // P1.0 output
  P1OUT &=  ~BIT0;  // P1.0 output LOW, LED Off
  // Configure Switch on P1.2 //
  P1REN |= BIT2;    // P1.2 Enable Pullup/Pulldown	
  P1OUT  = BIT2;    // P1.2 pullup			
  P1IE  |= BIT2;    // P1.2 interrupt enabled	
  P1IES |= BIT2;    // P1.2 Hi/lo falling edge	
  P1IFG &= ~BIT2;   // P1.2 IFG cleared just in case

  // Enter Low Power Mode 4 (LPM4) w/interrupt
  __bis_SR_register(LPM4_bits + GIE);
}

//! Configure the clocks
/*!
 * Configures the system clocks DCO to 8MHz
 * and the LFXT1 to the VLO
 */
void configureClocks()
{
  // Set system DCO to 8MHz
  BCSCTL1 = CALBC1_8MHZ;
  DCOCTL = CALDCO_8MHZ;
  // Set LFXT1 to the VLO
  BCSCTL3 |= LFXT1S_2;
}

//! Interrup Service Routing on port 1
/*!
 *  Toggle LED at P1.0, and clear P1.2 IFG 
 * 
 */

// Port 1 interrupt service routine
//#pragma vector=PORT1_VECTOR
//__interrupt void Port_1(void)
interrupt (PORT1_VECTOR) Port_1(void)
{
  P1OUT ^= BIT0;   // Toggle LED at P1.0
  P1IFG &= ~BIT2;  // P1.2 IFG cleared
}
