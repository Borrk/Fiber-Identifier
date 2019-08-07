/*****************************************************************************
* 
*   Copyright (C) 2011
*   All rights reserved.
*
*   Module Name:    74HC164.c
*   Author:
*   Description:    LEDs functions
*   
*   $Workfile: 74HC164.c $
*   $Author:  $
*   Date: 9/11/2011 
*
******************************************************************************/

#include "FIBER_IDENTIFIER.h"
#include "Gdata.h"


//     A
//    ---
// F | G | B
//    ---
// E |   | C
//    --- . DP
//     D
// D7 D6 D5 D4 D3 D2 D1 D0
// DP G  F  E  D  C  B  A
//tUINT8 code Led_Show[]=
//{0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F,0x40,0x00,0x76,0x38,0x79,0x7C
//,0x71};//数码管显示码表（共阴） 0-9,-,全灭,H,L,E,b,F

// D7 D6 D5 D4 D3 D2 D1 D0
// X  X  X  D5 D4 D3 D2 D1
//tUINT8 code Led_Map[]=
//{0xFE,0xFD,0xFB,0xF7,0xEF,0xFF};//LED显示码表（共阳） D1-D5,全灭


//     A
//    ---
// F | G | B
//    ---
// E |   | C
//    --- . DP
//     D
// D7 D6 D5 D4 D3 D2 D1 D0
// G  F  E  D  B  C  DP A
tUINT8 code Led_Show2[]=
{0xDE,0x06,0xBC,0xAE,0x66,0xEA,0xFA,0x86,0xFE,0xEE,0x20,0x00,0x76,0x58,0xF8,0x7A,0xF0};
//数码管显示码表（共阴） 0-9,-,全灭,H,L,E,b,F

/// 十位数 
tUINT8 code Led_Show[]=
{0xDB,0x03,0xE9,0xAB,0x33,0xBA,0xFA,0x0B,0xFB,0xBB,0x20,0x00,0x73,0xD0,0xF8,0xF2, 0x78};
//数码管显示码表（共阴） 0-9,-,全灭,H,L,E,b,F

//-----------------------------------------------------------------------------
//
// 功能:数码管显示子程序
//
//-----------------------------------------------------------------------------
void vHC164(tUINT8 temp )
{
    tUINT8 data i;
    tUINT8 data temp1;
    temp1 = Led_Show2[temp];
    for(i = 0;i < 8;i++ )
    {
        HC164_SCK1 = 0;
        if( (temp1 & 0x80) == 0x80 )
            HC164_DIN1 = 0;
        else
            HC164_DIN1 = 1;
        HC164_SCK1 = 1;
        temp1 <<=1;
    }
}

void vHC1642(tUINT8 temp )
{
    tUINT8 data i;
    tUINT8 data temp1;
    temp1 = Led_Show[temp];
    for(i = 0;i < 8;i++ )
    {
        HC164_SCK1 = 0;
        if( (temp1 & 0x80) == 0x80 )
            HC164_DIN1 = 0;
        else
            HC164_DIN1 = 1;
        HC164_SCK1 = 1;
        temp1 <<=1;
    }
}

//-----------------------------------------------------------------------------
//
// 功能:LED显示子程序
//
//-----------------------------------------------------------------------------
void vLEDs(tUINT8  temp )
{
    tUINT8 data i,temp1;

    temp1 = temp;
    for(i = 0;i < 8;i++ )
    {
        HC164_SCK0 = 0;
        if( (temp1 & 0x80) == 0x80 )
            HC164_DIN0 = 1;
        else
            HC164_DIN0 = 0;
        HC164_SCK0 = 1;
        temp1 <<=1;
    }

/*    tUINT32 data i,temp1;

    temp1 = 0xff0000;
    for(i = 0;i < 24;i++ )
    {
        HC164_SCK0 = 0;
        if( (temp1 & 0x1) == 0x1 )
            HC164_DIN0 = 0;
        else
            HC164_DIN0 = 1;
        HC164_SCK0 = 1;
        temp1 >>=1;
    }
*/
}

//-----------------------------------------------------------------------------
//
// 显示1个2位数的十进制数
//
//-----------------------------------------------------------------------------
void vDispDEC(tUINT8 temp)
{
	tUINT8 data j,k;

	k = temp % 10;
	j = temp / 10;
	vHC164(k);		//个位
	vHC164(j);		//十位
}

//-----------------------------------------------------------------------------
//
// LED1闪烁
//
//-----------------------------------------------------------------------------
void vLed1Blinking(void)
{
/*
	TestFlag ^= 1;
	if ( TestFlag == HIGH)
	{
    	LedStatus &= D1_270_ON;
//    	LedStatus |= D5_1K_OFF;
//    	LedStatus |= D4_2K_OFF;
    	vLEDs( LedStatus );
		
	}
	else
	{
    	LedStatus |= D1_270_OFF;
//    	LedStatus |= D5_1K_OFF;
//    	LedStatus |= D4_2K_OFF;
    	vLEDs( LedStatus );
		
	}
*/
}

//-----------------------------------------------------------------------------
//
// LED2闪烁
//
//-----------------------------------------------------------------------------
void vLed2Blinking(void)
{
/*	TestFlag ^= 1;
	if ( TestFlag == HIGH)
	{
    	LedStatus &= D2_RIGHT_ON;
//    	LedStatus |= D5_1K_OFF;
//    	LedStatus |= D4_2K_OFF;		
	}
	else
	{
    	LedStatus |= D2_RIGHT_OFF;
//    	LedStatus |= D5_1K_OFF;
//    	LedStatus |= D4_2K_OFF;
	}
  
   	vLEDs( LedStatus );	
*/		
}
