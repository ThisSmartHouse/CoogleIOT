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

#ifndef COOGLEIOT_EEPROM_MAP
#define COOGLEIOT_EEPROM_MAP

#define COOGLEIOT_AP_PASSWORD_ADDR 5 // 5-21
#define COOGLEIOT_AP_PASSWORD_MAXLEN 16

#define COOGLEIOT_AP_NAME_ADDR 22
#define COOGLEIOT_AP_NAME_MAXLEN 25 // 22-47

#define COOGLEIOT_REMOTE_AP_PASSWORD_ADDR 48
#define COOGLEIOT_REMOTE_AP_PASSWORD_MAXLEN 64 // 48-112

#define COOGLEIOT_MQTT_HOST_ADDR 113
#define COOGLEIOT_MQTT_HOST_MAXLEN 64 // 113 - 177

#define COOGLEIOT_MQTT_USER_ADDR 178
#define COOGLEIOT_MQTT_USER_MAXLEN 16 // 178 - 194

#define COOGLEIOT_MQTT_USER_PASSWORD_ADDR 195
#define COOGLEIOT_MQTT_USER_PASSWORD_MAXLEN 24 // 195 - 219

#define COOGLEIOT_MQTT_CLIENT_ID_ADDR 220
#define COOGLEIOT_MQTT_CLIENT_ID_MAXLEN 32 // 220 - 252

#define COOGLEIOT_MQTT_PORT_ADDR 253
#define COOGLEIOT_MQTT_PORT_MAXLEN 2 // 253 - 255 (int)

#define COOGLEIOT_FIRMWARE_UPDATE_URL_ADDR 282
#define COOGLEIOT_FIRMWARE_UPDATE_URL_MAXLEN 255 // 282 - 537

#define COOGLEIOT_REMOTE_AP_NAME_ADDR 538
#define COOGLEIOT_REMOTE_AP_NAME_MAXLEN 25 // 538- 563

#define COOGLEIOT_MQTT_LWT_TOPIC_ADDR 564
#define COOGLEIOT_MQTT_LWT_TOPIC_MAXLEN 128 // 564 - 692

#define COOGLEIOT_MQTT_LWT_MESSAGE_ADDR 693
#define COOGLEIOT_MQTT_LWT_MESSAGE_MAXLEN 128 // 693 - 821

//-
#define COOGLEIOT_MQTT_APP_SPECIFIC_1_ADDR 822 
#define COOGLEIOT_MQTT_APP_SPECIFIC_1_MAXLEN 64  // 822 - 886

#define COOGLEIOT_MQTT_APP_SPECIFIC_2_ADDR 855
#define COOGLEIOT_MQTT_APP_SPECIFIC_2_MAXLEN 32  // 887 - 951

#define COOGLEIOT_MQTT_SPECIFIC_1_NAME_ADDR 952
#define COOGLEIOT_MQTT_SPECIFIC_1_NAME_MAXLEN 25  // 952 - 977

#define COOGLEIOT_MQTT_SPECIFIC_2_NAME_ADDR 978
#define COOGLEIOT_MQTT_SPECIFIC_2_NAME_MAXLEN 25  // 977 - 1003

//-

#endif
