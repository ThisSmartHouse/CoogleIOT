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

}

void CoogleIOTWebserver::loop()
{
	webServer->handleClient();
}

void CoogleIOTWebserver::handleRoot()
{
    webServer->send_P(200, "text/html", WEBPAGE_Home);
}

void CoogleIOTWebserver::handleCSS()
{
	webServer->send_P(200, "text/css", WEBPAGE_CSS);
}

void CoogleIOTWebserver::handle404()
{
	webServer->send_P(404, "text/html", WEBPAGE_NOTFOUND);
}

void CoogleIOTWebserver::handleSubmit()
{

}
