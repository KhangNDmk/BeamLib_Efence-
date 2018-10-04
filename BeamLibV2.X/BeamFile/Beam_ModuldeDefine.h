/* Khoi tao Module la TX hay RX, comment 1 trong 2 */

#define BeamTX_Module
//#define BeamRX_Module

#define Beam_4Chanel
//#define Beam_6Chanel
//#define Beam_8Chanel

/* Khoi tao cac trang thai trong BeamTransfer*/
#define SessionStartBit 10
#define SessionSyncBit 11
#define SessionPattern 12

#define BeamStartBit 20
#define BeamSyncBit 21
#define BeamCheckBit 22

/* Khoi tao IO, LED0 -> LED7  == Bit 0-> Bit7 */
/* Khoi tao Module TX*/
#ifdef BeamTX_Module
#define LedSLAT LATAbits.LATA1
#define LedOutputData LATAbits.LATA2
#define LedSclock LATAbits.LATA3
#define LedOutputEnable  LATBbits.LATB4
#define LedOutputReset LATAbits.LATA4

#endif

/* Khoi tao Module RX*/
#ifdef BeamRX_Module
#define LED0Set TRISAbits.TRISA2
#define LED1Set TRISAbits.TRISA3
#define LED2Set TRISBbits.TRISB4
#define LED3Set TRISAbits.TRISA4

#define LED0 PORTAbits.RA2
#define LED1 PORTAbits.RA3
#define LED2 PORTBbits.RB4
#define LED3 PORTAbits.RA4
#endif

/* Dat thong so cho IO */
#define LedTX 0
#define LedRX 1

/* Dat Tham so Thoi gian bao loi */
#define Cycle PR1
#define TimeOutFault 400 /* So chu ki bao loi khong xac dinh duoc StartBit*/
#define SyncTimeOut 20   /* So chu ki khong xac dinh duoc SyncBit */

/* Dat Tham so cho  */