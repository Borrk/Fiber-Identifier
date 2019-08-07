/*****************************************************************************
* 
*   Copyright Photonic Ltd. (C) 2012
*   All rights reserved.
*
*   Module Name:    Battary.h
*   Author:
*   Description:    Display API declaration. 
*   
*   Author:  Huang Yu Qi
*   Date: 04/01/2012
*
******************************************************************************/

#ifndef BATTARY_H
#define BATTARY_H

#include "Typedefs.h"


//LED definition:
typedef enum
{
	BattaryStateStart = 0,
	BattaryNormal = BattaryStateStart,
	BattaryMiddle,
	BattaryLow,
	BattaryStateTypeOfValues
}eBattaryState;

/// Basic APIs
eBOOL 	InitBattMonitor( );

eBattaryState  CheckBattaryState();

/// Application APIs

#endif  // end of BATTARY_H
