/*****************************************************************************
* 
*   Copyright (C) 2011
*   All rights reserved.
*
*   Module Name:    Timer.c
*   Author:
*   Description:    Timer support functions
*   
*   $Workfile: Timer.c $
*   $Author:  $
*   Date: 9/11/2011
*
******************************************************************************/

#include <absacc.h>
#include <intrins.h>

#include "FIBER_IDENTIFIER.h"
#include "Gdata.h"


void vInitTimer0(void)
{
    wTimer = 0L;

    CKCON = 0; 		// Timer0&1 uses the system clock divided by 12. it's default
    TMOD = 0x11; 	// Timer0&1 in Mode1(16bit mode)
    TH0 = 0;
    TL0 = 0;
    TCON = 0x15; 	// Timer0 enabled, /INT0, /INT1 are in edge triggered mode.
    ET0 = ENABLE; 	// enable Timer0 interrupts.
    PT0 = LOW; 		// It's in Low Priority

    TH0_BYTE = TIMER0_2ms_HIGHBYTE;
    TL0_BYTE = TIMER0_2ms_LOWBYTE;

}

void vInitTimer1(void)
{
    TH1 = TIMER1_20ms_HIGHBYTE;
    TL1 = TIMER1_20ms_LOWBYTE;
    ET1 = ENABLE; 	// enable Timer1 interrupts.
    PT1 = LOW; 		// It's in Low Priority
}

//-----------------------------------------------------------------------------
// Timer2_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This function configures Timer2 as a 16-bit reload timer, interrupt enabled.
// Using the SYSCLK at 16MHz/8 with a 1:12 prescaler.
//
// Note: The Timer2 uses a 1:12 prescaler.  If this setting changes, the
// TIMER_PRESCALER constant must also be changed.
//-----------------------------------------------------------------------------
void vInitTimer2(void)
{
   	CKCON &= ~0x60;                     // Timer2 uses SYSCLK/12
   	TMR2CN &= ~0x01;

   	//TMR2RL = TIMER2_RELOAD;             // Reload value to be used in Timer2
   	TMR2RL = (0x10000-0.3*SYSCLK_HZ/1000/12);             // Reload value to be used in Timer2
   	TMR2 = TMR2RL;                      // Init the Timer2 register

   	TMR2CN = 0x04;                      // Enable Timer2 in auto-reload mode
   	ET2 = 1;                            // Timer2 interrupt enabled
}


//-----------------------------------------------------------------------------
// Interrupt Service Routines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//
// Timer 1 interrupt
//
//-----------------------------------------------------------------------------
void vInt_TIM1(void) interrupt 3
{
    TH1 = TH0_BYTE; 	// default: TIMER0_2ms_HIGHBYTE;
    TL1 = TL0_BYTE; 	// default: TIMER0_2ms_LOWBYTE;

    wTimer++; 			// system ticks in heart beating interval   
    
    // 2. Enable/Reset WD
    RefreshWatchDog_();
    if ( ++SoftWDCount >= WATCH_DOG_TIMEOUT )
    {
     //   FOREVER; // Stall here, WatchDog will reset the CPU.     
    } 
}

//-----------------------------------------------------------------------------
// Timer 2 interrupt
//-----------------------------------------------------------------------------
//
// Here we process the Timer2 interrupt and toggle the Speaker
//
//-----------------------------------------------------------------------------

/*
void vInt_TIM2 (void) interrupt 5
{
	tUINT8 data LedOldStatus;

	SPK ^= 1;   // Toggle the Speaker per 125us
    LedOldStatus = LedStatus;
    if ( ~LedOldStatus & 0x19 )	//Have modulation signal?
    {
		SPK ^= 1;   // Toggle the Speaker per 125us
    }
	else
    {
		if ( DisableSPK == ENABLE)
		{
			SPK = LOW;   // Disable the Speaker
		}
		else
		{
			SPK ^= 1;   // Toggle the Speaker per 125us
		}
    }
   	TF2H = 0;	// Reset Interrupt
}

*/


//-----------------------------------------------------------------------------
//
// Get current Timer
// Out: wTimer
//
//-----------------------------------------------------------------------------
tUINT16 wGetTimer(void)
{
    tUINT16 data wTmpTimer;

    DisableHeartBeatingInterrupts_();
    wTmpTimer = wTimer;
    EnableHeartBeatingInterrupts_();
    return(wTmpTimer);
}

//-----------------------------------------------------------------------------
//
// Check timeout in ms. Current system ticks will compare with saved time.
// Inp: wms - timeout in ms
//      pwTime - pointer to saved time
// Out: FALSE - no timeout, TRUE - timeout was happen
//
//-----------------------------------------------------------------------------
eBOOL tTimeout(tUINT16 wInterval, tUINT16 *pwTime)
{
    tUINT16 data w0;

    w0 = wGetTimer();

    if ( (w0 - *pwTime) >= wInterval )
    {
        *pwTime = w0; 	// save new value
        return(TRUE); 	// timeout was happen
    }
    return(FALSE); 		// no timeout
}


// Interrupts are OFF while invoking this function.
void vWait_us( tUINT16 bCnt )
{
    tUINT16 data i;
    
    for ( i = 0 ; i < bCnt ; i++ )
    {
        _nop_ ();
        _nop_ ();
        _nop_ ();
        _nop_ ();
        _nop_ ();
        _nop_ ();
        _nop_ ();
        _nop_ ();
        _nop_ ();
        _nop_ ();
        _nop_ ();
        _nop_ ();
    }
}

void vWait_ms( tUINT16 bCnt )
{
    tUINT16 data i,j;

//    FEED_DOG_(); // Clean SoftDog, avoid watch_dog reset

    for (i = 0; i< bCnt; i++)
    {
    	FEED_DOG_(); // Clean SoftDog, avoid watch_dog reset
		for ( j = 0; j < 2450; j++ );
    }   
}


//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------
