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

#ifndef COOGLEIOT_WEBSERVER_H
#define COOGLEIOT_WEBSERVER_H

#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>

#include "CoogleIOT.h"

#include "webpages/home.h"
#include "webpages/mini_css_default.h"

#define COOGLEIOT_WEBSERVER_PORT 80
#define COOGLEIOT_WS_MDNS_DOMAIN "coogleiot"

class CoogleIOT;

class CoogleIOTWebserver
{
	public:
		CoogleIOTWebserver(CoogleIOT& _iot);
		CoogleIOTWebserver(CoogleIOT& _iot, int port);

		CoogleIOTWebserver& setIOT(CoogleIOT& _iot);
		CoogleIOTWebserver& setWebserver(ESP8266WebServer* server);
		CoogleIOTWebserver& setServerPort(int port);

		bool initialize();
		void handleRoot();
		void handle404();
		void handleCSS();
		void loop();
	protected:
		CoogleIOTWebserver& initializePages();
	private:
		ESP8266WebServer* webServer;
		CoogleIOT* iot;
		int serverPort = 80;
};

#endif
