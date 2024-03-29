/* 
 * File:   can_command.h
 * Author: kazuki
 *
 * Created on 2019/05/04, 13:41
 */

#ifndef CAN_COMMAND_H
#define	CAN_COMMAND_H

#include "mcc_generated_files/ecan.h"

#ifndef _XTAL_FREQ
 // Unless already defined assume 64MHz system frequency
 // This definition is required to calibrate __delay_us() and __delay_ms()
 #define _XTAL_FREQ 16000000       // 使用するPIC等により動作周波数値を設定する
#endif

#define STOP_SW PORTBbits.RB5
#define CAN_ID 0x005

unsigned char can_recieve(uCAN_MSG rxMessage);

#endif	/* CAN_COMMAND_H */

