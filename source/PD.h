/*****************************************************************************
* 
*   Copyright Photonic Ltd. (C) 2012
*   All rights reserved.
*
*   Module Name:    PD.h
*   Author:
*   Description:    PD API. 
*   
*   Author:  Huang Yu Qi
*   Date: 04/01/2012
*
******************************************************************************/

#ifndef PD_H
#define PD_H

#include "Typedefs.h"


/// Basic APIs
void InitPD();
void EnableADC( eBOOL argEnable );

void SwitchADCChannel( ePD_No ch );
void SwitchTIA( ePD_No ch, tUINT8 val );
//tUINT8 GetTIA( ePD_No ch );

/// Application APIs
void UpdateLeds();

#endif  // end of PD_H
