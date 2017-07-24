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
#include "CoogleIOTConfig.h"

CoogleIOT* __coogle_iot_self;

extern "C" void __coogle_iot_firmware_timer_callback(void *pArg)
{
	__coogle_iot_self->firmwareUpdateTick = true;
}

CoogleIOT::CoogleIOT(int statusPin)
{
    _statusPin = statusPin;
    _serial = false;

}

CoogleIOT::CoogleIOT()
{
	CoogleIOT(-1);
}

CoogleIOT::~CoogleIOT()
{
	delete mqttClient;
	delete webServer;
}

bool CoogleIOT::serialEnabled()
{
	return _serial;
}

void CoogleIOT::loop()
{
	struct tm* p_tm;

	if(mqttClientActive) {
		if(!mqttClient->connected()) {
			yield();
			if(!connectToMQTT()) {
				flashSOS();
			}
		}

		yield();
		mqttClient->loop();
	}
	
	yield();
	webServer->loop();

	yield();
	dnsServer.processNextRequest();

	now = time(nullptr);

	if(now) {
		p_tm = localtime(&now);

		if( (p_tm->tm_hour == 12) &&
			(p_tm->tm_min == 0) &&
			(p_tm->tm_sec == 6)) {
			yield();
			syncNTPTime(COOGLEIOT_TIMEZONE_OFFSET, COOGLEIOT_DAYLIGHT_OFFSET);
		}
	}

	if(firmwareUpdateTick) {
		firmwareUpdateTick = false;

		checkForFirmwareUpdate();

		if(_serial) {
			switch(firmwareUpdateStatus) {
				case HTTP_UPDATE_FAILED:
					Serial.println("Warning! Failed to update firmware with specified URL");
					break;
				case HTTP_UPDATE_NO_UPDATES:
					Serial.println("Firmware update check completed - at current version");
					break;
				case HTTP_UPDATE_OK:
					Serial.println("Firmware updated!");
					break;
				default:
					Serial.println("Warning! No updated performed. Perhaps an invalid URL?");
					break;
			}
		}
	}
}

CoogleIOT& CoogleIOT::flashSOS()
{
	for(int i = 0; i < 3; i++) {
		flashStatus(200, 3);
		delay(1000);
		flashStatus(500, 3);
		delay(1000);
		flashStatus(200, 3);
		delay(5000);
	}
	
	return *this;
}

bool CoogleIOT::mqttActive()
{
	return mqttClientActive;
}

bool CoogleIOT::dnsActive()
{
	return dnsServerActive;
}

bool CoogleIOT::ntpActive()
{
	return ntpClientActive;
}

bool CoogleIOT::firmwareClientActive()
{
	return _firmwareClientActive;
}

bool CoogleIOT::apStatus()
{
	return _apStatus;
}

String CoogleIOT::getWiFiStatus()
{

	String retval;

    switch(WiFi.status()) {
        case WL_CONNECTED:
            retval = "Connected";
            break;
        case WL_NO_SSID_AVAIL:
            retval = "No SSID Available";
            break;
        case WL_CONNECT_FAILED:
            retval = "Failed to Connect";
            break;
        case WL_IDLE_STATUS:
            retval = "Idle";
            break;
        case WL_DISCONNECTED:
        default:
            retval = "Disconnected";
            break;
    }

    return retval;
}

CoogleIOT& CoogleIOT::syncNTPTime(int offsetSeconds, int daylightOffsetSec)
{
	if(!WiFi.status() == WL_CONNECTED) {

		if(_serial) {
			Serial.println("Cannot synchronize time with NTP Servers - No WiFi Connection");
		}

		return *this;
	}

	if(_serial) {
		Serial.println("Synchronizing time on device with NTP Servers");
		Serial.printf("Offset From UTC is %d seconds, daylight offset is %d seconds\n", offsetSeconds, daylightOffsetSec);
	}

	configTime(offsetSeconds, daylightOffsetSec, COOGLEIOT_NTP_SERVER_1, COOGLEIOT_NTP_SERVER_2, COOGLEIOT_NTP_SERVER_3);

	for(int i = 0; (i < 10) && !time(nullptr); i++) {
		delay(1000);

		if(_serial) {
			Serial.print(".");
		}
	}

	if(_serial) {
		Serial.println();
	}

	if(!(now = time(nullptr))) {
		if(_serial) {
			Serial.println("WARNING: Failed to synchronize time with time server!");
		}
	} else {
		if(_serial) {
			Serial.println("Time successfully synchronized with NTP server");
		}

		ntpClientActive = true;
	}

	return *this;
}

CoogleIOT& CoogleIOT::flashStatus(int speed)
{
	flashStatus(speed, 5);
	return *this;
}

CoogleIOT& CoogleIOT::flashStatus(int speed, int repeat)
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
	
	return *this;
}

bool CoogleIOT::initialize()
{
	String firmwareUrl;
	
	if(_statusPin > -1) {
		pinMode(_statusPin, OUTPUT);
		flashStatus(COOGLEIOT_STATUS_INIT);
	}
	
	if(_serial) {
		Serial.println("Coogle IOT v" COOGLEIOT_VERSION " initializing..");
	}
	
	verifyFlashConfiguration();

	randomSeed(micros());
	
	eeprom.initialize(COOGLE_EEPROM_EEPROM_SIZE);

	if(!eeprom.isApp((const byte *)COOGLEIOT_MAGIC_BYTES)) {
		
		if(_serial) {
			Serial.println("EEPROM not initialized for platform, erasing..");
		}
		eeprom.reset();
		eeprom.setApp((const byte *)COOGLEIOT_MAGIC_BYTES);
	}
	
	WiFi.disconnect();
	WiFi.setAutoConnect(false);
	WiFi.setAutoReconnect(true);
	WiFi.mode(WIFI_AP_STA);

	if(!connectToSSID()) {
		if(_serial) {
			Serial.println("Failed to connect to AP");
		}
	} else {
	
		syncNTPTime(COOGLEIOT_TIMEZONE_OFFSET, COOGLEIOT_DAYLIGHT_OFFSET);

		if(!initializeMQTT()) {
			if(_serial) {
				Serial.println("Failed to connect to MQTT");
			}
		}

	}

	// Used for callbacks into C, where we can't use std::bind to bind an object method
	__coogle_iot_self = this;

	enableConfigurationMode();

	firmwareUrl = getFirmwareUpdateUrl();

	if(firmwareUrl.length() > 0) {
		os_timer_setfn(&firmwareUpdateTimer, __coogle_iot_firmware_timer_callback, NULL);
		os_timer_arm(&firmwareUpdateTimer, COOGLEIOT_FIRMWARE_UPDATE_CHECK_MS, true);

		if(_serial) {
			Serial.printf("Firmware Automatic Update Will Occur every %d Milliseconds\n", COOGLEIOT_FIRMWARE_UPDATE_CHECK_MS);
		}

		_firmwareClientActive = true;
	}

	return true;
}

void CoogleIOT::restartDevice()
{
	_restarting = true;
	ESP.restart();
}

CoogleIOT& CoogleIOT::resetEEProm()
{
	eeprom.reset();
	return *this;
}

bool CoogleIOT::verifyFlashConfiguration()
{
	uint32_t realSize = ESP.getFlashChipRealSize();
	uint32_t ideSize = ESP.getFlashChipSize();
	FlashMode_t ideMode = ESP.getFlashChipMode();

	if(_serial) {
		Serial.println("Introspecting on-board Flash Memory:");
		Serial.printf("Flash ID: %08X\n", ESP.getFlashChipId());
		Serial.printf("Flash real size: %u\n", realSize);
		Serial.printf("Flash IDE Size: %u\n", ideSize);
		Serial.printf("Flash IDE Speed: %u\n", ESP.getFlashChipSpeed());
		Serial.printf("Flash IDE Mode: %u\n", (ideMode == FM_QIO ? "QIO" : ideMode == FM_QOUT ? "QOUT" : ideMode == FM_DIO ? "DIO" : ideMode == FM_DOUT ? "DOUT" : "UNKNOWN"));
	}

	if(ideSize != realSize) {
		if(_serial) {
			Serial.println("\n\n****** WARNING: Flashed Size is not equal to size available on chip! ******\n\n");
		}
	} else {
		if(_serial) {
			Serial.println("Flash Chip Configuration Verified: OK");
		}
	}

}

void CoogleIOT::enableConfigurationMode()
{
	if(_serial) {
		Serial.println("Enabling Configuration Mode");
	}

	initializeLocalAP();

	if(_serial) {
		Serial.println("CIOT: Creating Webserver");
	}

	webServer = new CoogleIOTWebserver(*this);

	if(!webServer->initialize()) {
		if(_serial) {
			Serial.println("Failed to initialize Web Server");
		}

		flashSOS();
	}
}


void CoogleIOT::initializeLocalAP()
{
	String localAPName, localAPPassword;

	IPAddress apLocalIP(192,168,0,1);
	IPAddress apSubnetMask(255,255,255,0);
	IPAddress apGateway(192,168,0,1);
	
	localAPName = getAPName();
	localAPPassword = getAPPassword();
	
	if(localAPPassword.length() == 0) {
		if(_serial) {
			Serial.println("No AP Password found in memory");
			Serial.println("Setting to default password: " COOGLEIOT_AP_DEFAULT_PASSWORD);
		}

		localAPPassword = COOGLEIOT_AP_DEFAULT_PASSWORD;
		setAPPassword(localAPPassword);

	}

	if(localAPName.length() == 0) {
		if(_serial) {
			Serial.println("No AP Name found in memory. Auto-generating AP name");
		}

		localAPName = COOGLEIOT_AP;
		localAPName.concat((int)random(100000, 999999));

		if(_serial) {
			Serial.print("Setting AP Name To: ");
			Serial.println(localAPName);
		}

		setAPName(localAPName);

		localAPName = getAPName();

		Serial.print("AP Name is: ");
		Serial.println(localAPName);
	}
	
	if(_serial) {
		Serial.println("Initializing WiFi");
		Serial.print("Local AP Name: ");
		Serial.println(localAPName);
	}

	WiFi.softAPConfig(apLocalIP, apGateway, apSubnetMask);
	WiFi.softAP(localAPName.c_str(), localAPPassword.c_str());
	
	if(_serial) {
		Serial.print("Local IP Address: ");
		Serial.println(WiFi.softAPIP());
	}
	
	if(_serial) {
		Serial.println("Initializing DNS Server");
	}

	if(WiFi.status() != WL_CONNECTED) {
		dnsServer.start(COOGLEIOT_DNS_PORT, "*", WiFi.softAPIP());

		if(_serial) {
			Serial.println("Enabled DNS Server, not connected to WiFi");
		}

		dnsServerActive = true;

	} else {

		if(_serial) {
			Serial.println("Disabled DNS Server while connected to WiFi");
		}

		dnsServerActive = false;

	}

	_apStatus = true;

}

String CoogleIOT::getFirmwareUpdateUrl()
{
	char firmwareUrl[COOGLEIOT_FIRMWARE_UPDATE_URL_MAXLEN];

	if(!eeprom.readString(COOGLEIOT_FIRMWARE_UPDATE_URL_ADDR, firmwareUrl, COOGLEIOT_FIRMWARE_UPDATE_URL_MAXLEN)) {
		if(_serial) {
			Serial.println("Failed to read Firmware Update URL");
		}
	}

	String retval(firmwareUrl);
	return filterAscii(retval);
}

String CoogleIOT::getMQTTHostname()
{
	char mqttHost[COOGLEIOT_MQTT_HOST_MAXLEN];

	if(!eeprom.readString(COOGLEIOT_MQTT_HOST_ADDR, mqttHost, COOGLEIOT_MQTT_HOST_MAXLEN)) {
		if(_serial) {
			Serial.println("Failed to read MQTT Server Hostname");
		}
	}

	String retval(mqttHost);
	return filterAscii(retval);
}

String CoogleIOT::getMQTTClientId()
{
	char mqtt[COOGLEIOT_MQTT_CLIENT_ID_MAXLEN];

	if(!eeprom.readString(COOGLEIOT_MQTT_CLIENT_ID_ADDR, mqtt, COOGLEIOT_MQTT_CLIENT_ID_MAXLEN)) {
		if(_serial) {
			Serial.println("Failed to read MQTT Client ID");
		}
	}

	String retval(mqtt);
	return filterAscii(retval);
}

String CoogleIOT::getMQTTUsername()
{
	char mqtt[COOGLEIOT_MQTT_USER_MAXLEN];

	if(!eeprom.readString(COOGLEIOT_MQTT_USER_ADDR, mqtt, COOGLEIOT_MQTT_USER_MAXLEN)) {
		if(_serial) {
			Serial.println("Failed to read MQTT Username");
		}
	}

	String retval(mqtt);
	return filterAscii(retval);
}

String CoogleIOT::getMQTTPassword()
{
	char mqtt[COOGLEIOT_MQTT_USER_PASSWORD_MAXLEN];

	if(!eeprom.readString(COOGLEIOT_MQTT_USER_PASSWORD_ADDR, mqtt, COOGLEIOT_MQTT_USER_PASSWORD_MAXLEN)) {
		if(_serial) {
			Serial.println("Failed to read MQTT Password");
		}
	}

	String retval(mqtt);
	return filterAscii(retval);
}

int CoogleIOT::getMQTTPort()
{
	int mqtt;

	if(!eeprom.readInt(COOGLEIOT_MQTT_PORT_ADDR, &mqtt)) {
		if(_serial) {
			Serial.println("Failed to read MQTT Port");
		}
	}

	return mqtt;
}

CoogleIOT& CoogleIOT::setMQTTPort(int port)
{
	if(!eeprom.writeInt(COOGLEIOT_MQTT_PORT_ADDR, port)) {
		if(_serial) {
			Serial.println("Failed to write MQTT Port to memory!");
		}
	}

	return *this;
}

CoogleIOT& CoogleIOT::setFirmwareUpdateUrl(String s)
{
	if(s.length() > COOGLEIOT_FIRMWARE_UPDATE_URL_MAXLEN) {
		if(_serial) {
			Serial.println("Attempted to write beyond max length!");
		}
		return *this;
	}

	if(!eeprom.writeString(COOGLEIOT_FIRMWARE_UPDATE_URL_ADDR, s)) {
		if(_serial) {
			Serial.println("Failed to write Firmware Update URL");
		}
	}

	return *this;
}

CoogleIOT& CoogleIOT::setMQTTClientId(String s)
{
	if(s.length() > COOGLEIOT_MQTT_CLIENT_ID_MAXLEN) {
		if(_serial) {
			Serial.println("Attempted to write beyond max length!");
		}
		return *this;
	}

	if(!eeprom.writeString(COOGLEIOT_MQTT_CLIENT_ID_ADDR, s)) {
		if(_serial) {
			Serial.println("Failed to write MQTT default Client ID");
		}
	}

	return *this;
}

CoogleIOT& CoogleIOT::setMQTTHostname(String s)
{
	if(s.length() > COOGLEIOT_MQTT_HOST_MAXLEN) {
		if(_serial) {
			Serial.println("Attempted to write beyond max length!");
		}
		return *this;
	}

	if(!eeprom.writeString(COOGLEIOT_MQTT_HOST_ADDR, s)) {
			if(_serial) {
				Serial.println("Failed to write MQTT default Client ID");
			}
		}

	return *this;
}

CoogleIOT& CoogleIOT::setMQTTUsername(String s)
{
	if(s.length() > COOGLEIOT_MQTT_USER_MAXLEN) {
		if(_serial) {
			Serial.println("Attempted to write beyond max length!");
		}
		return *this;
	}

	if(!eeprom.writeString(COOGLEIOT_MQTT_USER_ADDR, s)) {
			if(_serial) {
				Serial.println("Failed to write MQTT default Client ID");
			}
		}

	return *this;
}

CoogleIOT& CoogleIOT::setMQTTPassword(String s)
{
	if(s.length() > COOGLEIOT_MQTT_USER_PASSWORD_MAXLEN) {
		if(_serial) {
			Serial.println("Attempted to write beyond max length!");
		}
		return *this;
	}

	if(!eeprom.writeString(COOGLEIOT_MQTT_USER_PASSWORD_ADDR, s)) {
			if(_serial) {
				Serial.println("Failed to write MQTT default Client ID");
			}
		}

	return *this;
}

CoogleIOT& CoogleIOT::setRemoteAPName(String s)
{
	if(s.length() > COOGLEIOT_REMOTE_AP_NAME_MAXLEN) {
		if(_serial) {
			Serial.println("Attempted to write beyond max length!");
		}
		return *this;
	}

	if(!eeprom.writeString(COOGLEIOT_REMOTE_AP_NAME_ADDR, s)) {
		if(_serial) {
			Serial.println("Failed to write MQTT default Client ID");
		}
	}

	return *this;
}

CoogleIOT& CoogleIOT::setRemoteAPPassword(String s)
{
	if(s.length() > COOGLEIOT_REMOTE_AP_PASSWORD_MAXLEN) {
		if(_serial) {
			Serial.println("Attempted to write beyond max length!");
		}
		return *this;
	}

	if(!eeprom.writeString(COOGLEIOT_REMOTE_AP_PASSWORD_ADDR, s)) {
			if(_serial) {
				Serial.println("Failed to write MQTT default Client ID");
			}
		}

	return *this;
}

CoogleIOT& CoogleIOT::setAPName(String s)
{
	if(s.length() > COOGLEIOT_AP_NAME_MAXLEN) {
		if(_serial) {
			Serial.println("Attempted to write beyond max length!");
		}
		return *this;
	}

	if(!eeprom.writeString(COOGLEIOT_AP_NAME_ADDR, s)) {
			if(_serial) {
				Serial.println("Failed to write MQTT default Client ID");
			}
		}

	return *this;
}

void CoogleIOT::checkForFirmwareUpdate()
{
	String firmwareUrl;
	LUrlParser::clParseURL URL;
	int port;

	firmwareUrl = getFirmwareUpdateUrl();

	if(firmwareUrl.length() == 0) {
		return;
	}

	if(_serial) {
		Serial.println("Checking for Firmware Updates");
	}

	os_intr_lock();

	URL = LUrlParser::clParseURL::ParseURL(firmwareUrl.c_str());

	if(!URL.IsValid()) {
		os_intr_unlock();
		return;
	}

	if(!URL.GetPort(&port)) {
		port = 80;
	}

	firmwareUpdateStatus = ESPhttpUpdate.update(URL.m_Host.c_str(), port, URL.m_Path.c_str(), COOGLEIOT_VERSION);

	os_intr_unlock();
}

CoogleIOT& CoogleIOT::setAPPassword(String s)
{
	if(s.length() > COOGLEIOT_AP_PASSWORD_MAXLEN) {
		if(_serial) {
			Serial.println("Attempted to write beyond max length!");
		}
		return *this;
	}

	if(!eeprom.writeString(COOGLEIOT_AP_PASSWORD_ADDR, s)) {
			if(_serial) {
				Serial.println("Failed to write AP Password");
			}
		}

	return *this;
}

bool CoogleIOT::initializeMQTT()
{
	String mqttHostname, mqttClientId, mqttUsername, mqttPassword;
	int mqttPort;

	flashStatus(COOGLEIOT_STATUS_MQTT_INIT);

	mqttHostname = getMQTTHostname();

	if(mqttHostname.length() == 0) {
		
		if(_serial) {
			Serial.println("No MQTT Hostname specified. Cannot continue");
			mqttClientActive = false;
			return false;
		}
	}
	
	mqttClientId = getMQTTClientId();

	if(mqttClientId.length() == 0) {
		if(_serial) {
			Serial.println("Failed to read MQTT Client ID. Setting to Default");
		}
		
		mqttClientId = COOGLEIOT_DEFAULT_MQTT_CLIENT_ID;
		
		setMQTTClientId(mqttClientId);
	}
	
	mqttPort = getMQTTPort();
	
	if(mqttPort == 0) {
		if(_serial) {
			Serial.println("Failed to read MQTT Port from memory. Setting to Default");
		}
		
		setMQTTPort(COOGLEIOT_DEFAULT_MQTT_PORT);
	}
	
	mqttClient = new PubSubClient(espClient);
	mqttClient->setServer(mqttHostname.c_str(), mqttPort);
	
	/**
	 * @todo Callback here
	 */
	
	return connectToMQTT();
}

PubSubClient* CoogleIOT::getMQTTClient()
{
	return mqttClient;
}

bool CoogleIOT::connectToMQTT()
{
	bool connectResult;
	String mqttHostname, mqttUsername, mqttPassword, mqttClientId;
	int mqttPort;

	if(mqttClient->connected()) {
		mqttClientActive = true;
		return true;
	}

	if(WiFi.status() != WL_CONNECTED) {
		if(_serial) {
			Serial.println("Cannot connect to MQTT because we are not connected to WiFi");
			mqttClientActive = false;
			return false;
		}
	}

	mqttHostname = getMQTTHostname();
	mqttUsername = getMQTTUsername();
	mqttPassword = getMQTTPassword();
	mqttPort = getMQTTPort();
	mqttClientId = getMQTTClientId();

	if(mqttHostname.length() == 0) {
		mqttClientActive = false;
		return false;
	}

	if(_serial) {
		Serial.println("Attempting to connect to MQTT Server");
		Serial.print("Server: ");
		Serial.print(mqttHostname);
		Serial.print(":");
		Serial.println(mqttPort);
	}
	
	for(int i = 0; (i < 5) && (!mqttClient->connected()); i++) {
		
		if(mqttUsername.length() == 0) {
			connectResult = mqttClient->connect(mqttClientId.c_str());
		} else {
			connectResult = mqttClient->connect(mqttClientId.c_str(), mqttUsername.c_str(), mqttPassword.c_str());
		}
		
		if(!connectResult) {
			if(_serial) {
				Serial.println("Could not connect to MQTT Server.. Retrying in 5 seconds..");
				Serial.print("State: ");
				Serial.println(mqttClient->state());
				delay(5000);
			}
			mqttClientActive = false;
		}
	}
	
	if(!mqttClient->connected()) {

		if(_serial) {
			Serial.println("Failed to connect to MQTT Server! Aborting.");
		}
		
		flashSOS();
		mqttClientActive = false;
		return false;
	}

	if(_serial) {
		Serial.println("MQTT Client Initialized!");
	}
	
	mqttClientActive = true;
	return true;
}

String CoogleIOT::getAPName()
{
	char APName[COOGLEIOT_AP_NAME_MAXLEN];

	if(!eeprom.readString(COOGLEIOT_AP_NAME_ADDR, APName, COOGLEIOT_AP_NAME_MAXLEN)) {
		if(_serial) {
			Serial.println("Failed to read AP Name from EEPROM");
		}
	}

	String retval(APName);
	return filterAscii(retval);
}

String CoogleIOT::getAPPassword()
{
	char password[COOGLEIOT_AP_PASSWORD_MAXLEN];

	if(!eeprom.readString(COOGLEIOT_AP_PASSWORD_ADDR, password, COOGLEIOT_AP_PASSWORD_MAXLEN)) {
		if(_serial) {
			Serial.println("Failed to read AP Password from EEPROM");
		}
	}

	String retval(password);
	return filterAscii(retval);
}

String CoogleIOT::filterAscii(String s)
{
	String retval;

	for(int i = 0; i < s.length(); i++) {

		if(isascii(s.charAt(i))) {
			retval += s.charAt(i);
		}
	}

	return retval;
}

String CoogleIOT::getRemoteAPName()
{
	char remoteAPName[COOGLEIOT_AP_NAME_MAXLEN];

	if(!eeprom.readString(COOGLEIOT_REMOTE_AP_NAME_ADDR, remoteAPName, COOGLEIOT_REMOTE_AP_NAME_MAXLEN)) {
		if(_serial) {
			Serial.println("Failed to read Remote AP Name from EEPROM");
		}
		remoteAPName[0] = 0;
	}

	String retval(remoteAPName);


	return filterAscii(retval);
}

String CoogleIOT::getRemoteAPPassword()
{
	char remoteAPPassword[COOGLEIOT_REMOTE_AP_PASSWORD_MAXLEN];

	if(!eeprom.readString(COOGLEIOT_REMOTE_AP_PASSWORD_ADDR, remoteAPPassword, COOGLEIOT_REMOTE_AP_PASSWORD_MAXLEN)) {
		if(_serial) {
			Serial.println("Failed to read remote AP password from EEPROM");
		}
	}

	String retval(remoteAPPassword);
	return filterAscii(retval);
}

bool CoogleIOT::connectToSSID()
{
	String remoteAPName;
	String remoteAPPassword;

	flashStatus(COOGLEIOT_STATUS_WIFI_INIT);
	
	remoteAPName = getRemoteAPName();
	remoteAPPassword = getRemoteAPPassword();
	
	if(remoteAPName.length() == 0) {
	
		if(_serial) {
			Serial.println("No Remote AP Found in memory");
		}
		
		return false;
	} 
	
	if(_serial) {
		Serial.print("Connecting to AP: ");
		Serial.println(remoteAPName);
	}
	
	if(remoteAPPassword.length() == 0) {
		if(_serial) {
			Serial.println("WARNING No Remote AP Password Set");
		}
		
		WiFi.begin(remoteAPName.c_str(), NULL, 0, NULL, true);
		
	} else {
		
		WiFi.begin(remoteAPName.c_str(), remoteAPPassword.c_str(), 0, NULL, true);
		
	}
	
	for(int i = 0; (i < 50) && (WiFi.status() != WL_CONNECTED) && (WiFi.status() != WL_CONNECT_FAILED); i++) {
		delay(500);
		
		if(_serial) {
			Serial.print('.');
		}
		
	}
	
	if(WiFi.status() != WL_CONNECTED) {
		if(_serial) {
			Serial.println("\nERROR: Could not connect to AP!");
			WiFi.printDiag(Serial);
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

CoogleIOT& CoogleIOT::enableSerial()
{
	return enableSerial(15200);
}

CoogleIOT& CoogleIOT::enableSerial(int baud)
{
    if(!Serial) {

      Serial.begin(baud);
  
      while(!Serial) {
          /* .... tic toc .... */
      }

    }

    _serial = true;
    return *this;
}
