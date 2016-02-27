    //=============================================================================//
	//      Controlling 2 Motors using MSP430 Robot shield Ver 1.0                 //
	//=============================================================================//
	//                                                                             //
	//       MSP430G2553            L293D                                          //
	//      +------------+       +------------+       RIGHT MOTOR                  //
    //      |            |       |            |        +-------+                   //
	//      |       P2.1 |-------|1A        1Y|--------|   0   |                   //
	//      |       P2.2 |-------|2A        2Y|--------|       |                   //                     
    //      |       P2.0 |-------|1&1_EN      |        +-------+                   //                               
    //      |            |       |            |                                    //
	//      |       P2.3 |-------|3A          |        +-------+                   //
	//      |       P2.4 |-------|4A        3Y|--------|   0   |                   //
	//      |       P2.6 |-------|3&4_EN    4Y|--------|       |                   // 
	//      |            |       |            |        +-------+                   //
	//      |            |       |            |        LEFT MOTOR                  //
	//      +------------+       +------------+     (DC Brushed Motor)             //
	//        Launchpad           Robot Sheild                                     //
	//                                                                             //
	//=============================================================================//
    // Program rotates the 2 motors  in clockwise direction for sometime then      //
    // stops and reverses the direction,runs for sometime and stops.               //
    //=============================================================================//
	//    www.xanthium.in                                                          //
    //    (c) Rahul.S 2015                                                         //
    //=============================================================================//
	// Compiled on IAR Embedded Workbench for MSP430 version 5.30.1                //
    // 28-April-2015                                                               //
    // Rahul.S                                                                     //
    //=============================================================================//
	
	#include "msp430g2553.h"
	void main()
	{
		WDTCTL =  WDTPW + WDTHOLD;    // Stop the Watchdog
		
		void Delay(int j);            // Delay subroutine
		
		P2SEL  &= ~BIT6;              // Clear P2.6 in P2SEL  (by default Xin)
		P2SEL2 &= ~BIT6;              // Clear P2.6 in P2SEL2
		//Right Motor Control pins
		P2DIR |=  BIT0 + BIT1 + BIT2; // P2.0,P2.1,P2.2 all output
		P2OUT &= ~BIT0 + BIT1 + BIT2; // Clear P2.0,P2.1,P2.2  
		//Left Motor Control pins
		P2DIR |=  BIT3 + BIT4 + BIT6; // P2.3,P2.4,P2.6 all output
		P2OUT &= ~BIT3 + BIT4 + BIT6; // Clear P2.3,P2.4,P2.6 
		
		// Rotate the Right Motor clockwise
  		P2OUT |=  BIT1;              // P2.1 = 1,
		P2OUT &= ~BIT2;              // P2.2 = 0
		P2OUT |=  BIT0;              // P2.0 = 1 ,1&2_EN = 1,Motor is started
		
		// Rotate the Left Motor clockwise
  		P2OUT |=  BIT3;             // P2.3 = 1,
		P2OUT &= ~BIT4;             // P2.4 = 0,
		P2OUT |=  BIT6;             // P2.6 = 1 ,3&4_EN = 1,Motor is started
        
        Delay(10);      // Rotate motor for sometime
		
		//Stop both motors
		P2OUT &= ~BIT0;              // P2.0 = 0,1&2_EN = 0,Motor is stopped
		P2OUT &= ~BIT6;              // P2.6 = 0,3&4_EN = 0,Motor is stopped
		Delay(10);
		 
		// Rotate the Right Motor Counter clockwise
  		P2OUT |=  BIT2;            // P2.2 = 1,
		P2OUT &= ~BIT1;            // P2.1 = 0
		P2OUT |=  BIT0;            // P2.0 = 1 ,1&2_EN = 1,Motor is started
		
		// Rotate the Left Motor Counter clockwise
  		P2OUT |=  BIT4;           // P2.4 = 1,
		P2OUT &= ~BIT3;           // P2.3 = 0,
		P2OUT |=  BIT6;           // P2.6 = 1 ,3&4_EN = 1,Motor is started
		Delay(10);
		
		//Stop both motors
		P2OUT &= ~BIT0;          // P2.0 = 0,1&2_EN = 0,Motor is stopped
		P2OUT &= ~BIT6;          // P2.6 = 0,3&4_EN = 0,Motor is stopped
	}//End of Main

	void Delay(int j)// Delay subroutine
	{
		int i;
		for(j=0;j<10;j++)
			for(i=0;i<0xFFFF;i++);
	}
	  