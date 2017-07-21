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

#define COOGLEIOT_TIMEZONE_OFFSET ((3600 * 5) * -1) // Default Timezone is -5 UTC (America/New York)
#define COOGLEIOT_DAYLIGHT_OFFSET 0 // seconds

#define COOGLEIOT_NTP_SERVER_1 "pool.ntp.org"
#define COOGLEIOT_NTP_SERVER_2 "time.nist.gov"
#define COOGLEIOT_NTP_SERVER_3 "time.google.com"

#define COOGLEIOT_FIRMWARE_UPDATE_CHECK_MS 54000000  // 15 Minutes in Milliseconds

#include "Arduino.h"

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <PubSubClient.h>
#include <ESP8266httpUpdate.h>
#include <time.h>

#include "LUrlParser/LUrlParser.h"

#include "CoogleEEPROM.h"
#include "CoogleIOTWebserver.h"
#include "user_interface.h"

extern "C" void __coogle_iot_firmware_timer_callback(void *);

class CoogleIOTWebserver;

class CoogleIOT
{
    public:
		bool firmwareUpdateTick = false;
		bool _restarting = false;

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
        int getMQTTPort();
        String getFirmwareUpdateUrl();

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

        void checkForFirmwareUpdate();

    private:

        bool _serial;
        int _statusPin;
        HTTPUpdateResult firmwareUpdateStatus;
        time_t now;

        WiFiClient espClient;
        PubSubClient mqttClient;
        CoogleEEProm eeprom;
        CoogleIOTWebserver *webServer;
        os_timer_t firmwareUpdateTimer;
        
        bool mqttClientActive = false;


        void initializeLocalAP();
        void enableConfigurationMode();
        bool connectToSSID();
        bool initializeMQTT();
        bool connectToMQTT();
};

#endif
