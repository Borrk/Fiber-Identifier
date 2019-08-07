/*****************************************************************************
* 
*   Copyright (C) Brook 2011-2012
*   All rights reserved.
*
*   Module Name:    PD.c
*   Author:			Huang Yuqi
*   Description:    READ PD 
*   
*   Date: 9/11/2011
*
   	//                           Vref (mV)
   	//   measurement (mV) =   --------------- * result (bits)
   	//                       (2^24)-1 (bits)
   	//
   	//   measurement (mV) =  result (bits) / ((2^24)-1 (bits) / Vref (mV))
   	//
   	//
   	//   With a Vref (mV) of 2500:
   	//
   	//   measurement (mV) =  result (bits) / ((2^24)-1 / 2500)
   	//
   	//   measurement (mV) =  result (bits) / ((2^24)-1 / 2500)
   	//
   	//   measurement (mV) =  result (bits) / (16777215 / 2500)
   	//
   	//   measurement (mV) =  result (bits) / (6710)

//   	mV = PDHex[PD1] / 6710;        // Because of bounds issues, this
                                       	// calculation has been manipulated as
                                       	// shown above
                                       	// (i.e. 2500 (VREF) * 2^24 (ADC result)
                                       	// is greater than 2^32)
******************************************************************************/


#include <absacc.h>
#include <stdlib.h>
#include <math.h>
//#include <float.h>

#include "FIBER_IDENTIFIER.h"
#include "Gdata.h"
#include "PD.h"
#include "Speaker.h"
#include "Display.h"

#define POWER_DB_MIN -50

/// Sampling state
typedef enum
{
   SamplingStart = 0,
   SamplingAdjustRange, 
   SamplingEnd
} eSamplingState;

typedef enum
{
   RangeValid = 0,
   RangeLow,
   RangeHigh, 
   RangeUnknow
} eSamplingRange;


data tINT8 				gHigherValues, gLowerValues;
data eSamplingState 	gSamplingState = SamplingEnd;
data eSamplingRange		gRangeState;	
data tUINT16    		gElapseFreq;	

// Only if preFeq ==CurFreq, we think CurFreq is a valid one.	  
data tUINT8    			sPreFreq = FreqUnknow;	 	  

//-----------------------------------------------------------------------------
// Macro Routines
//-----------------------------------------------------------------------------
#define IsRangeHighest( ch ) 	(gChannelTIA[ch] <= TIA_START)
#define IsRangeLowest( ch ) 	(gChannelTIA[ch] >= (TIA_TypeNoOfValues-1))
#define GetTIA( ch )  			 gChannelTIA[ch]

//-----------------------------------------------------------------------------
// Voltage and Range definition
// 1. The max. dB is set to 2500 *(1-10%) = 2200 mV
// 2. Theorily the min. dB is log(100 mv / max_range)
//
//-----------------------------------------------------------------------------
#define ADC_RANGE_BASE			10000L
#define ADC_DB_MAX				(10*log10(2200.0/ADC_RANGE_BASE))
#define ADC_DB_MIN				-80 //(10*log10(10.0/(RANGE_BASE*1000)))
#define VoltRatio( TIAindex ) 	(ADC_RANGE_BASE*pow(10, (TIAindex)))
// 100k, 1M, 10M, 100M

#define InRange( val, MinVal, MaxVal ) ((MinVal<=val)&&(val<=MaxVal))



void InitPD()
{
	PDHex[PD1] = PDHex[PD2] = 0;
	PDPower[PD1] = PDPower[PD2] = 0;
    PDno = PD1;
	gChannelTIA[PD1] = TIA_START;
	gChannelTIA[PD2] = TIA_START;

	SwitchADCChannel(PD1);
	SwitchTIA( PD1,TIA_START );
	SwitchTIA( PD2,TIA_START );

	gFirstCheckPower = 1;

	sPreFreq = FreqUnknow;
}

void UpdateLedDirection()
{
    if ( PowerValid != gSystemState.myPowerState )
    {
        LedOff( LED_DIRECTION_ALL, 0 );
		Update( LED_REFRESH );
		return;
    }

    if ( DirectionToRight == gSystemState.myLaserDirection )
    {
        LedOn( LASER_RIGHT, 0 );
        LedOff( LASER_LEFT, 0 );
    }
    else if ( DirectionToLeft == gSystemState.myLaserDirection )
    {
        LedOff( LASER_RIGHT, 0 );
        LedOn( LASER_LEFT, 0 );
    }
    else
    {
        LedOff( LED_DIRECTION_ALL, 0 );
    }

	Update( LED_REFRESH );
}

void UpdateLedFreq()
{
    tUINT8 aFreqLed = 0;

    if ( PowerValid != gSystemState.myPowerState )
    {
        LedOff( LED_DIRECTION_ALL, 0 );
		Update( LED_REFRESH );
		return;
    }

    if ( Freq270 == gSystemState.myFreq )
    {
        aFreqLed = FREQ_270;
    }
    else if ( Freq330 == gSystemState.myFreq )
    {
        aFreqLed = FREQ_330;
    }
    else if ( Freq1k == gSystemState.myFreq )
    {
        aFreqLed = FREQ_1K;
    }
    else if ( Freq2k == gSystemState.myFreq )
    {
        aFreqLed = FREQ_2K;
    }
    else
    {
        /// unknow
        aFreqLed = 0;
    }

    LedOff( LED_FREQ_ALL, 0 );
    if ( aFreqLed != 0 )
    {
        LedOn( aFreqLed, 0 );        
    }

	Update( LED_REFRESH );
}

void UpdateLeds()
{
    if ( PowerValid != gSystemState.myPowerState )
    {
        LedOff( LED_DIRECTION_ALL, 0 );
        LedOff( LED_FREQ_ALL, 1 );

		return;
    }

    /// Laser direction
	UpdateLedDirection();

    /// Freq.
	UpdateLedFreq();
}

void vChangeEmergencyAlarmState( tUINT8 bOn )
{
	if ( bOn == 1 )
	{
		EmergencyAlarm( 1 );
		SetDisplayType(Disp_DynamicDEC);
	}
	else
	{
		EmergencyAlarm( 0 );
		SetDisplayType(Disp_Static);
	}
}

void vShowInvalidPowerStatus( ePowerState type )
{
	ePowerState aOldPowerState = gSystemState.myPowerState;

    gSystemState.myPowerState = type;
	gSystemState.myLaserDirection = DirectionUnknow;

	if ( type != PowerBothHigh )
	{
		SetDisplayType( Disp_Static );
	}

	if ( type == PowerLow )
	{
        DisplayLED( 0, 13, 0 ); // "L"
        DisplayLED( 1, 0, 0 );  // "O"
		//vChangeEmergencyAlarmState( 1 );	///< Alarm and sparkle
	}
	else if ( type == PowerHigh )
	{
        DisplayLED( 0, 12, 0 ); // "H"
        DisplayLED( 1, 1, 0 );  // "I"
	}
	else if ( type == PowerBothHigh )
	{
        DisplayLED( 0, 12, 0 ); // "H"
        DisplayLED( 1, 1, 0 );  // "I"
		vChangeEmergencyAlarmState( 1 );	///< Alarm and sparkle
	}
	else if ( type == PowerEqual )
	{
        DisplayLED( 0, 11, 0 ); // "-"
        DisplayLED( 1, 11, 0 );  // "-"
	}
	else
	{
	    return;
	}

    UpdateLeds();

	/// Check if power in low/high state
	if ( gFirstCheckPower == 1 )
	{
		gFirstCheckPower = 0;
		return;
	}

	if ( aOldPowerState != gSystemState.myPowerState )
	{
		if ( type == PowerLow )
			Alarm( AlarmPowerLow );
		else if ( type == PowerHigh )
			Alarm( AlarmPowerHigh );
	}
}

void vResetSamplingParam()
{
	gHigherValues = 0;
	gLowerValues = 0;
	gSamplingState = SamplingEnd;
	gRangeState = RangeUnknow;
}

void vIdentifyFreq( tUINT16 uFreq )
{
	eModulationFreq aFreq;

	if ( InRange( uFreq, FRQ_270_MIN, FRQ_270_MAX ) )
	{
		aFreq = Freq270;
	}
	else if ( InRange( uFreq, FRQ_330_MIN, FRQ_330_MAX ) )
	{
		aFreq = Freq330;
	}
	else if ( InRange( uFreq, FRQ_1K_MIN, FRQ_1K_MAX ) )
	{
		aFreq = Freq1k;
	}
	else if ( InRange( uFreq, FRQ_2K_MIN, FRQ_2K_MAX ) )
	{
		aFreq = Freq2k;
	}
	else
	{
		aFreq = FreqUnknow;
	}

	/// We look the Freq as a valid one only if this Freq is equal to the previous Freq.
	//if ( aFreq == sPreFreq )
	{
		gSystemState.myFreq = aFreq;
	}

	sPreFreq = aFreq;
}

void CalcualteFreq()
{
	tUINT32 aCount;
	tUINT32 aFreq;
    eModulationFreq  aOldFreq;
	tINT16 aCurTick = wGetTimer();

	/// Check Freq. only if power is vlaid.
	if ( PowerValid != gSystemState.myPowerState )
		return;

	aCount = TH0 *256 + TL0; //aHigh *256 + aLow;

	aCurTick = (aCurTick - gElapseFreq) * 2; /// convert to ms
	if ( aCurTick <= 0 )
		return;

	aFreq = (tUINT16)(aCount *(1000.0 / aCurTick)+0.5);
	PDFrq[PDno] = aFreq;

    if ( gSystemState.myLaserDirection == DirectionToLeft )
    {
    	aFreq = PDFrq[PD1];
    }
	else
    {
    	aFreq = PDFrq[PD2];
    }

    aOldFreq = gSystemState.myFreq;
	
	vIdentifyFreq( aFreq );

	UpdateLedFreq();
	
	/// Freq. changed
    if ( (aOldFreq != gSystemState.myFreq))
	{
	//	UpdateLedFreq();

		if ( FreqUnknow != gSystemState.myFreq ) 
			Alarm( AlarmFreqSwitch );
	}
}

void EnableADC( eBOOL argEnable )
{
	if ( argEnable == TRUE )
	{
		AD0INT = 0;
		ADC0MD = 0x83;   	// Start continuous conversions
	}
	else
	{
		ADC0MD  = 0x80;	/// Idle mode
	}
}

void SwitchADCChannel( ePD_No ch )
{
	if ( ch == PD1 ) /// Left
	{
		ADC0MUX = 0x08; // ADC power AIN0.0
		P0SKIP = 0x01; //0x00;	//T0 route to P0.0
	}
	else
	{
		ADC0MUX = 0x78; // ADC power AIN0.7
		P0SKIP = 0x00; //0x01;  // Mask P0.0,  T0 route to P0.1
	}

	PDno = ch;
}

void SwitchTIALeft( tUINT8 val )
{
	tUINT8 tmp = P1;
	P1 = tmp|0x0F;

	gChannelTIA[PD1] = val;

	switch( val )
	{
	case 0:
	TIA1_R = 0;		// 跨阻放大器的跨阻值, 10K
	break;
	case 1:
	TIA4_R = 0;		// 跨阻放大器的跨阻值, 100K
	break;

	case 2: 
	TIA3_R = 0;		// 跨阻放大器的跨阻值, 1M
	break;

	case 3:
	TIA2_R = 0;		// 跨阻放大器的跨阻值, 10M
	break;
	}
}

void SwitchTIARight( tUINT8 val )
{
	tUINT8 tmp = P1;
	P1 = tmp|0xF0;

	gChannelTIA[PD2] = val;

	switch( val )
	{
	case 0:
	TIA1_L = 0;		// 跨阻放大器的跨阻值, 10K
	break;
	case 1:
	TIA4_L = 0;		// 跨阻放大器的跨阻值, 100K
	break;

	case 2: 
	TIA3_L = 0;		// 跨阻放大器的跨阻值, 1M
	break;

	case 3:
	TIA2_L = 0;		// 跨阻放大器的跨阻值, 10M
	break;
	}
}

void SwitchTIA( ePD_No ch, tUINT8 val )
{
	if ( ch == PD2 )
		SwitchTIARight( val );
	else
		SwitchTIALeft( val );
}



void vCalculatePower()
{
   	static tUINT32 data mV[2] = {0, 0};
	tUINT8 aTIA;
	float power;
	
	/// Convert to Voltage
   	mV[0] = PDHex[PD1] / 6710;
	mV[1] = PDHex[PD2] / 6710;

	/// Calculate dB value
	aTIA = GetTIA( PD1 );
	PDPower[PD1] = 10.0* log10((float)(mV[0]) / VoltRatio(aTIA));

	aTIA = GetTIA( PD2 );
	PDPower[PD2] =  10.0* log10((float)(mV[1]) / VoltRatio(aTIA));

	/// Both channel's power > max. power  -----> explore directly under strong light
	if ( ( PDPower[0] >= ADC_DB_MAX ) && ( PDPower[1] >= ADC_DB_MAX ) )
	{
		vShowInvalidPowerStatus( PowerBothHigh );
	}
	/// Both channel's power < min. power   -----> Power low
	else if ( ( PDPower[0] <= ADC_DB_MIN ) && ( PDPower[1] <= ADC_DB_MIN ) )
	{
		vShowInvalidPowerStatus( PowerLow );
	}
	/// Both channel's powers are equal  -----> explore directly under air
	else if ( fabs(PDPower[0] - PDPower[1]) <= 0.2  )
	{
		vShowInvalidPowerStatus( PowerEqual );
	}
	/// One channel's power > max. power   -----> Power high
	else if ( ( PDPower[0] >= ADC_DB_MAX ) || ( PDPower[1] >= ADC_DB_MAX ) )
	{
		vShowInvalidPowerStatus( PowerHigh );
	}
	/// Left > right
	else if ( PDPower[0] > PDPower[1] )
	{
		power = fabs(PDPower[0]);
		if ( power > 0 )
		{
			gSystemState.myPowerState = PowerValid;
			gSystemState.myLaserDirection = DirectionToLeft;
         
		    DisplayDEC( (tUINT8)(power), 1 );
			UpdateLedDirection();
		}
		else ///< power = 0 is seem as error
		{
			vShowInvalidPowerStatus( PowerEqual ); ///< left=equal
		}
	}
	/// Left < right
	else if ( PDPower[0] < PDPower[1] )
	{
		power = fabs(PDPower[1]);
		if ( power > 0 )
		{
			gSystemState.myPowerState = PowerValid;
			gSystemState.myLaserDirection = DirectionToRight;
         
		    DisplayDEC( (tUINT8)(power), 1 );
			UpdateLedDirection();
		}
		else ///< power = 0 is seem as error
		{
			vShowInvalidPowerStatus( PowerEqual ); ///< left=equal
		}
	}

	/// If detected valid power, switch off sparkle mode
	if ( gSystemState.myPowerState == PowerValid )
	{
		vChangeEmergencyAlarmState( 0 );
	}
}

#define SWITCH_TIA_SYNC  0
#if SWITCH_TIA_SYNC
#define SWITCH_TIA( ch, tia )  SwitchTIA( 0, (tia) );SwitchTIA( 1, (tia) );
#else
#define SWITCH_TIA( ch, tia )  SwitchTIA( (ch), (tia) );
#endif

void ProcessPDSampling()
{
    tUINT8 aCurTIA = GetTIA( PDno );

	switch ( gSamplingState )
	{
	case SamplingStart:
        if ( gRangeState == RangeValid )	///< Sampling ok
        {
            gSamplingState = SamplingEnd;
        }
        else if ( gRangeState == RangeHigh )	///< Range low
        {
			if ( aCurTIA > TIA_START )
			{
				SWITCH_TIA( PDno, aCurTIA-1 );
		//		PDPower[0] = PDPower[1] = 0.0;
                gSamplingState = SamplingAdjustRange;
			}
            else
            {
                vShowInvalidPowerStatus( PowerHigh );
                gSamplingState = SamplingEnd;
            }
        }
        else if ( gRangeState == RangeLow )
        {
			if ( aCurTIA < (TIA_TypeNoOfValues-1) )
			{
				SWITCH_TIA( PDno, aCurTIA+1 );
			//	PDPower[0] = PDPower[1] = 0.0;
                gSamplingState = SamplingAdjustRange;
			}
            else
            {
            }
            
            gSamplingState = SamplingAdjustRange;
        }
		break;

	case SamplingAdjustRange:
		vResetSamplingParam();
		ADCAcumValue = 0;
		ADCAcqCount = 0;

		gSamplingState = SamplingStart;

		EnableADC(TRUE );   	// Start continuous conversions
	
		/// Start statistic Freq.
		gElapseFreq = wGetTimer();
	
		/// reset timer 0's counter
		TR0 = 0; /// Disable Timer0.
		TH0 = 0;
		TL0 = 0;
		TR0 = 1; /// Enable Timer0.

		break;

	case SamplingEnd:
		/// Calculate and display power	
		vCalculatePower();

		/// Calculate Freq.
		TR0 = 0; /// Disable Timer0.		
		CalcualteFreq();
	
		/// Switch channel
		if ( PDno == 0 )
			SwitchADCChannel(1);
		else
			SwitchADCChannel(0);

		vWait_ms(10);

		/// Start sampling another channel
		vResetSamplingParam();
		ADCAcumValue = 0;
		ADCAcqCount = 0;

		gSamplingState = SamplingStart;


		/// Start sampling
		EnableADC( TRUE );	// Start continuous conversions


		/// Start statistic Freq.
		gElapseFreq = wGetTimer();

		/// reset timer 0's counter
		TH0 = 0;
		TL0 = 0;
		TR0 = 1; /// Enable Timer0.

		break;

	default:
		break;
	}
}



//-----------------------------------------------------------------------------
// ADC0_ISR
//-----------------------------------------------------------------------------
void vInt_ADC0_EOC (void) interrupt 10
{
   	static LONGDATA data rawValue;

   	while(!AD0INT);                     // Wait till conversion complete
   	AD0INT = 0;                         // Clear ADC0 conversion complete flag

	if ( ADCAcqCount < ADC_AVERAGE_NUMBER )
	{
	   	// Copy the output value of the ADC
	   	rawValue.Byte[Byte3] = 0x00;
	   	rawValue.Byte[Byte2] = (unsigned char)ADC0H;
	   	rawValue.Byte[Byte1] = (unsigned char)ADC0M;
	   	rawValue.Byte[Byte0] = (unsigned char)ADC0L;
	
		if ( !IsRangeHighest(PDno) && rawValue.result > MCU_ADC0_STAGE1 )  
		{
			if ( ++gHigherValues > 2 ) ///< too many larger ADCs, need to switch Ω
			{
				gRangeState = RangeHigh;
				EnableADC( FALSE );
			}	
		}
		else if ( !IsRangeLowest(PDno) && (rawValue.result < MCU_ADC0_STAGE2) )
		{
			if ( ++gLowerValues > 2 ) ///< too many lower ADCs, need to switch Ω
			{
				gRangeState = RangeLow;
				EnableADC( FALSE );
			}
		}
		else
		{
	        ADCAcumValue += rawValue.result;
			if ( ++ADCAcqCount == ADC_AVERAGE_NUMBER )
			{
				EnableADC( FALSE );
		    	PDHex[PDno] = (ADCAcumValue >> ADC_AVERAGE_BIT);
				gRangeState = RangeValid;
			}
		}
	}
}
