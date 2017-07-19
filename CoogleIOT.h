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

#ifndef COOGLEIOT_H
#define COOGLEIOT_H

#include "EEPROM_map.h"

#define COOGLEIOT_VERSION "0.1b"

#define COOGLEIOT_STATUS_INIT 500
#define COOGLEIOT_STATUS_WIFI_INIT 250
#define COOGLEIOT_STATUS_MQTT_INIT 100

#define COOGLEIOT_MAGIC_BYTES "ciot"
#define COOGLEIOT_AP "COOGLEIOT_"
#define COOGLEIOT_AP_DEFAULT_PASSWORD "coogleiot"
#define COOGLEIOT_DEFAULT_MQTT_CLIENT_ID "coogleIoT"

#define COOGLEIOT_DEFAULT_MQTT_PORT 1883

#include "Arduino.h"

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <PubSubClient.h>

#include "CoogleEEPROM.h"
#include "CoogleIOTWebserver.h"

class CoogleIOTWebserver;

class CoogleIOT
{
    public:
        CoogleIOT(int);
        CoogleIOT();
        void loop();
        bool initialize();
        CoogleIOT& enableSerial(int);
        CoogleIOT& enableSerial();
        PubSubClient getMQTTClient();
        bool serialEnabled();
        CoogleIOT& flashStatus(int);
        CoogleIOT& flashStatus(int, int);
        CoogleIOT& flashSOS();
        std::string replace_all(std::string, const std::string&, const std::string&);
    private:
        bool _serial;
        int _statusPin;

		char mqttHostname[COOGLEIOT_MQTT_HOST_MAXLEN];
		char mqttUsername[COOGLEIOT_MQTT_USER_MAXLEN];
		char mqttPassword[COOGLEIOT_MQTT_USER_PASSWORD_MAXLEN];
		char mqttClientId[COOGLEIOT_MQTT_CLIENT_ID_MAXLEN];
		int  mqttPort;
    	
        WiFiClient espClient;
        PubSubClient mqttClient;
        CoogleEEProm eeprom;
        CoogleIOTWebserver *webServer;
        
        bool mqttClientActive = false;

        void initializeLocalAP();
        void enableConfigurationMode();
        bool connectToSSID();
        bool initializeMQTT();
        bool connectToMQTT();
};

#endif
