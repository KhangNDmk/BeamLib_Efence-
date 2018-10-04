#include <xc.h>
#include "Beam_StateFunction.h"
#include "Beam_Init.h"
#include "Beam_ModuldeDefine.h"
#include "extern_var.h"

void Beam_DataBuffer_WriteNextData(uint8_t nextData[32])
{
    int dataID = 0;
    if (Beam_DataBuffer_CanWrite)
    {
        for (dataID = 0; dataID < 32; dataID++)
        {
            Beam_DataBuffer[1 - Beam_DataBuffer_SendID][dataID] = nextData[dataID];
        }
        Beam_DataBuffer_CanWrite = 0;
    }   
}

void Beam_DataBuffer_WriteData(uint8_t Data[32])
{
    int dataID = 0;
    for (dataID = 0; dataID < 32; dataID++)
    {
        Beam_DataBuffer[Beam_DataBuffer_SendID][dataID] = Data[dataID];
    }
}
