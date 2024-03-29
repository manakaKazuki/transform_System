/*******************************************************************************
*  skSPIlib.c - ＳＰＩ通信を行う関数ライブラリ                                 *
*                                                                              *
*    SPI_Init            - ＳＰＩモード(マスター)の設定と初期化を行う処理      *
*    SPI_setDataMode     - ＳＰＩの転送モード設定を行う処理                    *
*    SPI_setClockDivider - ＳＰＩの通信速度設定を行う処理                      *
*    SPI_transfer        - ＳＰＩ通信でのデータ送信とデータ受信を行う処理      *
*                                                                              *
*    メモ：SDIピンは必ず「デジタル入力ピン」に設定を行って下さい。             *
*          使用するSS(CS)ピンは初期化時にHIGHにします。                        *
*          SSP2を利用する場合は、skI2Clib.hに"#define SPI_MSSP2_USE"を宣言する *
* ============================================================================ *

/*******************************************************************************
*  SPI_Init(mode,divider,sdo)                                                  *
*    ＳＰＩモードの設定と初期化を行う処理                                      *
*    割り込みでの処理を止めて直接"SSP1IF"を見る様にしています                  *
*                                                                              *
*    mode :   SPIの転送モードを設定します(クロック極性とクロック位相の組合せ)  *
*             SPI_MODE1 = 極性(0:LOW)  位相(0:アイドル0Vで、0V->5Vに変化で転送)*
*             SPI_MODE0 = 極性(0:LOW)  位相(1:アイドル0Vで、5V->0Vに変化で転送)*
*             SPI_MODE3 = 極性(1:HIGH) 位相(0:アイドル5Vで、5V->0Vに変化で転送)*
*             SPI_MODE2 = 極性(1:HIGH) 位相(1:アイドル5Vで、0V->5Vに変化で転送)*
*    divider :SPIの通信速度を設定します                                        *
*             SPI_CLOCK_DIV4   = Fosc/4                                        *
*             SPI_CLOCK_DIV16  = Fosc/16                                       *
*             SPI_CLOCK_DIV64  = Fosc/64                                       *
*             SPI_CLOCK_DIVT2  = TMR2の出力の1/2                               *
*             SPI_CLOCK_DIVADD = FOSC/((SSPxADD + 1)*4)                        *
*    sdo :    使用するSDO送信のピン番号を指定する                              *
*******************************************************************************/

#include <xc.h>
#include "SPIlib.h"

void SPI_Init(char mode,char divider,int sdo)
{
     char con , stat ;

#if defined(_12F1822)
     SDOSEL = 0 ;                  // 7番ピン(RA0)をSDO送信ピンとする
     if (sdo == 3) SDOSEL = 1 ;    // 3番ピン(RA4)をSDO送信ピンとする
#endif

#if defined(_16F1823) 
     SDOSEL = 0 ;                  // 8番ピン(RC2)をSDO送信ピンとする
     if (sdo == 3) SDOSEL = 1 ;    // 3番ピン(RA4)をSDO送信ピンとする
#endif

#if defined(_16F1825) 
     SDO1SEL = 0 ;                 // 8番ピン(RC2)をSDO1送信ピンとする
     if (sdo == 3) SDO1SEL = 1 ;   // 3番ピン(RA4)をSDO1送信ピンとする
#endif

#if defined(_16F1826) || defined(_16F1827)
     SDO1SEL = 0 ;                 //  8番ピン(RB2)をSDO1送信ピンとする
     if (sdo == 15) SDO1SEL = 1 ;  // 15番ピン(RA6)をSDO1送信ピンとする
#endif

#if defined(_16F1829) 
     SDO2SEL = 0 ;                 // 15番ピン(RC1)をSDO2送信ピンとする
     if (sdo == 2) SDO2SEL = 1 ;   //  2番ピン(RA5)をSDO2送信ピンとする
#endif

     con  = 0b00100000 ;       // クロック極性はLOW　マスタモードでFOSC/4のクロックに設定
     stat = 0b00000000 ;       // クロック位相は０(立上りエッジでデータを送信)
     con  = con | divider ;    // 指定のクロック速度を設定する
     if (mode == SPI_MODE0 || mode == SPI_MODE2) {
          stat = stat | 0x40 ; // 指定のクロック位相を設定する(クロック位相は1とする)
     }
     if (mode == SPI_MODE2 || mode == SPI_MODE3) {
          con = con | 0x10 ;   // 指定のクロック極性を設定する(極性をHIGHとする)
     }
     SPI_SSPCON1 = con ;
     SPI_SSPSTAT = stat ;
     SPI_SSPIF   = 0 ;         // ＳＰＩの割込みフラグを初期化する
     INTCONbits.PEIE  = 1 ;               // 周辺装置割込み有効
     INTCONbits.GIE   = 1 ;               // 全割込み処理を許可する
}
/*******************************************************************************
*  SPI_setDataMode(mode)                                                       *
*    ＳＰＩの転送モード設定を行う処理                                          *
*    mode :   SPIの転送モードを設定します(クロック極性とクロック位相の組合せ)  *
*******************************************************************************/
void SPI_setDataMode(char mode)
{
     if (mode == SPI_MODE0 || mode == SPI_MODE2) {
          SPI_SSPSTAT = SPI_SSPSTAT | 0x40 ; // 指定のクロック位相を１設定する
     } else {
          SPI_SSPSTAT = SPI_SSPSTAT & 0xBF ; // 指定のクロック位相を０設定する
     }
     if (mode == SPI_MODE2 || mode == SPI_MODE3) {
          SPI_SSPCON1 = SPI_SSPCON1 | 0x10 ; // 指定のクロック極性をHIGH設定する
     } else {
          SPI_SSPCON1 = SPI_SSPCON1 & 0xEF ; // 指定のクロック極性をLOW設定する
     }
}
/*******************************************************************************
*  SPI_setClockDivider(divider,rate)                                           *
*    ＳＰＩの通信速度設定を行う処理                                            *
*    divider :SPIの通信速度を設定します                                        *
*             SPI_CLOCK_DIVADD = FOSC/((SSPxADD + 1)*4)                        *
*    rate    :SSPxADDに設定する値                                              *
*******************************************************************************/
void SPI_setClockDivider(char divider,char rate)
{
     if (divider == SPI_CLOCK_DIVADD) {
          SPI_SSPADD = rate ;
     }
     SPI_SSPCON1 = (SPI_SSPCON1 & 0xF0)  | divider ;    // 指定のクロック速度を設定する
}
/*******************************************************************************
*  ans = SPI_transfer(dt)                                                      *
*    ＳＰＩ通信でのデータ送信とデータ受信を行う処理                            *
*                                                                              *
*    dt  : ８ビットの送信するデータを指定します                                *
*    ans : ８ビットの受信したデータを返します                                  *
*******************************************************************************/
unsigned char SPI_transfer(char dt)
{
     SPI_SSPBUF = dt ;            // データの送信
     while(SPI_SSPIF == 0) ;      // 受信待ち
     SPI_SSPIF = 0 ;              // フラグクリア
     return SPI_SSPBUF ;          // データの受信
}

