/**	
 * |----------------------------------------------------------------------
 * | Copyright (c) 2016 Tilen Majerle
 * |  
 * | Permission is hereby granted, free of charge, to any person
 * | obtaining a copy of this software and associated documentation
 * | files (the "Software"), to deal in the Software without restriction,
 * | including without limitation the rights to use, copy, modify, merge,
 * | publish, distribute, sublicense, and/or sell copies of the Software, 
 * | and to permit persons to whom the Software is furnished to do so, 
 * | subject to the following conditions:
 * | 
 * | The above copyright notice and this permission notice shall be
 * | included in all copies or substantial portions of the Software.
 * | 
 * | THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * | EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * | OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
 * | AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * | HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * | WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
 * | FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * | OTHER DEALINGS IN THE SOFTWARE.
 * |----------------------------------------------------------------------
 */
#include "tm_stm32_ds18s20.h"

uint8_t TM_DS18S20_Start(TM_OneWire_t* OneWire, uint8_t *ROM) {
	/* Check if device is DS18S20 */
	if (!TM_DS18S20_Is(ROM)) {
		return 0;
	}
	
	/* Reset line */
	TM_OneWire_Reset(OneWire);
	/* Select ROM number */
	TM_OneWire_SelectWithPointer(OneWire, ROM);
	/* Start temperature conversion */
	TM_OneWire_WriteByte(OneWire, DS18S20_CMD_CONVERTTEMP);
	
	return 1;
}

void TM_DS18S20_StartAll(TM_OneWire_t* OneWire) {
	/* Reset pulse */
	TM_OneWire_Reset(OneWire);
	/* Skip rom */
	TM_OneWire_WriteByte(OneWire, ONEWIRE_CMD_SKIPROM);
	/* Start conversion on all connected devices */
	TM_OneWire_WriteByte(OneWire, DS18S20_CMD_CONVERTTEMP);
}

uint8_t TM_DS18S20_Read(TM_OneWire_t* OneWire, uint8_t *ROM, uint8_t *temp) {
	float temperature;
//	uint8_t minus = 0;
	uint8_t i = 0;
	uint8_t data[9];
	uint8_t crc;

	/* Check if device is DS18S20 */
	if (!TM_DS18S20_Is(ROM)) {
		return 0;
	}

	/* Check if line is released, if it is, then conversion is complete */
	if (!TM_OneWire_ReadBit(OneWire)) {
		/* Conversion is not finished yet */
		return 0;
	}

	/* Reset line */
	TM_OneWire_Reset(OneWire);
	/* Select ROM number */
	TM_OneWire_SelectWithPointer(OneWire, ROM);
	/* Read scratchpad command by onewire protocol */
	TM_OneWire_WriteByte(OneWire, ONEWIRE_CMD_RSCRATCHPAD);
	/* Get data */
	for (i = 0; i < 9; i++) {
		/* Read byte by byte */
		data[i] = TM_OneWire_ReadByte(OneWire);
	}
//	return data[0];
	/* Calculate CRC */
	crc = TM_OneWire_CRC8(data, 8);

	/* Check if CRC is ok */
	if (crc != data[8]) {
		/* CRC invalid */
		return 0;
	}

//	temperature = (data[0])/2;
	temperature = ((data[0] - 0.25 +((data[7] - data[6])/data[7])))/2;

	/* Set to pointer */
	*temp = (int)temperature;

	/* Return 1, temperature valid */
	return 1;
}

uint8_t TM_DS18S20_Is(uint8_t *ROM) {
	/* Checks if first byte is equal to DS18S20's family code */
	if (*ROM == DS18S20_FAMILY_CODE) {
		return 1;
	}
	return 0;
}

uint8_t TM_DS18S20_SetAlarmLowTemperature(TM_OneWire_t* OneWire, uint8_t *ROM, int8_t temp) {
	uint8_t tl, th, conf;
	if (!TM_DS18S20_Is(ROM)) {
		return 0;
	}
	if (temp > 125) {
		temp = 125;
	} 
	if (temp < -55) {
		temp = -55;
	}
	/* Reset line */
	TM_OneWire_Reset(OneWire);
	/* Select ROM number */
	TM_OneWire_SelectWithPointer(OneWire, ROM);
	/* Read scratchpad command by onewire protocol */
	TM_OneWire_WriteByte(OneWire, ONEWIRE_CMD_RSCRATCHPAD);
	
	/* Ignore first 2 bytes */
	TM_OneWire_ReadByte(OneWire);
	TM_OneWire_ReadByte(OneWire);
	
	th = TM_OneWire_ReadByte(OneWire);
	tl = TM_OneWire_ReadByte(OneWire);
	conf = TM_OneWire_ReadByte(OneWire);
	
	tl = (uint8_t)temp; 

	/* Reset line */
	TM_OneWire_Reset(OneWire);
	/* Select ROM number */
	TM_OneWire_SelectWithPointer(OneWire, ROM);
	/* Write scratchpad command by onewire protocol, only th, tl and conf register can be written */
	TM_OneWire_WriteByte(OneWire, ONEWIRE_CMD_WSCRATCHPAD);
	
	/* Write bytes */
	TM_OneWire_WriteByte(OneWire, th);
	TM_OneWire_WriteByte(OneWire, tl);
	TM_OneWire_WriteByte(OneWire, conf);
	
	/* Reset line */
	TM_OneWire_Reset(OneWire);
	/* Select ROM number */
	TM_OneWire_SelectWithPointer(OneWire, ROM);
	/* Copy scratchpad to EEPROM of DS18S20 */
	TM_OneWire_WriteByte(OneWire, ONEWIRE_CMD_CPYSCRATCHPAD);
	
	return 1;
}

uint8_t TM_DS18S20_SetAlarmHighTemperature(TM_OneWire_t* OneWire, uint8_t *ROM, int8_t temp) {
	uint8_t tl, th, conf;
	if (!TM_DS18S20_Is(ROM)) {
		return 0;
	}
	if (temp > 125) {
		temp = 125;
	} 
	if (temp < -55) {
		temp = -55;
	}
	/* Reset line */
	TM_OneWire_Reset(OneWire);
	/* Select ROM number */
	TM_OneWire_SelectWithPointer(OneWire, ROM);
	/* Read scratchpad command by onewire protocol */
	TM_OneWire_WriteByte(OneWire, ONEWIRE_CMD_RSCRATCHPAD);
	
	/* Ignore first 2 bytes */
	TM_OneWire_ReadByte(OneWire);
	TM_OneWire_ReadByte(OneWire);
	
	th = TM_OneWire_ReadByte(OneWire);
	tl = TM_OneWire_ReadByte(OneWire);
	conf = TM_OneWire_ReadByte(OneWire);
	
	th = (uint8_t)temp; 

	/* Reset line */
	TM_OneWire_Reset(OneWire);
	/* Select ROM number */
	TM_OneWire_SelectWithPointer(OneWire, ROM);
	/* Write scratchpad command by onewire protocol, only th, tl and conf register can be written */
	TM_OneWire_WriteByte(OneWire, ONEWIRE_CMD_WSCRATCHPAD);
	
	/* Write bytes */
	TM_OneWire_WriteByte(OneWire, th);
	TM_OneWire_WriteByte(OneWire, tl);
	TM_OneWire_WriteByte(OneWire, conf);
	
	/* Reset line */
	TM_OneWire_Reset(OneWire);
	/* Select ROM number */
	TM_OneWire_SelectWithPointer(OneWire, ROM);
	/* Copy scratchpad to EEPROM of DS18S20 */
	TM_OneWire_WriteByte(OneWire, ONEWIRE_CMD_CPYSCRATCHPAD);
	
	return 1;
}

uint8_t TM_DS18S20_DisableAlarmTemperature(TM_OneWire_t* OneWire, uint8_t *ROM) {
	uint8_t tl, th, conf;
	if (!TM_DS18S20_Is(ROM)) {
		return 0;
	}
	/* Reset line */
	TM_OneWire_Reset(OneWire);
	/* Select ROM number */
	TM_OneWire_SelectWithPointer(OneWire, ROM);
	/* Read scratchpad command by onewire protocol */
	TM_OneWire_WriteByte(OneWire, ONEWIRE_CMD_RSCRATCHPAD);
	
	/* Ignore first 2 bytes */
	TM_OneWire_ReadByte(OneWire);
	TM_OneWire_ReadByte(OneWire);
	
	th = TM_OneWire_ReadByte(OneWire);
	tl = TM_OneWire_ReadByte(OneWire);
	conf = TM_OneWire_ReadByte(OneWire);
	
	th = 125;
	tl = (uint8_t)-55;

	/* Reset line */
	TM_OneWire_Reset(OneWire);
	/* Select ROM number */
	TM_OneWire_SelectWithPointer(OneWire, ROM);
	/* Write scratchpad command by onewire protocol, only th, tl and conf register can be written */
	TM_OneWire_WriteByte(OneWire, ONEWIRE_CMD_WSCRATCHPAD);
	
	/* Write bytes */
	TM_OneWire_WriteByte(OneWire, th);
	TM_OneWire_WriteByte(OneWire, tl);
	TM_OneWire_WriteByte(OneWire, conf);
	
	/* Reset line */
	TM_OneWire_Reset(OneWire);
	/* Select ROM number */
	TM_OneWire_SelectWithPointer(OneWire, ROM);
	/* Copy scratchpad to EEPROM of DS18S20 */
	TM_OneWire_WriteByte(OneWire, ONEWIRE_CMD_CPYSCRATCHPAD);
	
	return 1;
}

uint8_t TM_DS18S20_AlarmSearch(TM_OneWire_t* OneWire) {
	/* Start alarm search */
	return TM_OneWire_Search(OneWire, DS18S20_CMD_ALARMSEARCH);
}

uint8_t TM_DS18S20_AllDone(TM_OneWire_t* OneWire) {
	/* If read bit is low, then device is not finished yet with calculation temperature */
	return TM_OneWire_ReadBit(OneWire);
}


