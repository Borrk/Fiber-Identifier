/*****************************************************************************
* 
*   Copyright(C) Brook 2012
*   All rights reserved.
*
*   Module Name:    Display.c
*   Author:
*   Description:    Display API. 
*   
*   Author:  Huang Yu Qi
*   Date: 04/01/2012
*
******************************************************************************/

#ifndef DISPLAY_C
#define DISPLAY_C

//#include <stdio.h> 
#include <string.h>
#include <absacc.h>
#include <intrins.h>

#include "Hw.h"
#include "Display.h"
#include"ExtFuncDeclare.h"

#define LED_OFF 0xFF

typedef struct
{
    tINT8	myLED1;	///< LED for 十位
    tINT8	myLED2;	///< LED for 个位
    tINT8	myLED3;	///< LEDs for Freq, Direction, Battary...
    tINT8   myChanged;
	tINT8   mySparkleState;
	eSparkleType	myDisplayType; ///< Sparkle or not
} stVirtualScreen;

stVirtualScreen sVirtualScreen;

void vResetData()
{
    sVirtualScreen.myLED1 = LED_OFF;
    sVirtualScreen.myLED2 = LED_OFF;
    sVirtualScreen.myLED3 = LED_OFF;
    sVirtualScreen.myChanged = 0;
	sVirtualScreen.myDisplayType = Disp_Static;
	sVirtualScreen.mySparkleState = 0;
}

eBOOL InitScreen( )
{
    vResetData();

	return TRUE;
}

void Update( tINT8 bForced )
{
    if ( (0 == bForced) &(0 == sVirtualScreen.myChanged) )
    {
        return;
    }
    
    vHC1642( sVirtualScreen.myLED2 );   ///十位
    vHC164( sVirtualScreen.myLED1 );    ///个位
    vLEDs( sVirtualScreen.myLED3 );	///Leds
    
	/// After send all data, output display trigger signle.
	HC164_RCK0 = 0;
	vWait_ms(10);
	HC164_RCK0 = 1;

    sVirtualScreen.myChanged = 0;
}

void DisplayDEC(tUINT8 num, tUINT8 bUpdate )
{
    tUINT8 data j,k;
    
    k = num % 10;
    j = num / 10;
    if ( (k != sVirtualScreen.myLED2) || (j != sVirtualScreen.myLED1) )
    {
        sVirtualScreen.myLED1 = j;
        sVirtualScreen.myLED2 = k;
        sVirtualScreen.myChanged = 1;
    }

    if ( (1 == bUpdate)/* || (1 == sVirtualScreen.myChanged)*/ )
        Update( 1 );
}

void DisplayLED( tUINT8 ledIdx, tUINT8 val, tUINT8 bUpdate )
{
    if ( 0 == ledIdx )
    {
        sVirtualScreen.myLED1 = val;
        sVirtualScreen.myChanged = 1;
    }
    else if ( 1 == ledIdx )
    {
        sVirtualScreen.myLED2 = val;
        sVirtualScreen.myChanged = 1;
    }
  
    if ( (1 == bUpdate)/* || (1 == sVirtualScreen.myChanged)*/ )
        Update( 1 );
}

void LedOn( tUINT8 ledIndex, tUINT8 bUpdate )
{
    if ( ledIndex != IsLedOn(ledIndex) )
    {
        sVirtualScreen.myLED3 = sVirtualScreen.myLED3 & (~ledIndex);
        sVirtualScreen.myChanged = 1;
    }
 
    if ( (1 == bUpdate) /* || (1 == sVirtualScreen.myChanged)*/ )
        Update( 1 );
}

void LedOff( tUINT8 ledIndex, tUINT8 bUpdate )
{
    if ( ledIndex != IsLedOn(ledIndex) )
    {
        sVirtualScreen.myLED3 = sVirtualScreen.myLED3 | ledIndex;
        sVirtualScreen.myChanged = 1;
    }    
 
    if ( (1 == bUpdate) /*|| (1 == sVirtualScreen.myChanged)*/ )
        Update( 1 );
}

tUINT8 IsLedOn( tUINT8 ledIndex )
{
    return ~(sVirtualScreen.myLED3 & ledIndex);
}


void vUpdate( tUINT8 bOn )
{
	if ( bOn )
	{
	    vHC1642( sVirtualScreen.myLED2 );   ///十位
	    vHC164( sVirtualScreen.myLED1 );    ///个位
	    vLEDs( sVirtualScreen.myLED3 );	///Leds
	}
	else
	{
		if ( sVirtualScreen.myDisplayType == Disp_DynamicDEC )
		{
		    vHC1642( 11 );   ///十位
		    vHC164( 11 );    ///个位
		    vLEDs( sVirtualScreen.myLED3 );	///Leds
		}
		else if ( sVirtualScreen.myDisplayType == Disp_DynamicLED )
		{
		    vHC1642( sVirtualScreen.myLED2 );   ///十位
		    vHC164( sVirtualScreen.myLED1 );    ///个位
		    vLEDs( LED_OFF );	///Leds
		}
	}

	/// After send all data, output display trigger signle.
	HC164_RCK0 = 0;
	vWait_ms(10);
	HC164_RCK0 = 1;
}

void 	SetDisplayType( eSparkleType eDisplayType )
{
	if ( sVirtualScreen.myDisplayType != eDisplayType )
	{
		sVirtualScreen.myDisplayType = eDisplayType;
		//sVirtualScreen.mySparkleState = 0;
	}
}

void	TriggerDisplaySparkle()
{
	vUpdate( sVirtualScreen.mySparkleState );
	sVirtualScreen.mySparkleState = (sVirtualScreen.mySparkleState == 0 ) ? 1 : 0;
}

#endif  ///< end of DISPLAY_C
