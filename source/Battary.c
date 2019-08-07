/*****************************************************************************
* 
*   Copyright Photonic Ltd. (C) 2012
*   All rights reserved.
*
*   Module Name:    Battary.c
*   Author:
*   Description:    Battary API. 
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
#include "Battary.h"

eBattaryState sBattaryState;

eBOOL 	InitBattMonitor( )
{
	sBattaryState = BattaryNormal;
}

eBattaryState  CheckBattaryState()
{
	// Check current battary state
	// tba
	
	return sBattaryState;
}

#endif  ///< end of DISPLAY_C
