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

#include "CoogleIOT.h"

CoogleIOT::CoogleIOT(int statusPin)
{
    _statusPin = statusPin;
    _serial = false;

     webServer = new CoogleIOTWebserver(this);
}

CoogleIOT::CoogleIOT()
{
	CoogleIOT(-1);
}

bool CoogleIOT::serialEnabled()
{
	return _serial;
}

void CoogleIOT::loop()
{
	if(!mqttClient.connected()) {
		if(!connectToMQTT()) {
			return;
		}
	}
	
	mqttClient.loop();
}

void CoogleIOT::flashSOS()
{
	for(int i = 0; i < 3; i++) {
		flashStatus(100, 3);
		flashStatus(500, 3);
		flashStatus(100, 3);
		delay(5000);
	}
}

void CoogleIOT::flashStatus(int speed)
{
	flashStatus(speed, 5);
}

void CoogleIOT::flashStatus(int speed, int repeat)
{
	if(_statusPin > -1) {
		for(int i = 0; i < repeat; i++) {
			digitalWrite(_statusPin, LOW);
			delay(speed);
			digitalWrite(_statusPin, HIGH);
			delay(speed);
		}
		
		digitalWrite(_statusPin, HIGH);
	}
	
}

bool CoogleIOT::initialize()
{
	
	if(_statusPin > -1) {
		pinMode(_statusPin, OUTPUT);
		flashStatus(COOGLEIOT_STATUS_INIT);
	}
	
	if(_serial) {
		Serial.println("Coogle IOT v" COOGLEIOT_VERSION " initializing..");
	}
	
	randomSeed(micros());
	
	if(!eeprom.isApp((const byte *)COOGLEIOT_MAGIC_BYTES)) {
		
		if(_serial) {
			Serial.println("EEPROM not initialized for platform, erasing..");
		}
		eeprom.reset();
	}
	
	initializeLocalAP();
	
	if(!connectToSSID()) {
		if(_serial) {
			Serial.println("Failed to connect to AP, cannot continue.");
		}
		return false;
	}
	
	if(!initializeMQTT()) {
		if(_serial) {
			Serial.println("Failed to connect to MQTT, cannot continue");
		}
		
		return false;
	}
	
	return true;
}

void CoogleIOT::initializeLocalAP()
{
	char localAPName[50];
	char localAPPassword[COOGLEIOT_AP_PASSWORD_MAXLEN];
	
	if(!eeprom.readString(COOGLEIOT_AP_PASSWORD_ADDR, localAPPassword, COOGLEIOT_AP_PASSWORD_MAXLEN)) {
		
		if(_serial) {
			Serial.println("Failed to read from EEPROM for AP Password");
		}
	}
	
	if(strcmp(localAPPassword, "") == 0) {
		if(_serial) {
			Serial.println("No AP Password found in memory");
			Serial.println("Setting to default password: " COOGLEIOT_AP_DEFAULT_PASSWORD);
		}
		
		strncpy(localAPPassword, COOGLEIOT_AP_DEFAULT_PASSWORD, COOGLEIOT_AP_PASSWORD_MAXLEN);

		if(!eeprom.writeString(COOGLEIOT_AP_PASSWORD_ADDR, localAPPassword)) {
			if(_serial) {
				Serial.println("Failed to write default password!");
			}
		}
		
	}

	sprintf(localAPName, COOGLEIOT_AP "%d", random(100000, 999999));
	
	if(_serial) {
		Serial.println("Initializing WiFi");
		Serial.print("Local AP Name: ");
		Serial.println(localAPName);
	}

	WiFi.mode(WIFI_AP_STA);
	WiFi.softAP(localAPName, localAPPassword);
	
	if(_serial) {
		Serial.print("Local IP Address: ");
		Serial.println(WiFi.softAPIP());
	}
	
}

bool CoogleIOT::initializeMQTT()
{
	flashStatus(COOGLEIOT_STATUS_MQTT_INIT);
	
	if(!eeprom.readString(COOGLEIOT_MQTT_HOST_ADDR, mqttHostname, COOGLEIOT_MQTT_HOST_MAXLEN)) {
		if(_serial) {
			Serial.println("Failed to read MQTT Server Hostname");
		}
	}
	
	if(strcmp(mqttHostname, "") == 0) {
		
		if(_serial) {
			Serial.println("No MQTT Hostname specified. Cannot continue");
			return false;
		}
	}
	
	if(!eeprom.readString(COOGLEIOT_MQTT_CLIENT_ID_ADDR, mqttClientId, COOGLEIOT_MQTT_CLIENT_ID_MAXLEN)) {
		if(_serial) {
			Serial.println("Failed to read MQTT Client ID. Setting to Default");
		}
		
		strncpy(mqttClientId, COOGLEIOT_DEFAULT_MQTT_CLIENT_ID, COOGLEIOT_MQTT_CLIENT_ID_MAXLEN);
		
		if(!eeprom.writeString(COOGLEIOT_MQTT_CLIENT_ID_ADDR, mqttClientId)) {
			if(_serial) {
				Serial.println("Failed to write MQTT default Client ID");
			}
		}
	}
	
	if(!eeprom.readString(COOGLEIOT_MQTT_USER_ADDR, mqttUsername, COOGLEIOT_MQTT_USER_MAXLEN)) {
		if(_serial) {
			Serial.println("Failed to read MQTT User");
		}
	}
	
	if(!eeprom.readString(COOGLEIOT_MQTT_USER_PASSWORD_ADDR, mqttPassword, COOGLEIOT_MQTT_USER_PASSWORD_MAXLEN)) {
		if(_serial) {
			Serial.println("Failed to read MQTT Password");
		}
	}
	
	if(!eeprom.readInt(COOGLEIOT_MQTT_PORT_ADDR, &mqttPort)) {
		if(_serial) {
			Serial.println("Failed to read MQTT Port");
		}
	}
	
	if(mqttPort == 0) {
		if(_serial) {
			Serial.println("Failed to read MQTT Port from memory. Setting to Default");
		}
		
		mqttPort = COOGLEIOT_DEFAULT_MQTT_PORT;
		
		if(!eeprom.writeInt(COOGLEIOT_MQTT_PORT_ADDR, mqttPort)) {
			if(_serial) {
				Serial.println("Failed to write MQTT Port to memory!");
			}
		}
	}
	
	mqttClient.setServer(mqttHostname, mqttPort);
	
	/**
	 * @todo Callback here
	 */
	
	return connectToMQTT();
}

PubSubClient CoogleIOT::getMQTTClient()
{
	return mqttClient;
}

bool CoogleIOT::connectToMQTT()
{
	bool connectResult;
	
	if(mqttClient.connected()) {
		return true;
	}
	
	if(_serial) {
		Serial.println("Attempting to connect to MQTT Server");
		Serial.print("Server: ");
		Serial.print(mqttHostname);
		Serial.print(":");
		Serial.println(mqttPort);
	}
	
	for(int i = 0; (i < 5) && (!mqttClient.connected()); i++) {
		
		if(strcmp(mqttUsername, "") == 0) {
			connectResult = mqttClient.connect(mqttClientId);
		} else {
			connectResult = mqttClient.connect(mqttClientId, mqttUsername, mqttPassword);
		}
		
		if(!connectResult) {
			if(_serial) {
				Serial.println("Could not connect to MQTT Server.. Retrying in 5 seconds..");
				Serial.print("State: ");
				Serial.println(mqttClient.state());
				delay(5000);
			}
		}
	}
	
	if(!mqttClient.connected()) {
		if(_serial) {
			Serial.println("Failed to connect to MQTT Server! Aborting.");
			return false;
		}
		
		flashSOS();
	}
	
	return true;
}

bool CoogleIOT::connectToSSID()
{
	char remoteAPName[COOGLEIOT_AP_NAME_MAXLEN];
	char remoteAPPassword[COOGLEIOT_REMOTE_AP_PASSWORD_MAXLEN];

	flashStatus(COOGLEIOT_STATUS_WIFI_INIT);
	
	if(!eeprom.readString(COOGLEIOT_AP_NAME_ADDR, remoteAPName, COOGLEIOT_AP_NAME_MAXLEN)) {
		if(_serial) {
			Serial.println("Failed to read Remote AP Name from EEPROM");
		}
	}
	
	if(strcmp(remoteAPName, "") == 0) {
	
		if(_serial) {
			Serial.println("No Remote AP Found in memory");
		}
		
		return true;
	} 
	
	if(_serial) {
		Serial.print("Connecting to AP: ");
		Serial.println(remoteAPName);
	}
	
	if(!eeprom.readString(COOGLEIOT_REMOTE_AP_PASSWORD_ADDR, remoteAPPassword, COOGLEIOT_REMOTE_AP_PASSWORD_MAXLEN)) {
		if(_serial) {
			Serial.println("Failed to read remote AP password from EEPROM");
		}
	}
	
	if(strcmp(remoteAPPassword, "") == 0) {
		if(_serial) {
			Serial.println("WARNING No Remote AP Password Set");
		}
		
		WiFi.begin(remoteAPName);
		
	} else {
		
		WiFi.begin(remoteAPName, remoteAPPassword);
		
	}
	
	for(int i = 0; (i < 20) && (WiFi.status() != WL_CONNECTED); i++) {
		delay(500);
		
		if(_serial) {
			Serial.print('.');
		}
		
	}
	
	if(WiFi.status() != WL_CONNECTED) {
		if(_serial) {
			Serial.println("ERROR: Could not connect to AP!");
			
		}
		
		flashSOS();
		
		return false;
	}
	
	if(_serial) {
		Serial.println("");
		Serial.println("Connected to Remote AP");
		Serial.print("Remote IP Address: ");
		Serial.println(WiFi.localIP());
	}
	
	return true;
}

void CoogleIOT::enableSerial()
{
	return enableSerial(15200);
}

void CoogleIOT::enableSerial(int baud)
{
    if(!Serial) {

      Serial.begin(baud);
  
      while(!Serial) {
          /* .... tic toc .... */
      }

    }

    _serial = true;
}

