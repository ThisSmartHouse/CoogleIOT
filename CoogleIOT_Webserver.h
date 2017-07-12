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

#define COOGLEIOT_WEBSERVER_PORT 80
#define COOGLEIOT_WS_MDNS_DOMAIN "coogleiot"

class CoogleIOTWebserver
{
	public:
		CoogleIOTWebserver(CoogleIOT &);
		void initialize();
		void handleRoot();
		void handle404();
		void loop();
	private:
		ESP8266WebServer *webServer;
};

#endif
