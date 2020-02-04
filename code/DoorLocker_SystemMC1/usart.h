/*
 * usart.h
 *
 *  Created on: Jan 20, 2020
 *      Author: Lenovo
 */

#ifndef USART_H_
#define USART_H_

#include "common_macros.h"
#include "std_types.h"
#include "macro_config.h"
#include "usart_Messege.h"

#define INTERRUPT 0
#define USART_BAUDRATE 9600
/* ****************************************************************************** */
/* *************************** Types Declaration  ******************************* */
/* ***************************************************************************** */

typedef enum
{
	bit_5  , bit_6 , bit_7  , bit_8
}dataBits;

typedef enum
{
	No_Parity  , even_Parity =2 ,odd_Parity
}parityMode;

typedef enum
{
	stop_1bit , stop_2bits
}stopBit;
typedef enum
{
	Asynchronous , Synchronous
}USART_ModeSelect;

typedef enum
{
	transmition_only =1 , receiver_only , Both_transmition_and_receiver
}RecieveOrtransmit;

typedef struct
{
	dataBits numOfBits ;
	parityMode parityBit;
	stopBit numOfstopBit;
	RecieveOrtransmit RX_or_TX ;
	USART_ModeSelect modeSelect;

}usart_configType;

/* ****************************************************************************** */
/* *************************** Function Prototype ******************************* */
/* ***************************************************************************** */
void USART_init(const usart_configType * a_configrations);

void USART_sendByte(const uint8 data);

uint8 USART_recieveByte(void);

void USART_sendString(const uint8 *str);

void USART_receiveString(uint8 *str); // Receive until #

#endif /* USART_H_ */
