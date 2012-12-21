#include <msp430.h>
#include <stdio.h>
 
/*  volatile to prevent optimization */
volatile unsigned int i; 

void main(void)
{ 

  // Stop watchdog timer
  WDTCTL = WDTPW + WDTHOLD;

  // Set P1.0 to output direction
  P1DIR |= 0x01; 

  
  
  for (;;){
      // Toggle P1.0 using exclusive-OR
      P1OUT ^= 0x01;

      // Delay
      i = 50000;
      do (i--);
      while (i != 0);
  }
}
