/*
  +----------------------------------------------------------------------+
  | CoogleIOT for ESP8266                                                |
  +----------------------------------------------------------------------+
  | Copyright (c) 2017 John Coggeshall                                   |
  +----------------------------------------------------------------------+
  | Licensed under the Apache License, Version 2.0 (the "License");      |
  | you may not use this file except in compliance with the License. You |
  | may obtain a copy of the License at:                                 |
  |                                                                      |
  | http://www.apache.org/licenses/LICENSE-2.0                           |
  |                                                                      |
  | Unless required by applicable law or agreed to in writing, software  |
  | distributed under the License is distributed on an "AS IS" BASIS,    |
  | WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or      |
  | implied. See the License for the specific language governing         |
  | permissions and limitations under the License.                       |
  +----------------------------------------------------------------------+
  | Authors: John Coggeshall <john@coggeshall.org>                       |
  +----------------------------------------------------------------------+
*/

#include "CoogleEEPROM.h"

void CoogleEEProm::initialize()
{
	EEPROM.begin(COOGLE_EEPROM_EEPROM_SIZE);
}

void CoogleEEProm::reset()
{
	return fill(0, COOGLE_EEPROM_EEPROM_SIZE, 0x0);
}

void CoogleEEProm::fill(int startAddress, int endAddress, byte b)
{
	for(int i = startAddress; i <= endAddress; i++) {
		EEPROM.write(i, b);
	}
	
	EEPROM.commit();
}

bool CoogleEEProm::isApp(const byte *magic)
{
	byte bytes[4];
	
	readBytes(0, bytes, 4);
	
	for(int i = 0; i < 4; i++) {
		if(bytes[i] != magic[i]) {
			return false;
		}
	}
	
	return true;
}

void CoogleEEProm::dump(int bytesPerRow = 16)
{
	int curAddr, curRow;
	byte b;
	
	char buf[10];
	
	if(!Serial) {
		return;
	}
	
	curRow = 0;
	
	for(int i = 0; i <= COOGLE_EEPROM_EEPROM_SIZE; i++) {
		
		if(curRow == 0) {
			sprintf(buf, "%03X", i);
			Serial.print(buf);
		}
		
		b = EEPROM.read(i);
		
		sprintf(buf, "%02X", b);
		
		curRow++;
		
		if(curRow == bytesPerRow) {
			curRow = 0;
			Serial.println(buf);
		} else {
			Serial.print(buf);
		}
	}
}

bool CoogleEEProm::validAddress(int address)
{
	return (address <= COOGLE_EEPROM_EEPROM_SIZE);
}

bool CoogleEEProm::writeBytes(int startAddress, const byte *array, int length)
{
	if(!validAddress(startAddress) || !validAddress(startAddress + length)) {
		return false;
	}
	
	for(int i = 0; i < length; i++) {
		EEPROM.write(startAddress + i, array[i]);
	}
	
	EEPROM.commit();
	
	return true;
}

bool CoogleEEProm::readBytes(int startAddress, byte array[], int length)
{
	if(!validAddress(startAddress) || !validAddress(startAddress + length)) {
		return false;
	}
	
	for(int i = 0; i < length; i++) {
		array[i] = EEPROM.read(startAddress + i);
	}
	
	return true;
}

bool CoogleEEProm::writeInt(int address, int value)
{
	byte *ptr;
	
	ptr = (byte *)&value;
	
	return writeBytes(address, ptr, sizeof(value));
}

bool CoogleEEProm::readInt(int address, int *value)
{
	return readBytes(address, (byte *)value, sizeof(int));
}


bool CoogleEEProm::writeString(int address, const char *string)
{
	int length;
	
	length = strlen(string) + 1;
	
	return writeBytes(address, (const byte *)string, length);
}

bool CoogleEEProm::readString(int startAddress, char *buffer, int bufSize)
{
	byte ch;
	int bufIdx;
	
	if(!validAddress(startAddress)) {
		return false;
	}
	
	if(bufSize == 0) {
		return false;
	}
	
	if(bufSize == 1) {
		buffer[0] = '\0';
		return true;
	}
	
	bufIdx = 0;
	
	do {

		buffer[bufIdx] = EEPROM.read(startAddress + bufIdx);
		bufIdx++;
	
	} while( 
		(ch != 0x00) && // Null hit
		(bufIdx < bufSize) && // Out of space
		((startAddress + bufIdx) <= COOGLE_EEPROM_EEPROM_SIZE) // End of EEPROM
	);
	
	if((ch != 0x00) && (bufIdx >= 1)) {
		buffer[bufIdx - 1] = '\0';
	}
	
	return true;
	
}
