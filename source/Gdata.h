
#if !defined GDATA_H
#define GDATA_H

#include "Typedefs.h"

#ifdef EXTERN
  #undef EXTERN
#endif
#ifdef AT
  #undef AT
#endif
#ifdef EQU
  #undef EQU
#endif
#ifdef SLASH
  #undef SLASH
#endif

#ifdef GDATA_C
  #define EXTERN
  #define AT( location ) _at_ ( location )
  #define EQU =
  #define EQU_ML = 
  #define EQU_MLEND
  #define CONST const
  #define SBIT sbit
#else
  #define EXTERN extern
  #define AT( location )
  #define SLASH /
  #define EQU ; /SLASH
  #define EQU_ML ; SLASH*
  #define EQU_MLEND *SLASH
  #define CONST
  #define SBIT bit
#endif

typedef enum
{
    DirectionUnknow = 0,
    DirectionToLeft,
    DirectionToRight
}eLaserDirection;


/// Power state
typedef enum
{
    FreqUnknow = 0,
        Freq270,
        Freq330,
        Freq1k,
        Freq2k
} eModulationFreq;


typedef enum
{
    BattLow = 0,
        BattMiddle,
        BattNormal, 
        BattUnknow
} eBattaryState;

/// Fiber Power state
typedef enum
{
    PowerValid = 0,
        PowerLow,
        PowerHigh, 
		PowerBothHigh,
        PowerEqual
} ePowerState;

typedef struct  
{
    eLaserDirection myLaserDirection;
    ePowerState     myPowerState;
    eModulationFreq myFreq;
    eBattaryState   myBattState;
}stSystemState;

EXTERN stSystemState gSystemState;

typedef enum
{
    TIA_START = 0,
    TIA_10k = TIA_START,
    TIA_100k, 
    TIA_1M,
	TIA_10M,
	TIA_TypeNoOfValues
} eChannelTIA;

EXTERN xdata tUINT8 	PDno;       // Current Channel             
EXTERN xdata tUINT8 	gChannelTIA[MAX_PD_NUM];	// Range index

EXTERN xdata tUINT16   	PDFrq[MAX_PD_NUM];	  	// PD 调制信号的频率
EXTERN xdata tUINT32   	PDHex[MAX_PD_NUM];	  	// PD ADC HEX VALUE
EXTERN xdata float   	PDPower[MAX_PD_NUM];	// PD power dB value
EXTERN xdata tUINT32 	ADCAcumValue;
EXTERN xdata tINT8 		ADCAcqCount;
EXTERN xdata tINT8 		gFirstCheckPower;	///< when startup, if in low power status, don't beep

EXTERN data tUINT8 TH0_BYTE;                  	// Timer0 counter_Hbyte loader
EXTERN data tUINT8 TL0_BYTE;                  	// Timer0 counter_Lbyte loader

//reliability
//EXTERN xdata tUINT32   SecondCount;			   	// second counter
//EXTERN xdata tUINT16   HardwareErrCode;        	// hardware error log
//EXTERN xdata tUINT8    bErrCode;               	// records the error item
EXTERN xdata tUINT16   SoftWDCount;            	// softdog counter
EXTERN xdata eBOOL     TriggerWatchDog;        	// watchdog

//boot
/*EXTERN xdata eStartUpType  StartUpType;        	// Boot type
EXTERN xdata tUINT8    ColdStartBuf[COLD_BUFFER_SIZE];  
EXTERN xdata eBOOL     SoftStartFlag; 
EXTERN xdata tUINT16   SoftStartPattern;

EXTERN xdata tUINT8    LedStatus;
EXTERN xdata eBOOL     TestFlag; 
*/

//--------------------------------Timer	 
EXTERN tUINT16 data    wTimer;
EXTERN tUINT16 xdata   wTmrA2D;		  // ADC Timer
EXTERN tUINT16 xdata   wTmrSPK;		  // Speaker Timer


#endif // ndef GDATA_H