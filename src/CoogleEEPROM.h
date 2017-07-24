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

#ifndef COOGLE_EEPROM_H
#define COOGLE_EEPROM_H

#include <EEPROM.h>
#include "Arduino.h"
#include "CoogleIOTConfig.h"

class CoogleEEProm
{
	public:
		void initialize(size_t);
		void initialize();
		void reset();
		void fill(int, int, byte);
		void dump(int);
		bool validAddress(int);
		bool writeBytes(int, const byte *, int);
		bool readBytes(int, byte[], int);
		bool writeInt(int, int);
		bool readInt(int, int *);
		bool writeString(int address, String str);
		bool writeString(int, const char *);
		bool readString(int, char *, int);
		bool isApp(const byte *);
		bool setApp(const byte *);
};

#endif
