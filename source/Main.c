/*****************************************************************************
* 
*   Copyright(C) Brook 2012
*   All rights reserved.
*
*   Module Name:    Main.c
*   Author:         Huang Yuqi
*   Description:    Main functions
*   
*   Date: 1/11/2012
*
******************************************************************************/

#include <string.h>
#include <absacc.h>
#define MAIN_C
#define GDATA_C

#include "FIBER_IDENTIFIER.h"
#include "Gdata.h"
#include "Speaker.h"
#include "Display.h"
#include "PD.h"


eBOOL   InitSystemState()
{
    gSystemState.myBattState = BattNormal;
    gSystemState.myFreq      = FreqUnknow;
    gSystemState.myLaserDirection = DirectionUnknow;
    gSystemState.myPowerState = PowerEqual;

	return TRUE;
}

void Init()
{
	/// Initialization
    DisableWatchDog_();
    DisableGlobalInterrupts_();

 
    vInitICReg();
	InitScreen();
	InitSpeaker();

	InitSystemState();

	/// Init ADC 
	InitPD();
 
	/// Init           
    EnableGlobalInterrupts_();    
        
	DisplayDEC(0, 1);
    LedOff( LED_ALL, 1 );	

	Alarm( AlarmStartup );

	vWait_ms(500);

    EnableWatchDog_(); // 42ms WD Interval.
    TriggerWatchDog = FALSE; 
    wTmrA2D = wGetTimer();
    wTmrSPK = wGetTimer();
			

	EnableADC( TRUE );
}

void main(void)
{
	char aCnt = 0;
	char temp=1;

	Init();


    FOREVER                                                                
    {   
		#if 1
        //1.  A2D Manager
        if ( tTimeout( TIMER_20ms, &wTmrA2D ) )
		{
			ProcessPDSampling();
		} 

        if ( tTimeout( TIMER_500ms, &wTmrSPK ) )
		{
			TriggerDisplaySparkle();
		} 
		#endif

        FEED_DOG_();
    }
}
