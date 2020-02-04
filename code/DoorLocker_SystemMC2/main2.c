/*
 * main2.c
 * Created on: Jan 27, 2020
 * Author: Noha , Soha , Somaya
 */
#include "std_types.h"
#include "macro_config.h"

#include "usart.h"
#include "timer.h"
#include "eeprom.h"
#include "lcd.h"

#define USED_FIRST 0x05
#define FIRST_USED_VAL_LOCATION 0x0D

#define PASSWORD_LOCATION 0x320  /* the address in the External EEPROM that password will be save  in  */
uint8 volatile counter =0 , tick;
void drivers_init(); /* prototype of drivers initialization */
void receivePassword(); /* prototype of function that receive the password and write it in EEPROM  */
void UART_msgIdentify(uint8 msg); /* prototype of function contain message that MC1 transmit it to MC2  by uart to make MC2 take actions according to it */
void sendOldPassword(); /* prototype of function that send the password that saved in EEPROM to MC1 by uart */
void prepareToSend(uint8 array[], uint8 array2[]);/* prototype of function to add # to the new password in the buffer that is save in to prepare send it by uart to MC1 */
void rotateMotor(); /* prototype of function that open the door for 15s by rotate motor clock wise then close the door after 15s by rotate motor anti clock wise */
void prepareMotorPlusTimer(); /* prepare timer and motor and set its configuration */
void turnOnBuzzer(); /* prototype of function that turn on buzzer because the password is entered wrong 3 times */
void usingCheck();
void setFirstUsed();
void turnOffBuzzer();
void main()
{

	DDRD|=(1<<7); /* make buzzer pin as output */
	DDRB =0XFF; /* lcd data pins as output to test the data*/

	uint8 msg; uint8 passwordCode1[6];
	drivers_init();
	LCD_displayString(" welcome to our ");
	LCD_goToRowColumn(1,3);
	LCD_displayString("system");
	_delay_ms(300);
	LCD_clearScreen();
	while(1)
	{
		msg = USART_recieveByte(); /* receive message from MC1 to take actions according to it */
		UART_msgIdentify(msg); /* call function contain message that MC1 transmit it to MC2  by uart to make MC2 take actions according to it */
	}
}

/* Initialization of drivers "UART , LCD , External eeprom "*/
void drivers_init()
{
	/* usart set configrations */
	usart_configType UART_configration;
	UART_configration.modeSelect=Asynchronous;
	UART_configration.numOfBits =bit_8;
	UART_configration.numOfstopBit= stop_1bit;
	UART_configration.parityBit= No_Parity;
	UART_configration.RX_or_TX=Both_transmition_and_receiver;
	USART_init(&UART_configration);

	LCD_init();
	EEPROM_init();
}

/* function that receive the password and write it in EEPROM  */
void receivePassword()
{

	uint8 passwordCode1[6]   , passwordCode2[6], check;

	USART_receiveString(passwordCode1);

	check = EEPROM_writeString(PASSWORD_LOCATION ,passwordCode1);
	//EEPROM_writeByte(0X333,0X03);

	if (check == SUCCESS)
	{
		uint8 check2 = EEPROM_readString(PASSWORD_LOCATION ,passwordCode2 , 5 );
		//EEPROM_readByte(0X333,0X03);
		if (check2 == SUCCESS)
		{
			LCD_displayString("pass = ");
			LCD_goToRowColumn(1,0);
			LCD_clearScreen();
			LCD_displayString(passwordCode2);
		}
	}

}
/* function that send the password that saved in EEPROM to MC1 by uart */
void sendOldPassword()
{
	uint8 oldPassword [6] , oldPasswordPrepared [7];
	uint8 check2 = EEPROM_readString(PASSWORD_LOCATION ,oldPassword , 5 );
	if (check2 == SUCCESS)
	{
		LCD_displayString("pass = ");
		LCD_goToRowColumn(1,0);
		LCD_clearScreen();
		LCD_displayString(oldPassword);
	}
	prepareToSend(oldPassword, oldPasswordPrepared);
	USART_sendString(oldPasswordPrepared);
	//USART_sendByte(0X03);
}

/* identify the messages that MC1 transmit it to MC2  by uart to make MC2 take actions according to it */
void UART_msgIdentify(uint8 msg)
{
	//PORTA|=(1<<PA3);
	switch (msg)
	{
		case SAVE_PASS : receivePassword(); break;
		case OPEN_DOOR_REQ :sendOldPassword(); break;
		case OPEN_DOOR :prepareMotorPlusTimer(); break;
		case CHANGE_PASSWORD: sendOldPassword(); break;
		case WRONG_PASSWORD:turnOnBuzzer(); break;
		case FIRST_USE_CHECK :usingCheck(); break;
		case SET_FIRST_USE: setFirstUsed(); break;
	}
}
/* function to add # to the new password in the buffer that is save in to prepare send it by uart to MC1 */
void prepareToSend(uint8 array[], uint8 array2[])
{
	sint8 i = 0;
	for (i = 0; i < 6; i++)
	{
		array2[i] = array[i];
		if (i == 5)
		{
			array2[i] = '#';
			//break;
		}
	}
	array2[i] ='\0';
}

/* this fuction is called by ISR "call back"
 * it open the door for 15s by rotate motor clock wise then close the door after 15s by rotate motor anti clock wise then stop */
void rotateMotor()
{
	counter ++;
	if (counter ==15)
	{
		/* unti clock wise */
		PORTC |= (1<<PC6);
		PORTC &=~(1<<PC7);
		LCD_displayString("anti clockwise ");
		LCD_clearScreen();
	}
	else if (counter == 30)
	{
		/*  stop motor */
		PORTC &= ~(1<<PC7);
		PORTC &=~(1<<PC6);
		LCD_displayString("stop");
		//TIMER1_stop();
	}
}
/* prepare timer and motor and set its configuration */
void prepareMotorPlusTimer()
{
	/* prepare Motor */
	DDRC|=(1<<PC6)|(1<<PC7);
	/* rotate motor clock wise */
	PORTC |= (1<<PC7);
	PORTC &=~(1<<PC6);

	/* prepare time */
	TIMER_config config; // set timer configrations
	config.channel= timer1;
	config.waveMode.TIMER1_waveMode= TIMER1_CTC_OCR1A;
	config.clock.Timer_0_1_clock = TIMER_0_1_FCPU_64;
	config.initalValue =0;
	config.finalValue = 15624;
	config.compOut=0;
	SREG |=(1<<7); // I BIT Enable
	TIMER_init(&config);
	Timer1_setCallBack(rotateMotor);

}

/*function that turn on buzzer for 1 min by using timer 1 because the password is entered wrong 3 times */
void turnOnBuzzer()
{
	PORTD|=(1<<7);

	/* prepare time */
	TIMER_config config; // set timer configrations
	config.channel= timer1;
	config.waveMode.TIMER1_waveMode= TIMER1_CTC_OCR1A;
	config.clock.Timer_0_1_clock = TIMER_0_1_FCPU_64;
	config.initalValue =0;
	config.finalValue = 15624;
	config.compOut=0;
	SREG |=(1<<7); // I BIT Enable
	TIMER_init(&config);
	Timer1_setCallBack(turnOffBuzzer);

   /* ************* Another Method **************
		for (int i=0;i<12 ;i++)
		_delay_ms(60000); // delay for 1 min util buzzer finish
		PORTD&=~(1<<7);
	 ************************************************/
}

/* this fuction is called by ISR "call back"
 * it turn off motor */
void turnOffBuzzer()
{
	tick ++;
	if (tick == 60)
	{
		PORTD&=~(1<<7);
		USART_sendByte( BUZZER_OFF);
	}
}

/* this function is used every time run the system to check if the user has already use the system or not*/
void usingCheck()
{
	uint8 val;
	EEPROM_readByte(FIRST_USED_VAL_LOCATION , &val);
	USART_sendByte(val);
}

/* this function is used in first time used the system when the user sign up and enter password and confirm on it
 * then the MC1 send fixed value by USART and through this function we save this value in External EEPROM
 */
void setFirstUsed()
{
	EEPROM_writeByte(FIRST_USED_VAL_LOCATION , USED_FIRST);
}
