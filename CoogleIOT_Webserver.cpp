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

CoogleIOTWebserver::CoogleIOTWebserver(CoogleIOT &coogleiot)
{
	iot = coogleiot;
}

void CoogleIOTWebserver::initialize()
{
	if(!MDNS.begin(COOGLEIOT_WS_MDNS_DOMAIN)) {
		if(iot.serialEnabled()) {
			Serial.println("ERROR Initializing the mDNS Responder");
		}

		iot.flashSOS();
	}

	webServer = new ESP8266WebServer(COOGLEIOT_WEBSERVER_PORT);

	webServer.on("/", handleRoot);
	webServer.onNotFound(handle404);

	webServer.begin();
}

void CoogleIOTWebserver::loop()
{
	webServer.handleClient();
}

void CoogleIOTWebserver::handleRoot()
{

}

void CoogleIOTWebserver::handle404()
{

}
