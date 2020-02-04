/*
 * timer.c
 *
 *  Created on: Jan 6, 2020
 *      Author:
 */
#include "timer.h"


/*******************************************************************************
 *                           Global Variables                                  *
 *******************************************************************************/

/* Global variables to hold the address of the call back function in the application */

static volatile void (*g_CallBackPtr0)(void)=NULL;
static volatile void (*g_CallBackPtr1)(void)=NULL;
static volatile void (*g_CallBackPtr2)(void)=NULL;


/*******************************************************************************
 *                       Interrupt Service Routines                            *
 *******************************************************************************/


/*ISR for Timer 0  Normal mode */
ISR(TIMER0_OVF_vect)
{
   if(g_CallBackPtr0!=NULL)
      {
	   (*g_CallBackPtr0)();
      }

}

/*ISR for Timer 1  Normal mode */
ISR(TIMER1_OVF_vect)
{
   if(g_CallBackPtr1!=NULL)
      {
	   (*g_CallBackPtr1)();
      }

}

/*ISR for Timer 2  Normal mode */
ISR(TIMER2_OVF_vect)
{
   if(g_CallBackPtr2!=NULL)
      {
	   (*g_CallBackPtr2)();
      }

}

/*ISR for Timer 0  Compare mode */
ISR(TIMER0_COMP_vect)
{
	if(g_CallBackPtr0!=NULL)
	      {
		   (*g_CallBackPtr0)();
	      }
}

/*ISR for Timer 1  Compare mode */
ISR(TIMER1_COMPA_vect)
{
	if(g_CallBackPtr1!=NULL)
	      {
		   (*g_CallBackPtr1)();
	      }
}

/*ISR for Timer 2  Compare mode */
ISR(TIMER2_COMP_vect)
{
	if(g_CallBackPtr2!=NULL)
	      {
		   (*g_CallBackPtr2)();
	      }
}



void TIMER_init(const TIMER_config  * configOfTimer  )
{

	if(configOfTimer ->channel == timer1)
	{
		TIMER1_init(configOfTimer);
	}
	else if (configOfTimer ->channel == timer0)
	{
			TIMER0_init(configOfTimer);
	}
	else
	{
		TIMER2_init(configOfTimer);

	}
}

/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/
/*
 * Description : Function to initialize the Timer driver
 * 1. Non PWM mode FOC0=1
 * 2. set normal mode or CTC mode
 * 3. set Compare unit or display it
 * 4. Enable the Timer Interrupt.
 * 5.Set the required clock.
 */
static void TIMER0_init( const TIMER_config  * configOfTimer )
{
	TCCR0 = (1<<FOC0) | (TCCR0 |  (configOfTimer ->waveMode.TIMER_0_2_waveMode <<WGM01 ) ) | (TCCR0 | (configOfTimer ->compOut ) <<COM00) |(TCCR0|configOfTimer->clock.Timer_0_1_clock);
	TCNT0=configOfTimer->initalValue;

	if (configOfTimer->waveMode.TIMER_0_2_waveMode == TIMER_0_2_Normal )
		TIMSK |= (1<<TOIE0);

	else if (configOfTimer->waveMode.TIMER_0_2_waveMode== TIMER_0_2_CTC)
	{
		TIMSK |= (1<<OCIE0);
		OCR0 = configOfTimer->finalValue;
	}
}


 static void TIMER2_init( const TIMER_config  * configOfTimer )
 {
 	TCCR2 = (1<<FOC2) | (TCCR2 |  (configOfTimer ->waveMode.TIMER_0_2_waveMode <<WGM21 ) ) | (TCCR2 | (configOfTimer ->compOut) <<COM20) |(TCCR2|configOfTimer->clock.Timer_2_clock);
 	TCNT2=configOfTimer->initalValue;

 	if (configOfTimer->waveMode.TIMER_0_2_waveMode == TIMER_0_2_Normal )
 		TIMSK |= (1<<TOIE2);

 	else if (configOfTimer->waveMode.TIMER_0_2_waveMode== TIMER_0_2_CTC)
 	{
 		TIMSK |= (1<<OCIE2);
 		OCR2 = configOfTimer->finalValue;
 	}
 }


static void TIMER1_init( const TIMER_config  * configOfTimer )
{
	/*
	 * 1) Wave generation Mode table have 4 bits WGM10: WGM13 first 2 bits in TCCR1A  and the other  2bits in TCCR1
	 * So we take first 2 bits by clear the others and it is done by  make the Wave mode value & 0x03 " 0b00000011 "  and then put it in TCCR1A
	 * then we take the last 2 bits by clear the others and it is done by  make the Wave mode value & 0x0C " 0b00001100 "  and then put it in TCCR1B
	 * 2) we assign the  value of Compare out table of channel A/B COM1A0 /COM1B0
	 * 3) we assign the value of the clock in TCCR1B register
	 * 4)assign initial value in TCNT1 register
	 * 6) Put the top value in OCR1A or  ICR1 in case of CTC mode
	 * 7) enable the suitable interrupt
	 */
	TCCR1A = (1<<FOC1A) |(1<<FOC1B) | ( TCCR1A | (configOfTimer->waveMode.TIMER1_waveMode &0x03) ) | (TCCR1A |(configOfTimer->compOut << COM1A0) ) ;
	TCCR1B = ( TCCR1B | ( (configOfTimer->waveMode.TIMER1_waveMode &0x0C) <<1 ) ) |(TCCR1B | configOfTimer->clock.Timer_0_1_clock);
	TCNT1=configOfTimer->initalValue;

	switch (configOfTimer->waveMode.TIMER1_waveMode)
	{
		case TIMER1_Normal: TIMSK |= (1<<TOIE1); break; // OCR1A VALUE ??

		case TIMER1_CTC_OCR1A : OCR1A = configOfTimer->finalValue;
								TIMSK |= (1<<OCIE1A); break;

		case TIMER1_CTC_ICR1 :  ICR1 = configOfTimer->finalValue;
								TIMSK |= (1<<OCIE1A); break;

	}

}

/*
 * Description: Function to set the Call Back function address.
 */
void Timer0_setCallBack(void(*ptr0Func)(void))

{
	/* Save the address of the Call back function in a global variable */
  g_CallBackPtr0=ptr0Func;

}
void Timer1_setCallBack(void(*ptr1Func)(void))

{
	/* Save the address of the Call back function in a global variable */
  g_CallBackPtr1=ptr1Func;

}
void Timer2_setCallBack(void(*ptr2Func)(void))

{
	/* Save the address of the Call back function in a global variable */
  g_CallBackPtr2=ptr2Func;
}
