#include <xc.h>
#include "Beam_StateFunction.h"
#include "Beam_Init.h"
#include "Beam_ModuldeDefine.h"
#include "extern_var.h"

uint8_t pattern_default[3]={0xff, 0x33, 0xcc};      /*Pattern mac dinh */

void Beam_Param_DefaultInit(void)
{
    Beam_LedChanel_Init();
    Beam_Tbit455us_Init();
    Beam_PatternParam_Init(pattern_default);
}

void Beam_LedChanel_Init()
{
#ifdef BeamTX_Module

    
#endif
#ifdef BeamRX_Module
    LED0Set = LedRX;
    LED1Set = LedRX;
    LED2Set = LedRX;
    LED3Set = LedRX;
#endif
}

void Beam_Tbit455us_Init()
{
    TMR1 = 0x00;
    Cycle = 0x38E;
    T1CONbits.TCKPS=1;
    T1CONbits.TON=0;
    IFS0bits.T1IF = 0;
    IEC0bits.T1IE = 0;
}

void Beam_TbitParam_Init(uint16_t tbits)
{
    TMR1 = 0x00;
    Cycle = tbits * 2 ;
    T1CONbits.TCKPS=1;
    T1CONbits.TON=0;
    IFS0bits.T1IF = 0;
    IEC0bits.T1IE = 0;
}

void Beam_PatternParam_Init(uint8_t pattern[3])
{
    Beam_Pattern[0] = pattern[0];
    Beam_Pattern[1] = pattern[1];
    Beam_Pattern[2] = pattern[2];
}

void Beam_Transfer_Start()
{
    T1CONbits.TON = 1;
    IFS0bits.T1IF = 0;
    IEC0bits.T1IE = 1;
    IPC0bits.T1IP = 3;
    Beam_State = SessionStartBit;
}

void Beam_Transfer_Stop()
{
    T1CONbits.TON = 0;
    IFS0bits.T1IF = 0;
    IEC0bits.T1IE = 0;
}

void Beam_Transfer_Reset()
{
    Beam_State = SessionStartBit;       /* Dat lai trang thai dau */
#ifdef  BeamRX_Module  
    Beam_TimeOut = 0;                   /* Xoa cac loi trong Module*/
   Beam_FaultByte = 0;                 /* Khong xoa du lieu trong buff */
#endif    
    Beam_CurrentStateCount = 0;
    Cycle = Beam_Period ;
}