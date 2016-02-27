	//=============================================================================//
	//  Controlling 2 Motors using MSP430 Robot shield Ver 1.0 With soft PWM       //
	//=============================================================================//
	//                                                                             //
	//       MSP430G2553            L293D                                          //
	// +-----------------+       +------------+       RIGHT MOTOR                  //
    // |                 |       |            |        +-------+                   //
	// |            P2.1 |-------|1A        1Y|--------|   0   |                   //
	// |            P2.2 |-------|2A        2Y|--------|       |                   //                     
    // |    +-------P2.0 |-------|1&1_EN      |        +-------+                   //                               
    // |[TIMER A]        |       |            |                                    //
	// |    +-------P2.3 |-------|3A          |        +-------+                   //
	// |            P2.4 |-------|4A        3Y|--------|   0   |                   //
	// |            P2.6 |-------|3&4_EN    4Y|--------|       |                   // 
	// |                 |       |            |        +-------+                   //
	// |                 |       |            |        LEFT MOTOR                  //
	// +-----------------+       +------------+     (DC Brushed Motor)             //
	//      Launchpad             Robot Shield                                     //
	//                                                                             //
	//=============================================================================// 
	// Program starts the timer in continous mode,Both motors are configured first //
    // and enabled in the CCR0 interrupt and disabled in Timer Over flow interrupt.//      
    // Decreasing the value in CCR1 increases the speed of the motors.             //
    //=============================================================================//
    // www.xanthium.in                                                             //
    // (c) Rahul.S 2015                                                            //
    //=============================================================================//
	// Compiled on IAR Embedded Workbench for MSP430 version 5.30.1                //
    // 28-April-2015                                                               //
    // Rahul.S                                                                     //
    //=============================================================================//
	
	#include "msp430g2553.h"
	void main(void)
	{
		WDTCTL = WDTPW + WDTHOLD; //Stop the WDT
		
		//------------------- Configure the Clocks -------------------//
		// Set DCO Clock = 8MHz                                       //
		if (CALBC1_8MHZ==0xFF)   // If calibration constant erased
		{	
			while(1);            // do not load, trap CPU!!
		} 
		DCOCTL  = 0;             // Select lowest DCOx and MODx settings
		BCSCTL1 = CALBC1_8MHZ;   // Set range
		DCOCTL  = CALDCO_8MHZ;   // Set DCO step + modulation 
		
		// Port Configuration
		P2SEL  &= ~BIT6;                // Clear P2.6 in P2SEL  (by default Xin)
		P2SEL2 &= ~BIT6;                // Clear P2.6 in P2SEL2
		//Right Motor Control pins
		P2DIR |=  BIT0 + BIT1 + BIT2;   // P2.0,P2.1,P2.2 all output
		P2OUT &= ~BIT0 + BIT1 + BIT2;   // Clear P2.0,P2.1,P2.2  
		//Left Motor Control pins
		P2DIR |=  BIT3 + BIT4 + BIT6;   // P2.3,P2.4,P2.6 all output
		P2OUT &= ~BIT3 + BIT4 + BIT6;   // Clear P2.3,P2.4,P2.6 
		
		// Rotate the Right Motor clockwise
  		P2OUT |=  BIT1;              // P2.1 = 1,
		P2OUT &= ~BIT2;              // P2.2 = 0
		
		// Rotate the Left Motor clockwise
  		P2OUT |=  BIT3;             // P2.3 = 1,
		P2OUT &= ~BIT4;             // P2.4 = 0,
		
		// Timer 0 Configurtion
		TACTL |= MC_0;                   // Stop Timer0_A3 
		TACTL  = TASSEL_2 + TAIE;        // TAR Clock = 1MHz,1MHz/8 = 125KHz,Enable Timer Interrupt
		CCTL1  = CCIE;                   // Enable interrupt for CCR1 
		CCR1   = 6150;                  // Load value 
		TACTL |= MC_2;                   // Start Timer0 in Continous Mode
		
		_BIS_SR(LPM0_bits + GIE);        // Go to Sleep in LPM0 with interrupts enabled   
	}

	#pragma vector = TIMER0_A1_VECTOR
	__interrupt void TimerA(void)
	{
	  switch(TAIV)
	  {
	  		case 0x002: // CCR1 interrupt
			  			P2OUT |=  BIT0;    // P2.0 = 1 ,1&2_EN = 1,Right Motor is started
						P2OUT |=  BIT6;    // P2.6 = 1 ,3&4_EN = 1,Left  Motor is started
			  		    break;
						
	  		case 0x004:  //CCR2 not used
			  		    break;
						
	  		case 0x00A: //TAR overflow interrupt
			  			P2OUT &= ~BIT0;     // P2.0 = 0 ,1&2_EN = 0,Right Motor is stoped
						P2OUT &= ~BIT6;     // P2.6 = 0 ,3&4_EN = 0,Left  Motor is stoped
			            break;
	  }
	}