

#include "i2c.h"


/*******************************************************************************
 *                       Interrupt Service Routines                            *
 *******************************************************************************/
#if INTERRUPT
volatile uint8 g_TWIFlag =0;
	ISR (TWI_vect)
	{
		g_TWIFlag=1;
	}
#endif



/*******************************************************************************
*                      Functions Definitions                                  *
 *******************************************************************************/
/*
 * Description : Function to initialize the I2C driver
 * 	1. Set the required  bit rate.
 * 	2. Set the required prescaler.
 * 	3. Set the required de.
 * 	4. Enable I2C.
 */

void TWI_init(uint8 a_deviceAddress  , uint8 a_bitRate , TWI_prescaler prescaler) //
{
	TWBR= a_bitRate;
	TWSR |= prescaler; // (TWSR & 0XFC) | (prescaler) prescaler
	TWCR =(1<<TWEN);
	TWAR = (a_deviceAddress <<TWA0 ); //((a_deviceAddress & 0X7f) <<TWA0)
#if INTERRUPT
	TWCR |=(1<<TWIE);
#endif
}

void TWI_start()
{

# if INTERRUPT
	if (g_TWIFlag ==1)
		TWCR =(1<<TWINT)|(1<<TWSTA)|(1<<TWEN);
#else
	TWCR =(1<<TWINT)|(1<<TWSTA)|(1<<TWEN);

	/* Wait for TWINT flag set in TWCR Register (start bit is send successfully) */
	while(BIT_IS_CLEAR(TWCR , TWINT));
#endif
}

void TWI_stop()
{

# if INTERRUPT
	if (g_TWIFlag ==1)
		TWCR =(1<<TWINT)|(1<<TWEN)|(1<<TWSTO);
#else

	/*
	 * Clear the TWINT flag before sending the stop bit TWINT=1
	 * send the stop bit by TWSTO=1
	 * Enable TWI Module TWEN=1
	 */
	TWCR =(1<<TWINT)|(1<<TWEN)|(1<<TWSTO);
#endif
}
void TWI_write(uint8 data)
{
	/* Put data On TWI data Register */
	TWDR= data;
	/*
	 * Clear the TWINT flag before sending the data TWINT=1
	 * Enable TWI Module TWEN=1
	 */
	TWCR =(1<<TWINT)|(1<<TWEN);
	/* Wait for TWINT flag set in TWCR Register(data is send successfully) */
	while(BIT_IS_CLEAR(TWCR , TWINT));
}

uint8 TWI_readWithACK()
{
	/*
	 * Clear the TWINT flag before reading the data TWINT=1
     * Enable sending ACK after reading or receiving data TWEA=1
     * Enable TWI Module TWEN=1
	 */
	TWCR =(1<<TWINT)|(1<<TWEN)|(1<<TWEA);
	/* Wait for TWINT flag set in TWCR Register (data received successfully) */
	while(BIT_IS_CLEAR(TWCR , TWINT));
	/* Read Data */
	return TWDR;
}
uint8 TWI_readWithNACK()
{

	/*
	 * Clear the TWINT flag before reading the data TWINT=1
	 * Enable TWI Module TWEN=1
	 */
	TWCR =(1<<TWINT)|(1<<TWEN);
	/* Wait for TWINT flag set in TWCR Register (data received successfully) */
	while(BIT_IS_CLEAR(TWCR , TWINT));
	 /* Read Data */
	return TWDR;
}

uint8 TWI_getStatus()
{
	/* masking to eliminate first 3 bits and get the last 5 bits (status bits) */
	uint8 status = (TWSR & 0XF8);
	return status;
}
