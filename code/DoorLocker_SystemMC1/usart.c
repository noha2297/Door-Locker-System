/*
 * usart.c
 *
 *  Created on: Jan 20, 2020
 *      Author: Lenovo
 */
#include "usart.h"

/********** baud rate *********/
#define BAUDRATE_PRESCALE  ( ((F_CPU / (USART_BAUDRATE *8UL)) ) -1)

#if INTERRUPT
volatile bool g_recieveComplete =1  , g_transmitionComplete =0 , g_dataRegEmpty=0;

ISR(USART_RXC_vect)
{
	g_recieveComplete=1;
}
ISR(USART_TXC_vect)
{
	g_transmitionComplete=1;
}
ISR(USART_UDRE_vect)
{
	g_dataRegEmpty=1;
}
#endif


void USART_init(const usart_configType * a_configrations)
{
	/* we work on double speed mode */
	SET_BIT( UCSRA  , U2X);  	/* UCSRA|=(1<<USX); */

	#if (INTERRUPT)
	 {
		if(a_configrations->RX_or_TX != receiver_only)
			UCSRB|=(1<<UDRIE);

		UCSRB|=(a_configrations->RX_or_TX << TXCIE);
	 }
	#endif

	 /* clear register to make sure that every bit =0 before assign any value on it */
	UCSRB=0; UCSRC=0;

	/* enable TX or RX or Both */
	UCSRB |=(a_configrations->RX_or_TX << TXEN);

	/************************** UCSRC Description **************************
	 * URSEL   = 1 The URSEL must be one when writing the UCSRC
	 * UMSEL   = a_configrations->modeSelect  -> 0 Asynchronous  , 1 Synchronous
	 * UPM1:0  =  parity bit Mode
	 * USBS    = number of stop bits
	 * UCSZ1:0 = number of bits data mode
	 * UCPOL   = 0 Used with the Synchronous operation only
	 ***********************************************************************/
	/* ex : UCSRC=(UCSRC& 0XF9)| (1 << UCSZ0) */
	 UCSRC |= (1<<URSEL)|(a_configrations->numOfBits << UCSZ0)|(a_configrations->parityBit <<UPM0)|(a_configrations->numOfstopBit <<USBS);

	 if (a_configrations->modeSelect == Synchronous) // if work on synchronous mode
		 UCSRC|= (1<<UMSEL);

	 /* Adjust the baud rate  */
	 UBRRH = BAUDRATE_PRESCALE >>8;
	 UBRRL=BAUDRATE_PRESCALE;

}

void USART_sendByte(const uint8 data)
{

	#if INTERRUPT
	/* if the data register is empty , the put the new data in UDR register */
		if (g_dataRegEmpty ==1)
		{
			UDR= data;
			g_dataRegEmpty=0;
		}
	while (BIT_IS_CLEAR(UCSRA , UDRE));
	UDR= data;
	#else
	/* *****************steps *********************
	 * Loop until the previous data all transmitted
	 * put the new data in the UDR to be transmitted
	 * when data i put in UDR then the UDRE is cleared automatic
	 * ******************************************** */
	while (BIT_IS_CLEAR(UCSRA , UDRE));
	UDR= data;


	 /* ************** Another Method *****************
	   UDR =data;
	   while(BIT_IS_CLEAR(UCSRA , TXC);
	   SET_BIT(UCSRA,TXC); // Clear the TXC flag
	 *************************************************/
	#endif
}
uint8 USART_recieveByte(void)
{
#if INTERRUPT
	/* if the data register is empty , recieve data from in UDR register */
	if(g_recieveComplete ==1)
	{
		g_recieveComplete=0;
		return UDR;
	}
#else
	/* *****************steps *********************
	 * Loop until the data are received
	 * then return the UDR which contain the data
	 * ******************************************** */
	while(BIT_IS_CLEAR(UCSRA , RXC));
	return UDR;
#endif
}

void USART_sendString(const uint8 *str)
{
	uint8 i=0;
	while(str[i] !='\0')
	{
		USART_sendByte(str[i]);
		i++;
	}
	/************************* Another Method *************************
	while(*str != '\0')
	{
		UART_sendByte(*str);
		str++;
	}
	*******************************************************************/
}
void USART_receiveString(uint8 *str) /* Receive until #*/
{
	/* in USART we send the string with # in last character of the string
	 * so we recieve the string until #and replace it with null '\0' */
	uint8 i=-1;
	do
	{
		i++;
		str[i]=USART_recieveByte();
	}while (str[i] !='#');

	/* add the null at last of string */
	str[i]='\0';
}

