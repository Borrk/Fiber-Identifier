/*****************************************************************************
* 
*   Copyright(C) Brook 2012
*   All rights reserved.
*
*   Module Name:    Display.h
*   Author:
*   Description:    Display API declaration. 
*   
*   Author:  Huang Yu Qi
*   Date: 04/01/2012
*
******************************************************************************/

#ifndef DISPLAY_H
#define DISPLAY_H

#include "Typedefs.h"


//LED definition:
//D7 D6 D5 D4 D3 D2 D1 D0
#define BATT_GRN     		0x01
#define BATT_RED     		0x02
#define LASER_RIGHT  		0x04
#define LASER_LEFT 	 		0x08
#define FREQ_270     		0x10
#define FREQ_330     		0x20
#define FREQ_1K 	    	0x40
#define FREQ_2K 	    	0x80
#define LED_DIRECTION_ALL   (LASER_RIGHT | LASER_LEFT)
#define LED_FREQ_ALL        (FREQ_270 | FREQ_330 | FREQ_2K | FREQ_1K)
#define LED_BATT_ALL        (BATT_RED | BATT_GRN)
#define LED_ALL         	0xFF

typedef enum  
{
    Disp_Static = 0,
    Disp_DynamicDEC, 
    Disp_DynamicLED
}eSparkleType;

#define LED_REFRESH 0

/// Basic APIs
eBOOL 	InitScreen( );
void    DisplayDEC( tUINT8 num, tUINT8 bUpdate );
void    DisplayLED( tUINT8 ledIdx, tUINT8 val, tUINT8 bUpdate );
void    LedOn( tUINT8 ledIndex, tUINT8 bUpdate );
void    LedOff( tUINT8 ledIndex, tUINT8 bUpdate );
void    Update( tINT8 bForced );
tUINT8  IsLedOn( tUINT8 led );


/// Application APIs
void	TriggerDisplaySparkle();
void 	SetDisplayType( eSparkleType eDisplayType );	///< make the DEC led sparkle.

#endif  // end of DISPLAY_H
