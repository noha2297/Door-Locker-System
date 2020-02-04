/*
 * timer.h
 *
 *  Created on: Jan 6, 2020
 *      Author:
 */

#ifndef TIMER_H_
#define TIMER_H_

#include "macro_config.h"
#include "std_types.h"
/*******************************************************************************
 *                         Types Declaration                                   *
 *******************************************************************************/
#define NULL (void *)0

typedef enum
{
	TIMER_0_2_Normal  , TIMER_0_2_CTC =1
}TIMER_0_2_waveGnerationMode;

typedef enum
{
	TIMER1_Normal ,TIMER1_CTC_OCR1A =4,TIMER1_CTC_ICR1 =12
}TIMER1_waveGnerationMode;

typedef enum
{
	normal ,toggle , clear , set
}TIMER_compOutMode;


typedef enum
{
	TIMER_0_1_No_clock , TIMER_0_1_FCPU , TIMER_0_1_FCPU_8 , TIMER_0_1_FCPU_64 ,
	TIMER_0_1_FCPU_256 , TIMER_0_1_FCPU_1024 , TIMER_0_1_ExternalOnFalling ,TIMER_0_1_ExternalOnRising

}TIMER_0_1_clock;
typedef enum
{
	TIMER2_No_clock , TIMER2_FCPU , TIMER2_FCPU_8 , TIMER2_FCPU_32 ,
	TIMER2_FCPU_64 ,TIMER2_FCPU_128, TIMER2_FCPU_256 , TIMER2_FCPU_1024
}TIMER2_clock;

typedef enum
{
	timer0  , timer1  , timer2
}TIMER_channel;

typedef struct
{
	TIMER_channel  channel;
	TIMER_compOutMode compOut;
	union Timer_WaveMode
	{
		TIMER_0_2_waveGnerationMode TIMER_0_2_waveMode;
		TIMER1_waveGnerationMode TIMER1_waveMode;
	}waveMode;

	union Timer_clock
	{
		TIMER_0_1_clock Timer_0_1_clock;
		TIMER2_clock Timer_2_clock;

	}clock;

	uint16 initalValue; 				/* SET =0 as an initial value */
	uint16 finalValue; 				/* SET =0 as an initial value */
}TIMER_config;


/*******************************************************************************
 *                         Function Prototype                                   *
 *******************************************************************************/



void TIMER_init(const TIMER_config  * configOfTimer );
static void TIMER0_init(const TIMER_config  * configOfTimer);
static void TIMER2_init(const TIMER_config  * configOfTimer);
static void TIMER1_init( const TIMER_config  * configOfTimer );
void TIMER1_stop();
void Timer0_setCallBack(void(*ptr0Func)(void));
void Timer1_setCallBack(void(*ptr1Func)(void));
void Timer2_setCallBack(void(*ptr2Func)(void));

#endif /* TIMER_H_ */
