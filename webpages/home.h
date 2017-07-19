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

#ifndef COOGLEIOT_WEBPAGES_HOME_H_
#define COOGLEIOT_WEBPAGES_HOME_H_

const char WEBPAGE_Home[] PROGMEM = R"=====(
<html>
  <head>
    <title>CoogleIOT</title>
    <link href="/css" type="text/css" rel="stylesheet">
    <meta name="viewport" content="width=device-width, initial-scale=1">
  </head>
  <body>
    <h3>CoogleIOT Device Setup</h3>
    <div class="tabs" style="margin-top: 5px; margin-bottom: 5px;">
      <input type="radio" name="tabdemo" id="tab1" checked="" aria-hidden="true">
      <label for="tab1" aria-hidden="true">WiFi</label>
        <div style="height: 500px">
         <form>
         <fieldset>
           <legend>Device Wireless Setup</legend>
             <p>Settings for the device WiFi (as AP)</p>
             <div class="input-group fluid">
               <label aria-hidden="true" for="ap_name">Device AP SSID</label>
               <input aria-hidden="true" type="text" value="" id="ap_name" placeholder="Device AP Name">
             </div>
             <div class="input-group fluid">
               <label aria-hidden="true" for="ap_password">Device AP Password</label>
               <input aria-hidden="true" type="password" value="" id="ap_password">
             </div>
         </fieldset>
         <fieldset>
           <legend>Client WiFi Setup</legend>
           <p>Settings for WiFi (as Client)</p>
           <div class="input-group fluid">
             <label aria-hidden="true" for="ap_remote_name">Remote SSID</label>
             <input aria-hidden="true" type="text" value="" id="ap_remote_name" placeholder="Remote AP Name">
           </div>
           <div class="input-group fluid">
              <label aria-hidden="true" for="ap_remote_password">Remote SSID Password</label>
              <input aria-hidden="true"type="password" value="" id="ap_remote_password">
           </div>
         </fieldset>
        </form>
      </div>
      <input type="radio" name="tabdemo" id="tab2" aria-hidden="true">
      <label for="tab2" aria-hidden="true">MQTT</label>
      <div style="height: 500px">
        <form>
        <fieldset>
          <legend>MQTT Client Configuration</legend>
          <div class="input-group fluid">
            <label aria-hidden="true" for="mqtt_host">MQTT Host</label>
            <input aria-hidden="true" type="text" value="" id="mqtt_host" placeholder="mqtt.example.com">
          </div>
          <div class="input-group fluid">
            <label aria-hidden="true" for="mqtt_port">MQTT Port</label>
            <input aria-hidden="true" type="text" value="" id="mqtt_port" placeholder="1883">
          </div>
          <div class="input-group fluid">
            <label aria-hidden="true" for="mqtt_username">MQTT Username</label>
            <input aria-hidden="true" type="text" value="" id="mqtt_username" placeholder="coogleiot">
          </div>
          <div class="input-group fluid">
            <label aria-hidden="true" for="mqtt_password">MQTT Pasword</label>
            <input aria-hidden="true" type="password" id="mqtt_password">
          </div>
          <div class="input-group fluid">
            <label aria-hidden="true" for="mqtt_client_id">MQTT Client ID</label>
            <input aria-hidden="true" type="text" value="" id="mqtt_client_id" placeholder="my-client-id">
          </div>
        </fieldset>
        </form>
      </div>
    </div>
    <button class="primary bordered" style="width: 100%">Save and Restart</button>
  </body>
</html>
)=====";


#endif /* COOGLEIOT_WEBPAGES_HOME_H_ */
