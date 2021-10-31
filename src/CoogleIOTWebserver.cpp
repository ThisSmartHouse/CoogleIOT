/*
  +----------------------------------------------------------------------+
  | CoogleIOT for ESP8266                                                |
  +----------------------------------------------------------------------+
  | Copyright (c) 2017-2018 John Coggeshall                              |
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
  | Authors: John Coggeshall <john@thissmarthouse.com>                   |
  +----------------------------------------------------------------------+
*/

#include <CoogleIOTWebserver.h>
#include <ESP8266WebServer.h>

#ifdef COOGLEIOT_DEBUG

#define MDNS_DEBUG_RX
#define MDNS_DEBUG_TX
#define MDNS_DEBUG_ERR

#endif

CoogleIOTWebserver::CoogleIOTWebserver(CoogleIOT &_iot)
{
	setIOT(_iot);

	this->serverPort = 80;

	iot->info("Creating Configuration Web Server");

	setWebserver(new ESP8266WebServer(this->serverPort));
}

CoogleIOTWebserver::CoogleIOTWebserver(CoogleIOT& _iot, int port)
{
	setIOT(_iot);

	this->serverPort = port;

	iot->info("Creating Configuration Web Server");
	setWebserver(new ESP8266WebServer(this->serverPort));
}

CoogleIOTWebserver::~CoogleIOTWebserver()
{
	delete webServer;
}

CoogleIOTWebserver& CoogleIOTWebserver::initializePages()
{
	webServer->on("/", std::bind(&CoogleIOTWebserver::handleRoot, this));
	webServer->on("/css", std::bind(&CoogleIOTWebserver::handleCSS, this));
	webServer->on("/reset", std::bind(&CoogleIOTWebserver::handleReset, this));
	webServer->on("/restart", std::bind(&CoogleIOTWebserver::handleRestart, this));
	webServer->on("/jquery", std::bind(&CoogleIOTWebserver::handleJS, this));
	webServer->on("/logs", std::bind(&CoogleIOTWebserver::handleLogs, this));

	webServer->on("/api/status", std::bind(&CoogleIOTWebserver::handleApiStatus, this));
	webServer->on("/api/reset", std::bind(&CoogleIOTWebserver::handleApiReset, this));
	webServer->on("/api/restart", std::bind(&CoogleIOTWebserver::handleApiRestart, this));
	webServer->on("/api/save", std::bind(&CoogleIOTWebserver::handleSubmit, this));

	webServer->on("/firmware-upload",
				  HTTP_POST,
				  std::bind(&CoogleIOTWebserver::handleFirmwareUploadResponse, this),
				  std::bind(&CoogleIOTWebserver::handleFirmwareUpload, this)
	);

	webServer->onNotFound(std::bind(&CoogleIOTWebserver::handle404, this));

	return *this;
}

CoogleIOTWebserver& CoogleIOTWebserver::setServerPort(int port)
{
	this->serverPort = port;
	return *this;
}

CoogleIOTWebserver& CoogleIOTWebserver::setWebserver(ESP8266WebServer* server)
{
	this->webServer = server;
	return *this;
}

CoogleIOTWebserver& CoogleIOTWebserver::setIOT(CoogleIOT& _iot)
{
	this->iot = &_iot;
	return *this;
}

bool CoogleIOTWebserver::initialize()
{
	iot->info("Initializing Webserver");

	initializePages();
	webServer->begin();

	iot->info("Webserver Initiailized!");

	return true;
}

void CoogleIOTWebserver::loop()
{
	webServer->handleClient();
}

String CoogleIOTWebserver::htmlEncode(char *input)
{
	String i = String(input);
	return htmlEncode(i);
}

String CoogleIOTWebserver::htmlEncode(String& input)
{
	char t;
	String retval, escape;

	for(int i = 0; i < input.length(); i++) {
		t = input.charAt(i);
        switch(t) {
			case '&':
				escape = "&amp;";
				break;

			case '<':
				escape = "&lt;";
				break;

			case '>':
				escape = "&gt;";
				break;

			case '"':
				escape = "&quot;";
				break;

			case '\'':
				escape = "&#39;";
				break;

			default:
				escape = t;
				break;
        }

		retval = retval + escape;
	}

	return retval;
}
void CoogleIOTWebserver::handleRoot()
{
	String page(FPSTR(WEBPAGE_Home));
	String ap_name, ap_password, ap_remote_name, ap_remote_password,
	       mqtt_host, mqtt_username, mqtt_password, mqtt_client_id,
				 mqtt_lwt_topic, mqtt_lwt_message, 
				 appl_specific_1, appl_specific_2,
				 appl_specific_name_1, appl_specific_name_2,
				 firmware_url, mqtt_port,
				 local_ip, mac_address, wifi_status, logs;

	ap_name = iot->getAPName();
	ap_password = iot->getAPPassword();
	ap_remote_name = iot->getRemoteAPName();
	ap_remote_password = iot->getRemoteAPPassword();
	mqtt_host = iot->getMQTTHostname();
	mqtt_username = iot->getMQTTUsername();
	mqtt_password = iot->getMQTTPassword();
	mqtt_client_id = iot->getMQTTClientId();
	mqtt_lwt_topic = iot->getMQTTLWTTopic();
	mqtt_lwt_message = iot->getMQTTLWTMessage();
	//--
	appl_specific_1 = iot->getMQTTAppSpecific1();
	appl_specific_2 = iot->getMQTTAppSpecific2();
	appl_specific_name_1 = iot->getMQTTSpecific1Name();
	appl_specific_name_2 = iot->getMQTTSpecific2Name();
	//--
	firmware_url = iot->getFirmwareUpdateUrl();
	mqtt_port = String(iot->getMQTTPort());
	local_ip = WiFi.localIP().toString();
	mac_address = WiFi.macAddress();
	wifi_status = iot->getWiFiStatus();

	page.replace(F("{{ap_name}}"), htmlEncode(ap_name));
	page.replace(F("{{ap_password}}"), htmlEncode(ap_password));
	page.replace(F("{{remote_ap_name}}"), htmlEncode(ap_remote_name));
	page.replace(F("{{remote_ap_password}}"), htmlEncode(ap_remote_password));
	page.replace(F("{{mqtt_host}}"), htmlEncode(mqtt_host));
	page.replace(F("{{mqtt_username}}"), htmlEncode(mqtt_username));
	page.replace(F("{{mqtt_password}}"), htmlEncode(mqtt_password));
	page.replace(F("{{mqtt_client_id}}"), htmlEncode(mqtt_client_id));
	page.replace(F("{{mqtt_lwt_topic}}"), htmlEncode(mqtt_lwt_topic));
	page.replace(F("{{mqtt_lwt_message}}"), htmlEncode(mqtt_lwt_message));
	//-
	
	page.replace(F("{{appl_specific_1}}"), htmlEncode(appl_specific_1));
	page.replace(F("{{appl_specific_2}}"), htmlEncode(appl_specific_2));
	page.replace(F("{{appl_specific_name_1}}"), htmlEncode(appl_specific_name_1));
	page.replace(F("{{appl_specific_name_2}}"), htmlEncode(appl_specific_name_2));
	//-
	page.replace(F("{{firmware_url}}"), htmlEncode(firmware_url));
	page.replace(F("{{mqtt_port}}"), htmlEncode(mqtt_port));
	page.replace(F("{{coogleiot_version}}"), htmlEncode(COOGLEIOT_VERSION));
	page.replace(F("{{coogleiot_buildtime}}"), htmlEncode(__DATE__ " " __TIME__));
	page.replace(F("{{coogleiot_ap_ssid}}"), htmlEncode(ap_name));
	page.replace(F("{{wifi_ip_address}}"), htmlEncode(local_ip));
	page.replace(F("{{mac_address}}"), htmlEncode(mac_address));
	page.replace(F("{{wifi_status}}"), htmlEncode(wifi_status));
	page.replace(F("{{mqtt_status}}"), iot->mqttActive() ? "Active" : "Not Connected");
	page.replace(F("{{ntp_status}}"), iot->ntpActive() ? "Active" : "Not Connected");
	page.replace(F("{{dns_status}}"), iot->dnsActive() ? "Active" : "Disabled");
	page.replace(F("{{firmware_update_status}}"), iot->firmwareClientActive() ? "Active" : "Disabled");
	page.replace(F("{{coogleiot_ap_status}}"), iot->apStatus() ? "Active" : "Disabled");

    webServer->send(200, "text/html", page.c_str());
}

void CoogleIOTWebserver::handleJS()
{
	webServer->sendHeader("Content-Encoding", "gzip", true);
	webServer->send_P(200, "application/javascript", jquery_3_2_1_min_js_gz, jquery_3_2_1_min_js_gz_len);
}

void CoogleIOTWebserver::handleCSS()
{
	webServer->send_P(200, "text/css", WEBPAGE_CSS, mini_default_min_css_len);
}

void CoogleIOTWebserver::handle404()
{
	webServer->send_P(404, "text/html", WEBPAGE_NOTFOUND);
}

void CoogleIOTWebserver::handleLogs()
{
	File logFile;

	logFile = iot->getLogFile();

	logFile.seek(0, SeekSet);
	webServer->streamFile(logFile, "text/html");
	logFile.seek(0, SeekEnd);
}

void CoogleIOTWebserver::handleFirmwareUploadResponse()
{
	if(_manualFirmwareUpdateSuccess) {
		webServer->send_P(200, "text/html", WEBPAGE_Restart);
		return;
	}

	webServer->send(200, "text/html", "There was an error uploading the firmware");

}

void CoogleIOTWebserver::handleFirmwareUpload()
{
	HTTPUpload& upload = webServer->upload();
	uint32_t maxSketchSpace;

	switch(upload.status) {
		case UPLOAD_FILE_START:
			WiFiUDP::stopAll();

			iot->info("Receiving Firmware Upload...");

			maxSketchSpace = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;

			if(!Update.begin(maxSketchSpace)) {
				iot->error("Failed to begin Firmware Upload!");

				if(iot->serialEnabled()) {
					Update.printError(Serial);
				}

				_manualFirmwareUpdateSuccess = false;
			}

			break;
		case UPLOAD_FILE_WRITE:

			if(Update.write(upload.buf, upload.currentSize) != upload.currentSize) {

				iot->error("Failed to write Firmware Upload!");

				if(iot->serialEnabled()) {
					Update.printError(Serial);
				}

				_manualFirmwareUpdateSuccess = false;
			}
			break;

		case UPLOAD_FILE_END:

			if(Update.end(true)) {

				iot->info("Firmware updated!");

				_manualFirmwareUpdateSuccess = true;

			} else {
				iot->error("Failed to update Firmware!");

				if(iot->serialEnabled()) {
					Update.printError(Serial);
				}

				_manualFirmwareUpdateSuccess = false;
			}

			break;

		case UPLOAD_FILE_ABORTED:
			Update.end();

			iot->info("Firmware upload aborted!");

			_manualFirmwareUpdateSuccess = false;

			break;
	}

	yield();
}

void CoogleIOTWebserver::handleSubmit()
{
	StaticJsonBuffer<200> jsonBuffer;
	WiFiClientPrint<> p(webServer->client());

	JsonObject& retval = jsonBuffer.createObject();
	JsonArray& errors = retval.createNestedArray("errors");

	String ap_name, ap_password, remote_ap_name, remote_ap_password,
	       mqtt_host, mqtt_port, mqtt_username, mqtt_password, mqtt_client_id,
		   	mqtt_lwt_topic, mqtt_lwt_message,
			appl_specific_1, appl_specific_2,
			firmware_url;

	bool success = true;

	ap_name = webServer->arg("ap_name");
	ap_password = webServer->arg("ap_password");
	remote_ap_name = webServer->arg("remote_ap_name");
	remote_ap_password = webServer->arg("remote_ap_password");
	mqtt_host = webServer->arg("mqtt_host");
	mqtt_port = webServer->arg("mqtt_port");
	mqtt_username = webServer->arg("mqtt_username");
	mqtt_password = webServer->arg("mqtt_password");
	mqtt_client_id = webServer->arg("mqtt_client_id");
	mqtt_lwt_topic = webServer->arg("mqtt_lwt_topic");
	mqtt_lwt_message = webServer->arg("mqtt_lwt_message");
	//-
	appl_specific_1 = webServer->arg("appl_specific_1");
	appl_specific_2 = webServer->arg("appl_specific_2");
	//-
	firmware_url = webServer->arg("firmware_url");

	if(ap_name.length() > 0) {
		if(ap_name.length() < COOGLEIOT_AP_NAME_MAXLEN) {
			iot->setAPName(ap_name);
		} else {
			errors.add("AP Name was too long");
			success = false;
		}
	}

	if(ap_password.length() < COOGLEIOT_AP_PASSWORD_MAXLEN) {
		iot->setAPPassword(ap_password);
	} else {
		errors.add("AP Password is too long!");
		success = false;
	}

	if(remote_ap_name.length() > 0) {
		if(remote_ap_name.length() < COOGLEIOT_REMOTE_AP_NAME_MAXLEN) {
			iot->setRemoteAPName(remote_ap_name);
		} else {
			errors.add("Remote AP Name is too long!");
			success = false;
		}
	}

	if(remote_ap_password.length() < COOGLEIOT_REMOTE_AP_PASSWORD_MAXLEN) {
		iot->setRemoteAPPassword(remote_ap_password);
	} else {
		errors.add("Remote AP Password was too long!");
		success = false;
	}

	if(mqtt_host.length() > 0) {
		if(mqtt_host.length() < COOGLEIOT_MQTT_HOST_MAXLEN) {
			iot->setMQTTHostname(mqtt_host);
		} else {
			errors.add("The MQTT Hostname was too long!");
			success = false;
		}
	}

	if(mqtt_port.length() > 0) {
		if(mqtt_port.toInt() > 0) {
			iot->setMQTTPort(mqtt_port.toInt());
		} else {
			errors.add("The MQTT Port was Invalid");
			success = false;
		}
	}

	if(mqtt_username.length() > 0) {
		if(mqtt_username.length() < COOGLEIOT_MQTT_USER_MAXLEN) {
			iot->setMQTTUsername(mqtt_username);
		} else {
			errors.add("The MQTT Username was too long");
			success = false;
		}
	}

	if(mqtt_password.length() > 0) {
		if(mqtt_password.length() < COOGLEIOT_MQTT_USER_PASSWORD_MAXLEN) {
			iot->setMQTTPassword(mqtt_password);
		} else {
			errors.add("The MQTT Password was too long");
			success = false;
		}
	}

	if(mqtt_client_id.length() > 0) {
		if(mqtt_client_id.length() < COOGLEIOT_MQTT_CLIENT_ID_MAXLEN) {
			iot->setMQTTClientId(mqtt_client_id);
		} else {
			errors.add("The MQTT Client ID was too long");
			success = false;
		}
	}

	if(mqtt_lwt_topic.length() > 0) {
		if(mqtt_lwt_topic.length() < COOGLEIOT_MQTT_LWT_TOPIC_MAXLEN) {
			iot->setMQTTLWTTopic(mqtt_lwt_topic);
		} else {
			errors.add("The MQTT LWT topic was too long");
			success = false;
		}
	}

	if(mqtt_lwt_message.length() > 0) {
		if(mqtt_lwt_message.length() < COOGLEIOT_MQTT_LWT_MESSAGE_MAXLEN) {
			iot->setMQTTLWTMessage(mqtt_lwt_message);
		} else {
			errors.add("The MQTT LWT message was too long");
			success = false;
		}
	}

	//---
	if(appl_specific_1.length() > 0) {
		if(appl_specific_1.length() < COOGLEIOT_MQTT_APP_SPECIFIC_1_MAXLEN) {
			iot->setMQTTAppSpecific1(appl_specific_1);
		} else {
			errors.add("App specific data (1) was too long");
			success = false;
		}
	}
	if(appl_specific_2.length() > 0) {
		if(appl_specific_2.length() < COOGLEIOT_MQTT_APP_SPECIFIC_2_MAXLEN) {
			iot->setMQTTAppSpecific2(appl_specific_2);
		} else {
			errors.add("App specific data (2) was too long");
			success = false;
		}
	}
	//---

	if(firmware_url.length() > 0) {
		if(firmware_url.length() < COOGLEIOT_FIRMWARE_UPDATE_URL_MAXLEN) {
			iot->setFirmwareUpdateUrl(firmware_url);
		} else {
			errors.add("The Firmware Update URL was too long");
			success = false;
		}
	}

	retval["status"] = success;

	webServer->setContentLength(retval.measureLength());
	webServer->send(200, "application/json", "");

	retval.printTo(p);
	p.stop();

}

void CoogleIOTWebserver::handleReset()
{
	webServer->send_P(200, "text/html", WEBPAGE_Restart);
	iot->resetEEProm();
}

void CoogleIOTWebserver::handleRestart()
{
	webServer->send_P(200, "text/html", WEBPAGE_Restart);
}

void CoogleIOTWebserver::handleApiReset()
{
	iot->resetEEProm();
}

void CoogleIOTWebserver::handleApiRestart()
{
	iot->restartDevice();
}

void CoogleIOTWebserver::handleApiStatus()
{
	StaticJsonBuffer<200> jsonBuffer;
	WiFiClientPrint<> p(webServer->client());

	JsonObject& retval = jsonBuffer.createObject();

	retval["status"] = !iot->_restarting;

	webServer->setContentLength(retval.measureLength());
	webServer->send(200, "application/json", "");
	retval.printTo(p);
	p.stop();

}
