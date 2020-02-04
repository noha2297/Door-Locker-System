 /******************************************************************************
 *
 * Module: External EEPROM
 *
 * File Name: external_eeprom.c
 *
 * Description: Source file for the External EEPROM Memory
 *
 * Author: Mohamed Tarek
 *
 *******************************************************************************/
#include "i2c.h"
#include "eeprom.h"

void EEPROM_init(void)
{
	/* just initialize the I2C(TWI) module inside the MC */
	TWI_init(1, 2 , TWI_1);
}

uint8 EEPROM_writeByte(uint16 u16addr, uint8 u8data)
{
	/* Send the Start Bit */
    TWI_start();
    if (TWI_getStatus() != TW_START)
        return ERROR;

    /* Send the device address, we need to get A8 A9 A10 address bits from the
     * memory location address and R/W=0 (write) */
    TWI_write((uint8)(0xA0 | ((u16addr & 0x0700)>>7)));
    if (TWI_getStatus() != TW_MT_SLA_W_ACK)
        return ERROR;

    /* Send the required memory location address */
    TWI_write((uint8)(u16addr));
    if (TWI_getStatus() != TW_MT_DATA_ACK)
        return ERROR;

    /* write byte to eeprom */
    TWI_write(u8data);
    if (TWI_getStatus() != TW_MT_DATA_ACK)
        return ERROR;

    /* Send the Stop Bit */
    TWI_stop();

    return SUCCESS;
}

uint8 EEPROM_readByte(uint16 u16addr, uint8 *u8data)
{
	/* Send the Start Bit */
    TWI_start();
    if (TWI_getStatus() != TW_START)
        return ERROR;

    /* Send the device address, we need to get A8 A9 A10 address bits from the
     * memory location address and R/W=0 (write) */
    TWI_write((uint8)((0xA0) | ((u16addr & 0x0700)>>7)));
    if (TWI_getStatus() != TW_MT_SLA_W_ACK)
        return ERROR;

    /* Send the required memory location address */
    TWI_write((uint8)(u16addr));
    if (TWI_getStatus() != TW_MT_DATA_ACK)
        return ERROR;

    /* Send the Repeated Start Bit */
    TWI_start();
    if (TWI_getStatus() != TW_REP_START)
        return ERROR;

    /* Send the device address, we need to get A8 A9 A10 address bits from the
     * memory location address and R/W=1 (Read) */
    TWI_write((uint8)((0xA0) | ((u16addr & 0x0700)>>7) | 1));
    if (TWI_getStatus() != TW_MT_SLA_R_ACK)
        return ERROR;

    /* Read Byte from Memory without send ACK */
    *u8data = TWI_readWithNACK();
    if (TWI_getStatus() != TW_MR_DATA_NACK)
        return ERROR;

    /* Send the Stop Bit */
    TWI_stop();
    return SUCCESS;
}

/* function that write Array of Charracter in Exterrnal EEPROM and return success of all bytes written or Error if any bytes not wrritten */

uint8 EEPROM_writeString ( uint16 address , uint8 * str)
{
	uint8 i=0, write , check;
	while (str[i] != '\0')
		{
			write = str[i];
			check=EEPROM_writeByte( address + i, write);
			if (check == ERROR)
				return ERROR;
			_delay_ms(50);
			i++;
		}
	return SUCCESS;
}

/* function that read Array of Charracter from Exterrnal EEPROM and return success of all bytes read or Error if any bytes not read */
uint8 EEPROM_readString(uint16 address , uint8 * str ,uint8 strSize)
{
	uint8 i , read , check;
	for (i = 0; i < strSize; i++)
	{
		check =EEPROM_readByte( address + i, &read);
		if (check == ERROR)
			return ERROR;
		 str[i] = read;
	}
	str[5] = '\0';
	return SUCCESS;
}
