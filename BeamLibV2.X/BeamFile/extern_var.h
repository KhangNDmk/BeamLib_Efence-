extern volatile uint8_t Beam_State;

extern volatile uint8_t Beam_CurrentStateCount;
extern volatile uint16_t Beam_Period;

extern volatile uint8_t Beam_Pattern[3]; 
extern volatile uint8_t Beam_DataBuffer[2][32];
extern volatile uint8_t Beam_DataBuffer_SendID;           
extern volatile uint8_t Beam_DataBuffer_CanWrite;   

extern volatile uint16_t Beam_TimeOut;                     /* Tinh thoi gian, wait SessionStartBit bao nhieu lan */
extern volatile uint16_t Beam_LostSessionCount[8];             /* Tinh so loi trong Session */
extern volatile uint16_t Beam_LostSequenceCount[8];
extern volatile uint8_t  Beam_FaultByte;  