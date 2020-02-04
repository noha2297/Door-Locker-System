/*
 * main1.c
 *
 *  Created on:
 *  Author: Noha , Soha , Somaya
 */
#include "std_types.h"
#include "macro_config.h"

#include "lcd.h"
#include "keypad.h"
#include "usart.h"
#include "timer.h"

void drivers_init(); /* prototype of drivers initialization */
void signUp(); /* prototype of sign up function that execute just in first time */
void systemSettings(); /* prototype of system settings function that contain setting of open the door and change password */
void enterPassword (char * passwordArr); /* prototype of enter password function that save password in buffer after convert it to character */
void prepareToSend(uint8 array[], uint8 array2[]); /* prototype of function to add # to the new password in the buffer that is save in to prepare send it by uart to MC2 */
void prepareTimer(); /* prepare timer and set its configuration */

#define USED_FIRST 0x05 /* the value that will be save in the Internal EEPROM in the first time of the program */
#define INTERNA_EEPROM_ADDRESS 0x0D /* the address in the Internal EEPROM that the value (0x05) will be save  in the first time of the program */
uint8 flag = 0; /* flag for switch between system setting and sin up function */
uint8 passwordMessage[7];/* contains the string to be sent by UART_sendString() */
uint8 counter=0 , openingMsgFlag =0;

int main (void)
{

	uint8 val;
	drivers_init();
    //send msg to MC2 to read a value (0x05) from address (0x0D) in exernal EEPROM to check if the first time
	USART_sendByte(FIRST_USE_CHECK);
	val = USART_recieveByte();
	if (val != USED_FIRST)
	{
		flag = 0;

	}
	else if (val == USED_FIRST)
	{
		flag = 1;
	}

	while(1)
	{
	if (flag==0)
		{
			signUp(); /* sign up function that execute just in first time */
		}
		else if (flag==1)
		{
			if (openingMsgFlag ==0)
				systemSettings(); /* contain setting of open the door and change password */
		}
	}
}
/* Initialization of drivers "LCD , UART " */
void drivers_init()
{
	LCD_init();

	/* set UART configrations */
	usart_configType UART_configration;
	UART_configration.modeSelect=Asynchronous;
	UART_configration.numOfBits =bit_8;
	UART_configration.numOfstopBit= stop_1bit;
	UART_configration.parityBit= No_Parity;
	UART_configration.RX_or_TX=Both_transmition_and_receiver;
	USART_init(&UART_configration);
}


/* sign up function that execute just in first time ,its steps:
 * 1. it request from user to enter the password and confirm
 * 2. it then convert it to string  , then save the new password again to make a confirmation
 * 3. if there are match ,uart send the new password to MC2 to save it in
 * External EEPROM ,if there are not match the lcd will display invalid match .
 */
void signUp()
{
	uint8 passwordCode1[6] = {0},  passwordCode2[6] = {0}; /* two buffer for save the new password and its confirmation */

	/*LCD_displayString("Welcome to Our ");
	LCD_goToRowColumn(1, 0);
	LCD_displayString("system");
	LCD_clearScreen();*/

	LCD_displayString("Please enter new");/* display that string */
	LCD_goToRowColumn(1, 0);
	LCD_displayString("password (5 Num)");
	_delay_ms(1000);
	LCD_clearScreen();

	enterPassword(&passwordCode1); /* save password in buffer after convert it to string */


	while (1)
	{

		LCD_clearScreen();
		LCD_displayString("Please rewrite password ");
		LCD_goToRowColumn(1, 0);
		LCD_displayString("password (5 Num)");
		_delay_ms(1000);
		LCD_clearScreen();

		enterPassword(&passwordCode2);
		LCD_clearScreen();

		if (strcmp(passwordCode1 , passwordCode2)==0) /* if the two password are identical and Matching */
		{
	       /*send msg to MC2 to write a fixed value (USED_FIRST) in address (INTERNA_EEPROM_ADDRESS) of Internal EEPROM to check it in the first time of the program */
		    USART_sendByte(SET_FIRST_USE);
			flag = 1; /* make the flag =1 it mean the sign up function will not execute again and the system function it will be execute in the second time of program */
			LCD_clearScreen();
			LCD_displayString(" Done ");
			LCD_displayString(passwordCode1);
			prepareToSend(passwordCode1,passwordMessage); /* add # to the new password in the buffer that is save in to prepare send it by uart to MC2 */
			USART_sendByte(SAVE_PASS); /* send msg to MC2 to can understand that it will SAVE Password now in External EEPROM */
			USART_sendString(passwordMessage);/* uart send the new password to MC2 save it in the External EEPROM */
			_delay_ms(1000);
			break;
		}
		else
		{
			/* to repeat again the sign up function */
			//flag=0;
			LCD_displayString("invalid match");
			_delay_ms(1000);
		}
	}
}
/* system settings function that contain some setting :
 * 1. open the door by  press x
 * 2. change password by press +
 * */
void systemSettings()
{
	uint8 passwordCode[6];
	uint8 oldPassword[6] , count =0;
	LCD_clearScreen();

	LCD_displayString("open the door ");
	LCD_goToRowColumn(1 ,0);
	LCD_displayString("(press X )");
	_delay_ms(800);
	LCD_clearScreen();
	LCD_displayString("Change password ");
	LCD_goToRowColumn(1 ,0);
	LCD_displayString("(press + )");
	_delay_ms(800);

	if (KeyPad_getPressedKey()== '*') /* open the door */
	{
			LCD_clearScreen();
			USART_sendByte(OPEN_DOOR_REQ); /* send msg to MC2 to can understand that it will receive the old password from MC2 */
			USART_receiveString(oldPassword); /* uart  receive the old password from MC2 to compare it with current password */
			while (1)
			{
				LCD_clearScreen();
				LCD_displayString("enter password");
				_delay_ms(1000);
				LCD_clearScreen();
				enterPassword(&passwordCode);
				if(count ==2)
				{
					LCD_clearScreen();
					LCD_displayString("Exceed 3 times");
					/* send msg to MC2 to can understand that it will turn on buzzer because the password is entered wrong 3 times */
					USART_sendByte(WRONG_PASSWORD);

					/* recieve finising bit when the buzzer finished its work */
					uint8 buzzerOff =USART_recieveByte();
					if(buzzerOff == BUZZER_OFF)
						break;
				   /* ************* Another Method **************
						for (int i=0;i<12 ;i++)
						_delay_ms(60000); // delay for 1 min util buzzer finish
						break;
					 ************************************************/

				}
				if(strcmp(passwordCode , oldPassword)==0 && count <2)
				{
					count=0;

					USART_sendByte(OPEN_DOOR); /* send msg to MC2 to can understand that it will open the door */

					//LCD_clearScreen();
					//LCD_displayString("open door");
					LCD_clearScreen();
					LCD_displayString("Door is opening");
					//_delay_ms(1000);
					/* prepare timer to display on lcd door unlocking for 15s when the door open by dc motor for 15s,then display on lcd door locking for 15s when the door close by dc motor for 15s */
					openingMsgFlag =1;
					prepareTimer();

					break;
				}
				count++;
				LCD_clearScreen();
				LCD_displayString("invalid match");
				_delay_ms(500);

			}

	}
	else if (KeyPad_getPressedKey()== '+') /* change password */
	{
		uint8 oldPassword2[6] ;
		USART_sendByte(CHANGE_PASSWORD); /* send msg to MC2 to can understand that it will receive the old password from MC2 */
		USART_receiveString(oldPassword); /* uart  receive the old password from MC2 to compare it with current password */

		while(1)
		{

			LCD_clearScreen();
			LCD_displayString("please enter ");
			LCD_goToRowColumn(1 ,0);
			LCD_displayString("old pass: ");
			_delay_ms(1000);
			enterPassword(oldPassword2);
			if(count ==2)
			 {
			   LCD_clearScreen();
			   LCD_displayString("Exceed 3 times");
			   USART_sendByte(WRONG_PASSWORD); /* send msg to MC2 to can understand that it will turn on buzzer because the password is entered wrong 3 times */

				/* recieve finising bit when the buzzer finished its work */
				uint8 buzzerOff =USART_recieveByte();
				if(buzzerOff == BUZZER_OFF)
					break;
			   /* ************* Another Method **************
					for (int i=0;i<12 ;i++)
					_delay_ms(60000); // delay for 1 min util buzzer finish
					break;
				 ************************************************/
			 }
			if (strcmp(oldPassword , oldPassword2)==0&& count <2)
			{
				count=0;
				LCD_clearScreen();
				LCD_displayString("SUCESS "); /* display sucess  if enter the old password right */
				_delay_ms(1000);
				LCD_clearScreen();
				signUp(); /* return back to sign up function to change password and enter the new one  */
				break;
			}
			count++;
			LCD_clearScreen();
			LCD_displayString("invalid match");  /* display invalid if enter the old password 3 times wrong */
			_delay_ms(500);
			}
		}

}

/* function that save password in buffer after convert it to character */
void enterPassword (char * passwordArr)
{
	uint8 key ;
	for (int i=0;i<5;i++)
	{
		/* convert from number to character then , put them in the array*/
		passwordArr[i] = KeyPad_getPressedKey()+48; /* 48 -> is the ascii of character 0 */
		LCD_displayCharacter('*');
		_delay_ms(500); /* press time */
	}
	passwordArr[5]='\0';
}

/* function to add # to the new password in the buffer that is save in to prepare send it by uart to MC2 */
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

/* function that pass to another function ,it display the door is unlocking for 15s when the door is opening ,then display the door is locking after 15s when the door is closed  in the MC2 */
void Timer_lcd_unlocking()
{
	counter ++;
	if (counter ==15)
	{
		LCD_clearScreen();
		LCD_displayString("Door closing");
	}
	else if (counter == 30)
	{

		LCD_clearScreen();
		openingMsgFlag=0;
		//systemSettings();
		//TIMER1_stop();
	}
}

/* prepare timer and set its configuration */
void prepareTimer()
{

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
	Timer1_setCallBack(Timer_lcd_unlocking);

}
