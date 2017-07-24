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
#include "CoogleIOTConfig.h"

#include "Arduino.h"

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <PubSubClient.h>
#include <ESP8266httpUpdate.h>
#include <time.h>

#include "DNSServer/DNSServer.h"
#include "LUrlParser/LUrlParser.h"

#include "CoogleEEPROM.h"
#include "CoogleIOTWebserver.h"

#include <user_interface.h>

station_status_t wifi_station_get_connect_status(void);
extern "C" void __coogle_iot_firmware_timer_callback(void *);

class CoogleIOTWebserver;

class CoogleIOT
{
    public:
		bool firmwareUpdateTick = false;
		bool _restarting = false;

        CoogleIOT(int);
        CoogleIOT();
        ~CoogleIOT();
        void loop();
        bool initialize();
        CoogleIOT& enableSerial(int);
        CoogleIOT& enableSerial();
        PubSubClient* getMQTTClient();
        bool serialEnabled();
        CoogleIOT& flashStatus(int);
        CoogleIOT& flashStatus(int, int);
        CoogleIOT& flashSOS();
        	CoogleIOT& resetEEProm();
        	void restartDevice();

        String getRemoteAPName();
        String getRemoteAPPassword();
        String getMQTTHostname();
        String getMQTTUsername();
        String getMQTTPassword();
        String getMQTTClientId();
        String getAPName();
        String getAPPassword();

        String filterAscii(String);
        int getMQTTPort();
        String getFirmwareUpdateUrl();
        String getWiFiStatus();

        bool verifyFlashConfiguration();

        CoogleIOT& setMQTTPort(int);
        CoogleIOT& setMQTTHostname(String);
        CoogleIOT& setMQTTUsername(String);
        CoogleIOT& setMQTTPassword(String);
        CoogleIOT& setRemoteAPName(String);
        CoogleIOT& setRemoteAPPassword(String);
        CoogleIOT& setMQTTClientId(String);
        CoogleIOT& setAPName(String);
        CoogleIOT& setAPPassword(String);
        CoogleIOT& setFirmwareUpdateUrl(String);
        CoogleIOT& syncNTPTime(int, int);

        bool mqttActive();
        bool dnsActive();
        bool ntpActive();
        bool firmwareClientActive();
        bool apStatus();

        void checkForFirmwareUpdate();

    private:

        bool _serial;
        int _statusPin;
        HTTPUpdateResult firmwareUpdateStatus;
        time_t now;

        DNSServer dnsServer;
        WiFiClient espClient;
        PubSubClient *mqttClient;
        CoogleEEProm eeprom;
        CoogleIOTWebserver *webServer;
        os_timer_t firmwareUpdateTimer;
        
        bool mqttClientActive = false;
        bool dnsServerActive = false;
        bool ntpClientActive = false;
        bool _firmwareClientActive = false;
        bool _apStatus = false;

        void initializeLocalAP();
        void enableConfigurationMode();
        bool connectToSSID();
        bool initializeMQTT();
        bool connectToMQTT();
};

#endif
