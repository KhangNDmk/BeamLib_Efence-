#include <xc.h>
#include "Beam_StateFunction.h"
#include "Beam_Init.h"
#include "Beam_ModuldeDefine.h"

volatile uint8_t Beam_State = 0;
volatile uint8_t Beam_Pattern[3]; /* Pattern byte */
volatile uint16_t Beam_Period;

volatile uint8_t Beam_BitIndex = 0;
volatile uint8_t Beam_ByteIndex = 0;
volatile uint16_t Beam_SubSessionIndex = 0;
volatile uint8_t Beam_CurrentStateCount = 0;    /* dem so lan vao ngat cua trang thai*/

volatile uint8_t Beam_DataBuffer[2][32];       /* Bo dem luu du lieu thu-phat*/
volatile uint8_t Beam_DataBuffer_SendID = 0;   /* Chi so cua mang dang duoc send */
volatile uint8_t Beam_DataBuffer_CanWrite = 0; /* Dieu kien co the ghi data cho session tiep theo*/

volatile uint8_t Beam_Data_ByteSend; /* Gia tri byte dang duoc thu-phat */
volatile uint8_t Beam_Data_BitSend;  /* Gia tri bien dang duoc thu-phat */

volatile uint8_t Beam_LedSendByte;
volatile uint8_t Beam_LedReadByte;
volatile uint8_t Beam_LedState;

#ifdef BeamRX_Module
volatile uint16_t Beam_TimeOut = 0;         /* Tinh thoi gian, wait SessionStartBit bao nhieu lan */
volatile uint16_t Beam_LostSessionCount[8]; /* Tinh so loi trong Session */
volatile uint16_t Beam_LostSequenceCount[8];
volatile uint8_t Beam_FaultByte = 0; /* Luu thong tin cac loi */
/* Bit 0: Loi TimeOut, qua thoi gian chua tim duoc StartBit
     * Bit 2: Loi qua thoi gian chua xac dinh duoc thoi gian SyncBit, 
     *          co the do co chum tia lien tuc chieu vao
     * Bit 3: Loi Checkbit, do mot chum tia bi chan - khong thu duoc
     */
#endif

void Beam_PeriodCall()
{
#ifdef BeamRX_Module
    Beam_LedToData_Read();
    Beam_TimeOut += 1;
    if (Beam_TimeOut > TimeOutFault)
    {
        Beam_FaultByte |= 1;
    }
#endif
    switch (Beam_State)
    {
    case SessionStartBit:
        Beam_StartBit_Fcn(SessionSyncBit);
        break;
    case SessionSyncBit:
        Beam_SyncBit_Fcn(SessionPattern);
        break;
    case SessionPattern:
        Beam_Pattern_Fcn();
        break;
    case BeamStartBit:
        Beam_StartBit_Fcn(BeamSyncBit);
        break;
    case BeamSyncBit:
        Beam_SyncBit_Fcn(BeamCheckBit);
        break;
    case BeamCheckBit:
        Beam_CheckBit_Fcn();
        break;
    }
}

void Beam_StartBit_Fcn(uint8_t nextState)
{
    Cycle = Beam_Period;
#ifdef BeamTX_Module
    Beam_LedSendByte = 0xff; /* Tat ca cac Led deu muc logic 1*/
    Beam_DataToLed_Send();   /* Chuyen sang trang thai Led */

    Beam_CurrentStateCount += 1;
    /* Kiem tra dieu kien chuyen trang thai, khi phat duoc 3 lan StartBit */
    if (Beam_CurrentStateCount == 3)
    {
        Beam_State = nextState;
        Beam_CurrentStateCount = 0;
        Cycle = Beam_Period / 2; /* Dat lai chu ki ngat Timer2 */
    }
#endif

#ifdef BeamRX_Module

#ifdef Beam_4Chanel
    if (Beam_LedReadByte == 0x0f) /* Check xem co phai StartBit khong */
#elif Beam_6Chanel
    if (Beam_LedReadByte == 0x3f)
#elif Beam_8Chanel
    if (Beam_LedReadByte == 0xff)
#endif
    { /* Neu nhan duoc Startbit 2 lan thi chuyen trang thai */
        Beam_CurrentStateCount += 1;
        if (Beam_CurrentStateCount == 2)
        {
            Beam_State = nextState;
            Beam_CurrentStateCount = 0;
            Cycle = Beam_Period / 4;
        }
    }
    else
    { /* khong nhan duoc Startbit, chuyen ve wait Startbit */
        Beam_State = SessionStartBit;
        Beam_CurrentStateCount = 0;
    }
#endif
}

void Beam_SyncBit_Fcn(uint8_t nextState)
{
#ifdef BeamTX_Module
    Beam_LedSendByte = 0x00; /* Tat ca cac Led deu muc logic 0 */
    Beam_DataToLed_Send();   /* Chuyen sang trang thai Led */
    /* Dat lai thoi gian ngat va chuyen trang thai */
    Cycle = Beam_Period / 2;
    Beam_State = nextState;
    Beam_CurrentStateCount = 0;
#endif

#ifdef BeamRX_Module
    Beam_CurrentStateCount += 1;

    if (Beam_LedReadByte == 0)
    {
        Beam_State = nextState;
        Beam_CurrentStateCount = 0;
        Beam_State = 3 * Beam_Period / 4;
    }

    if (Beam_CurrentStateCount > SyncTimeOut)
    {
        Beam_State = SessionStartBit; /* Dat lai trang thai wait SessionStartBit*/
        Beam_CurrentStateCount = 0;
        Cycle = Beam_Period;
        Beam_FaultByte |= 2; /*  Set bit cua byte bao loi*/
    }
#endif
}

void Beam_Pattern_Fcn()
{
    Cycle = Beam_Period;
    Beam_LedSendByte = Beam_Pattern[Beam_CurrentStateCount];
#ifdef Beam_4Chanel
    Beam_LedSendByte &= 0x0f;
#elif Beam_6Chanel
    Beam_LedSendByte &= 0x3f;
#elif Beam_8Chanel
    Beam_LedSendByte &= 0xff;
#endif

#ifdef BeamTX_Module
    /* Lay gia tri pattern byte se gui */
    Beam_DataToLed_Send();
    Beam_CurrentStateCount += 1;
    if (Beam_CurrentStateCount == 3)
    { /* Moi khi vao SubSession moi, reset het cac bien dem*/
        Beam_State = BeamStartBit;
        Beam_CurrentStateCount = 0;
        Beam_BitIndex = 0;
        Beam_ByteIndex = 0;
    }
#endif

#ifdef BeamRX_Module
    Beam_LedReadByte &= Beam_LedSendByte;

    if (Beam_LedReadByte == Beam_LedSendByte)
    {
        Beam_CurrentStateCount += 1;
        if (Beam_CurrentStateCount == 3)
        {
            Beam_TimeOut = 0;
            Beam_CurrentStateCount = 0;
            Beam_State = BeamStartBit;
            Beam_BitIndex = 0;
            Beam_ByteIndex = 0;
        }
    }
    else
    {
        Beam_State = SessionStartBit; /* Dat lai trang thai wait SessionStartBit*/
        Beam_CurrentStateCount = 0;
    }
#endif
}

void Beam_CheckBit_Fcn()
{
    Beam_Data_ByteSend = Beam_DataBuffer[Beam_DataBuffer_SendID][Beam_ByteIndex];
    Beam_Data_BitSend = (Beam_Data_ByteSend >> Beam_BitIndex) & 1;
    Beam_LedSendByte = Beam_Data_BitSend << Beam_CurrentStateCount;
#ifdef BeamTX_Module
    Beam_DataToLed_Send();
#endif

#ifdef BeamRX_Module
    Beam_LedReadByte &= Beam_LedSendByte;
    if (Beam_LedReadByte != Beam_LedSendByte)
    {
        Beam_LostSessionCount[Beam_CurrentStateCount] += 1;
        Beam_FaultByte |= 4;
    }
#endif
    /* Cap nhat gia tri BitIndex va StateCount moi */
    Beam_CurrentStateCount += 1;
    Beam_BitIndex += 1;
    Beam_CheckBit_Update();
}

void Beam_CheckBit_Update()
{ /* Kiem tra dieu kien de chuyen SubSession tiep theo */
    if (Beam_BitIndex == 8)
    {
        Beam_ByteIndex += 1;
        Beam_BitIndex = 0;
        if (Beam_ByteIndex == 32)
        {
            Beam_ByteIndex = 0;
            Beam_State = SessionStartBit;
            Beam_CurrentStateCount = 0;
            Beam_DataBuffer_CanWrite = 1;
            Beam_DataBuffer_SendID = 1 - Beam_DataBuffer_SendID;
        }
    }

#ifdef Beam_4Chanel
    if (Beam_CurrentStateCount == 4)
#elif Beam_6Chanel
    if (Beam_CurrentStateCount == 6)
#elif Beam_8Chanel
    if (Beam_CurrentStateCount == 8)
#endif
    {
        Beam_State = BeamStartBit;
        Beam_CurrentStateCount = 0;
    }
}

void Beam_DataToLed_Send() /* Chuyen du lieu sang trang thai Led */
{
    int i;
    for (i=0; i<4; i++)
    {
        LedOutputData = ( Beam_LedSendByte >> (3-i)) & 1;
        LedSclock = 0;
        LedSclock = 1;       
    }
    LedSLAT= 0;
    LedSLAT = 1;

}

#ifdef BeamRX_Module
void Beam_LedToData_Read()
{ /* Luu du lieu Led thu duoc vao Beam_LedReadByte */
    Beam_LedReadByte = LED0 + (LED1 << 1) + (LED2 << 2) + (LED3 << 3);
#ifdef Beam_6Chanel
    Beam_LedReadByte += ((LED4 << 4) + (LED5 << 5));
#elif Beam_8Chanel
    Beam_LedReadByte += ((LED4 << 4) + (LED5 << 5) + (LED6 << 6) + (LED7 << 7));
#endif
}
#endif