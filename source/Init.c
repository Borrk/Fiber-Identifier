/*****************************************************************************
* 
*   Copyright (C) 2011
*   All rights reserved.
*
*   Module Name:    Init.c
*   Author:
*   Description:    Initialize Module 
*   
*   $Workfile: Init.c $
*   $Author:  $
*   Date: 9/11/2011
*
******************************************************************************/


#ifndef INIT_C
#define INIT_C

//#include <stdio.h>
#include <string.h>
#include <absacc.h>
#include <intrins.h>

#include "FIBER_IDENTIFIER.h"
#include "Gdata.h"
#include "Speaker.h"


void fillColdStartBuf(void)
{
/*    tUINT8 data i;

    for ( i = 0 ; i < COLD_BUFFER_SIZE ; i++ )
    {
        ColdStartBuf[i] = i;
    }
*/}

static eBOOL isColdStart(void)
{
/*    tUINT8 data i;

    for ( i = 0 ; i < COLD_BUFFER_SIZE ; i++ )
    {
        if ( ColdStartBuf[i] != i )
        {
            return( TRUE );
        }
    } 
*/    return( FALSE );
}

eStartUpType getStartUpType(void)
{
 /*   tUINT8 data rstSource;
    eStartUpType data result;

    // 1. I assume it's a hardware reset.
    if ( isColdStart() == TRUE )
    {
        return( HARD_START );
    }
    else 
    {
        // We can't check RSTSRC directly, because jump 0x0000 won't change the value of it. so it keep the
        // initial PowerOn/RST/WatchDog Start information.
        if ( SoftStartFlag == TRUE )
        {
            if ( SoftStartPattern == MMI_SOFTSTART_PATTERN )
            {
                return( SOFT_START );
            }
        }
        // No Software Start Mark, it's a start caused by PowerOn, RST or Watch Dog, let's check the RSTSRC.
        rstSource = RSTSRC;
        if ( rstSource & 0x02 ) // POR 
        {
            result = HARD_START;
        }
        else if ( rstSource & 0x01 ) // Reset Pin: It's not HardStart
        {
            result = RESET_START;
        }
        else if ( rstSource & 0x08 ) // Watch Dog starts the reset. 
        {
            result = WATCHDOG_START;
        }
        else
        {
            result = HARD_START; // For safety, I set it as HardStart.
        }
        return(result);
    }

*/
return HARD_START;
}


//-----------------------------------------------------------------------------
// Initialization Subroutines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// vInitOscillator
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This routine initializes the system clock to use the internal 24.5MHz
// oscillator as its clock source.  Also enables missing clock detector reset.
//
//-----------------------------------------------------------------------------
void vInitOscillator(void)
{

    // oscillator initialization
    OSCICN = 0x83;  // internal 24.5MHz. 
}

//-----------------------------------------------------------------------------
//
// Configure the Crossbar and GPIO ports
//
//-----------------------------------------------------------------------------
void vInitGPIO(void)
{

    XBR0 = 0x00;    // disable UART0
    XBR1 = 0x00;

    P0MDIN  = 0xff; // P0 used as I/O.
    P0MDOUT = 0xe0; // output pin ->Pushpull/Open-drain
    P1MDIN  = 0xff; // P1 used as I/O.
    P1MDOUT = 0xff; // Pushpull 
    
    P2MDOUT = 0xff; // Pushpull 

    P0 = 0xff;
    P1 = 0xff;		
    P2 = 0xff;

    P0SKIP = 0x00;
    P1SKIP = 0x00;

    XBR1 = 0x40; // Enable Cross Bar
}


//-----------------------------------------------------------------------------
//
// Onchip ADC0 Init,including Temperature sensor initialization
//
//-----------------------------------------------------------------------------
void vInitMcuADC0(void)
{

   	// Use Internal Vref: 
	//REF0CN &= ~0x01;				// disable internal vref
    ADC0CN = 0x00; 					// Internal Bias Generator Off. Internal Reference Buffer Off. Gain = 1

    ADC0L = 0x00;
    ADC0M = 0x00;
    ADC0H = 0x00;

	// ADC0 voltage reference from VREFA pin.ADC0ÖÐ¶ÏÔ´Ñ¡Ôñ:SINC3ÂË²¨Æ÷
   	// Use internal Vref:
	ADC0CF = 0x00; 
	//ADC0CF = 0x04;				// Interrupts upon SINC3 filter output
                        			// and uses external VREF
   	ADC0CLK = (SYSCLK_HZ / MDCLK)-1;	// Generate MDCLK for modulator.
                                	// Ideally MDCLK = 2.4576MHz

   	// Program decimation rate for desired OWR
   	ADC0DEC = ((unsigned long) MDCLK / (unsigned long) OWR /
              (unsigned long) 128) - 1;

   	ADC0BUF = 0x00;              	// Turn off Input Buffers
   	ADC0MUX = 0x08;                 // Select AIN0.0

   	ADC0MD = 0x81;                  // Start internal calibration
   	while(AD0CALC != 1);            // Wait until calibration is complete

   	EIE1   |= 0x08;                	// Enable ADC0 Interrupts
   	ADC0MD  = 0x80;               	// Enable the ADC0 (IDLE Mode)
}


//----------------------------------------------------
//             MCU  Initialization
//----------------------------------------------------

void vInitICReg(void)
{
    // HW initialization
    vInitGPIO();

    vInitOscillator(); // initialize oscillator: internal 24.5MHz.
  
	/// Init Timer0 and Timer1
    wTimer = 0L;
 
    TH0_BYTE = TIMER0_2ms_HIGHBYTE;
    TL0_BYTE = TIMER0_2ms_LOWBYTE;

	/// Timer 1 as the tick
    CKCON = 0; 		// Timer1 uses the system clock divided by 12. it's default
    TMOD = 0x15 ; 	// Timer1 in Mode1(16bit mode), timer 0 as counter driven by P0.0.
	                // C/T0 = 1, T0M = 0x01
    TH1 = TIMER0_2ms_HIGHBYTE;
    TL1 = TIMER0_2ms_LOWBYTE;
    TCON = 0x45;    // Timer1 enabled, /INT0, /INT1 are in edge triggered mode.
	                // TR1 = 1, 
    ET1 = ENABLE; 	// enable Timer1 interrupts.
    PT1 = HIGH; 		// It's in Low Priority

	/// Timer0 as counter for counting Freq.
    XBR1 = 0x50;    // T0E=1: T0 Enable; XBARE=1: Crossbar Enable
//P0SKIP = 0x01;
   	//vInitTimer2();                     // Initialize the Timer2
	SpearkerInitHW();

    vInitMcuADC0();
}


//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------

#endif