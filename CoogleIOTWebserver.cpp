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

	if(iot->serialEnabled()) {
		Serial.print("Creating WebServer listening on port ");
		Serial.println(this->serverPort);
	}

	setWebserver(new ESP8266WebServer(this->serverPort));
}

CoogleIOTWebserver::CoogleIOTWebserver(CoogleIOT& _iot, int port)
{
	setIOT(_iot);

	this->serverPort = port;

	if(iot->serialEnabled()) {
		Serial.print("Creating WebServer listening on port ");
		Serial.println(this->serverPort);
	}

	setWebserver(new ESP8266WebServer(this->serverPort));
}

CoogleIOTWebserver& CoogleIOTWebserver::initializePages()
{
	webServer->on("/", std::bind(&CoogleIOTWebserver::handleRoot, this));
	webServer->on("/css", std::bind(&CoogleIOTWebserver::handleCSS, this));
	webServer->on("/save", std::bind(&CoogleIOTWebserver::handleSubmit, this));
	webServer->on("/reset", std::bind(&CoogleIOTWebserver::handleReset, this));
	webServer->on("/restart", std::bind(&CoogleIOTWebserver::handleRestart, this));
	webServer->on("/jquery", std::bind(&CoogleIOTWebserver::handleJS, this));

	webServer->on("/api/status", std::bind(&CoogleIOTWebserver::handleApiStatus, this));

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
//	if(!MDNS.begin(COOGLEIOT_WS_MDNS_DOMAIN)) {
//		if(iot->serialEnabled()) {
//			Serial.println("ERROR Initializing the mDNS Responder");
//		}
//
//		iot->flashSOS();
//		return false;
//
//	} else {
//
//		if(iot->serialEnabled()) {
//			Serial.println("mDNS Responder initialized");
//		}
//
//	}

	if(iot->serialEnabled()) {
		Serial.println("Initialzing Webserver...");
	}

	initializePages();
	webServer->begin();

	if(iot->serialEnabled()) {
		Serial.println("Webserver Initialized!");
	}

	return true;
}

void CoogleIOTWebserver::loop()
{
	webServer->handleClient();
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
		   firmware_url;

	ap_name = iot->getAPName();
	ap_password = iot->getAPPassword();
	ap_remote_name = iot->getRemoteAPName();
	ap_remote_password = iot->getRemoteAPPassword();
	mqtt_host = iot->getMQTTHostname();
	mqtt_username = iot->getMQTTUsername();
	mqtt_password = iot->getMQTTPassword();
	mqtt_client_id = iot->getMQTTClientId();
	firmware_url = iot->getFirmwareUpdateUrl();

	page.replace(F("{{ap_name}}"), htmlEncode(ap_name));
	page.replace(F("{{ap_password}}"), htmlEncode(ap_password));
	page.replace(F("{{remote_ap_name}}"), htmlEncode(ap_remote_name));
	page.replace(F("{{remote_ap_password}}"), htmlEncode(ap_remote_password));
	page.replace(F("{{mqtt_host}}"), htmlEncode(mqtt_host));
	page.replace(F("{{mqtt_username}}"), htmlEncode(mqtt_username));
	page.replace(F("{{mqtt_password}}"), htmlEncode(mqtt_password));
	page.replace(F("{{mqtt_client_id}}"), htmlEncode(mqtt_client_id));
	page.replace(F("{{firmware_url}}"), htmlEncode(firmware_url));

    webServer->send(200, "text/html", page.c_str());
}

void CoogleIOTWebserver::handleJS()
{
	webServer->sendHeader("Content-Encoding", "gzip", true);
	webServer->send_P(200, "application/javascript", jquery_3_2_1_slim_min_js_gz, jquery_3_2_1_slim_min_js_gz_len);
}

void CoogleIOTWebserver::handleCSS()
{
	webServer->send_P(200, "text/css", WEBPAGE_CSS, mini_default_min_css_len);
}

void CoogleIOTWebserver::handle404()
{
	webServer->send_P(404, "text/html", WEBPAGE_NOTFOUND);
}

void CoogleIOTWebserver::handleSubmit()
{
}

void CoogleIOTWebserver::handleReset()
{
	iot->resetEEProm();
	iot->restartDevice();
}

void CoogleIOTWebserver::handleRestart()
{
	iot->restartDevice();
}

void CoogleIOTWebserver::handleApiStatus()
{
	StaticJsonBuffer<200> jsonBuffer;
	WiFiClientPrint<> p(webServer->client());

	JsonObject& retval = jsonBuffer.createObject();

	retval["status"] = true;

	webServer->setContentLength(retval.measureLength());
	webServer->send(200, "application/json", "");
	retval.printTo(p);
	p.stop();

}
