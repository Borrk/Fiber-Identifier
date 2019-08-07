#ifndef FIBER_IDENTIFIER_H
#define FIBER_IDENTIFIER_H

#include "C8051F350.h"
#include "Typedefs.h"
#include "Error.h"
#include "Hw.h"
#include "ExtFuncDeclare.h"
#include "Fakefunc.h"

                              
#define ON      1
#define OFF     0
#define ENABLE  1
#define DISABLE 0
#define HIGH    1
#define LOW     0
#define SUCCESS 1
#define FAIL    0
#define LED_ON  0
#define LED_OFF 1

 

//Board Info
typedef struct
{
    tUINT8  Major;
    tUINT8  Minor;
    tUINT8  Revision;
} tFWVERSION;


// Key ascii
#define KEY_ESC         0x1B
#define KEY_BACKSPACE   0x08
#define KEY_ENTER       0x0D
#define KEY_SPACE       0x20
#define KEY_LF          0x0A
#define EOS             0x00

//UART0 BUF 
#define MMI_BUF_SIZE    0x100
#define M_SIZE          0x80

#define MAX_DARK_CURRENT	0x0050

typedef void (* VOID_RET_FUNCPTR)();
     
#endif  // ndef LUCENT_ORS_1X2_H

