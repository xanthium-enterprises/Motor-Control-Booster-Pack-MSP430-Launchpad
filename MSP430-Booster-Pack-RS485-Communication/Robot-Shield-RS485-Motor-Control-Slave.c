//============================================================================================================//
// The Program acts as a slave to the software running on the PC.The Program configures the MSP430 UART to-   //
// wait for data coming from the PC.It puts the MAX485 chip on the RobotShield board in receive mode-         //
// and switches on /off the motors connected to the L293D chip.                                               //
// RS485 communication happens at 9600bps (8N1)                                                               //
//============================================================================================================//
// www.xanthium.in                                                                                            //
// (c) Rahul.S 2015                                                                                           //
//============================================================================================================//
//                                                                                                            //
//  |------------|                                                  MAX485             MSP430G2553            //
//  | RS485 Motor|                                               +------------+       +--------------+        //
//  | control.exe|             USB2SERIAL V2.0                   |          RO|------>|P1.1/RXD      |        //
//  |------------|            |--------------|                   |          DI|<------|P1.2/TXD      |        //
//   \ [][][][][] \===========| USB -> RS485 |~~~~~~~~~~~~~~~~~~~|A,B         |       |              |        //
//    \------------\   USB    |--------------|      Twisted      |          DE|-------|P1.7          |        //
//        Laptop                                     Pair        |	       ~RE|-------|P2.5          |        //
//                                                               +------------+       |              |        //
//                              			                        L293D             |              |        //
//                                                               +------------+       |              |        //
//                                       |----RIGHT MOTOR--------|1Y        1A|-------|P2.1          |        //
//                                       |-----------------------|2Y        2A|-------|P2.2          |        //
//                                                               |      1&2_EN|-------|P2.0(EN)      |        //
//                                                               |            |       |              |        //
//                                       |-----LEFT MOTOR--------|3Y        3A|-------|P2.3          |        //
//                                       |-----------------------|4Y        4A|-------|P2.4          |        //
//                                                               |      3&4_EN|-------|P2.6(EN)      |        //
//                                                               +------------+       |              |        //
//                                                                                    +--------------+        //
//                                                           [---Robot shield---]   [-----Launchpad-----]     //
//    [Transmitter] ------------------------------------>   [-----------------Receiver-------------------]    //
//                                                                                                            //
//============================================================================================================//
// Compiled on IAR Embedded Workbench for MSP430 version 5.30.1                                               //
// License       :  BSD (2 clause)                                                                            // 
// Programmer    :	Rahul.S                                                                                   //
// 20-May-2015                                                                                                //                                                                                                  //
//============================================================================================================//


	#include "msp430g2553.h"
	void main(void)
	{
		WDTCTL = WDTPW + WDTHOLD; // Stop the Watch dog
     
		//------------------- Configure the Clocks -------------------//
        
		if (CALBC1_1MHZ==0xFF)   // If calibration constant erased
		{	
			while(1);          // do not load, trap CPU!!
		} 
		DCOCTL  = 0;             // Select lowest DCOx and MODx settings
		BCSCTL1 = CALBC1_1MHZ;   // Set range
		DCOCTL  = CALDCO_1MHZ;   // Set DCO step + modulation 
       
		//------------------- Configure the Ports  -------------------//
      
		// Setting the UART function for P1.1 & P1.2
		P1SEL  |=  BIT1 + BIT2;  		// P1.1 UCA0RXD input
		P1SEL2 |=  BIT1 + BIT2;  		// P1.2 UCA0TXD output
        
		// Setting the ports for MAX485 control
		 P1DIR  |=  BIT7;                // P1.7-> DE Output
         P2DIR  |=  BIT5;                // P2.5 -> ~RE Output
		 
		 P1OUT  &= ~BIT7;                // P1.7 = 0 
		 P2OUT  &= ~BIT5;                // P2.5 = 0, Receive Mode
		 
		// Setting the Ports for L293D control
		P2DIR   = 0xFF; // Port 2 all pins output
		
		// Port Configuration for P2.6
		P2SEL  &= ~BIT6;                // Clear P2.6 in P2SEL  (by default Xin)
		P2SEL2 &= ~BIT6;                // Clear P2.6 in P2SEL2
		
		//Setting the direction of Right Motor (MOTOR_R)
		P2OUT |=  BIT1;  // P2.1 = 1A = 1
		P2OUT &= ~BIT2;  // P2.2 = 2A = 0
		
		//Setting the direction of Left Motor (MOTOR_L)
		P2OUT |=  BIT3;  // P2.3 = 3A = 1
		P2OUT &= ~BIT4;  // P2.4 = 4A = 0
		
		P2OUT &= ~BIT0 + BIT6; // Both motors Off,P2.0 = P2.6 = 0,
		
		//-------------- Configure USCI_A0 in UART mode --------------//  
      
		UCA0CTL1 |=  UCSSEL_2 + UCSWRST;  // USCI Clock = SMCLK,USCI_A0 disabled
		UCA0BR0   =  104;                 //  104 From datasheet table-  
		UCA0BR1   =  0;                   // -selects baudrate =9600,clk = SMCLK
		UCA0MCTL  =  UCBRS_6;             // Modulation value = 6 from datasheet
      
		UCA0CTL1 &= ~UCSWRST;             // Clear UCSWRST to enable USCI_A0
      
		//-------------- Configure USCI_A0 interrupts  --------------// 
      
		IE2 |= UCA0RXIE;                 //Enable Recieve interrupt            
      
		_BIS_SR(LPM0_bits + GIE);        // Going to LPM0
        
		}//end of main()

		//-----------------   USCI_A0 Recieve ISR    --------------------------//

		#pragma vector = USCIAB0RX_VECTOR
		__interrupt void USCI_A0_RecieveInterrupt(void)
		{
			IFG2 &= ~UCA0RXIFG; //Clear the UCA0RXIFG
			
			switch(UCA0RXBUF) // Switch to ontrol P1.0 and P1.6
				{
					case 'A': P2OUT |=  BIT0; //Switch on the Right motor
					          P2OUT &= ~BIT6;
							  break;
							  
					case 'Q': P2OUT &= ~BIT0; //Switch off the Right motor
					          P2OUT &= ~BIT6;
							  break;
							  
				    case 'D': P2OUT |=  BIT6;// Switch on the Left Motor
							  P2OUT &= ~BIT0;
				              break;
							  
				case 'E':     P2OUT &= ~BIT6;// Switch off the Left Motor
							  P2OUT &= ~BIT0;
					          break;
			  			  
				}//end of switch
		}//end of ISR