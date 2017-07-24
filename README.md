# CoogleIOT

A ESP8266-12 Library for easy IOT device development.

The CoogleIOT library was created to make building IOT devices on the ESP8266-12 microcontroller easier by providing a
solid encapsulated framework for most of the common things you want to do on an IOT device, including:

- Captive Portal for configuration of the device - allowing you to configure the AP name, the Wifi Client, and the built in MQTT Client. 
  Just connect to the AP and configure (mobile friendly).
- Built in MQTT client (provided by PubSubClient) 
- Built in UI libraries for the device (Mini.css for style, jquery 3.x for Javascript) that can be served from the AP
  using the /css or /jquery URLs
- Built in NTP client for access to local date / time on device
- Built in DNS Server during configuration for captive portal support when connected to the device as an AP directly
- Built in Security-minded tools like HTML Escaping and other filters to provide malicious inputs
- Built in OTA firmware update support. Can both upload a new firmware from the UI or pull a new one down from a server

## Screenshots

WiFi Configuration

![Wifi Configuration](https://raw.github.com/coogle/CoogleIOT/master/screenshots/wifi-screen.png)

MQTT Client Configuration

![MQTT Configuration](https://raw.github.com/coogle/CoogleIOT/master/screenshots/mqtt-screen.png)

System Configuration

![System Configuration](https://raw.github.com/coogle/CoogleIOT/master/screenshots/system-screen.png)

Status Page

![Status Page](https://raw.github.com/coogle/CoogleIOT/master/screenshots/status-screen.png)

## Example

CoogleIOT is designed to hang out in the background so your sketches are focused on the things you actually want to
work on without having to worry about things like WiFi or MQTT clients. Here is an example of using it to control a
Garage Door w/open and close sensors

```
#include <CoogleIOT.h>
#include "GarageDoor-Opener.h"

CoogleIOT *iot;
PubSubClient *mqtt;

GarageDoorState _currentState = GD_UNKNOWN;

String getDoorStateAsString(GarageDoorState state)
{
	switch(state) {
		case GD_OPEN:
			return String("open");
		case GD_CLOSED:
			return String("closed");
		case GD_OPENING:
			return String("opening");
		case GD_CLOSING:
			return String("closing");
		case GD_UNKNOWN:
			return String("unknown");
	}

	if(iot->serialEnabled()) {
		Serial.println("Warning: Garage Door State value unknown");
	}

	return String("unknown");
}

GarageDoorState getGarageDoorState()
{
	bool isClosed, isOpen;
	GarageDoorState retval = GD_UNKNOWN;

	isOpen = digitalRead(OPEN_SENSOR_PIN) == LOW;
	isClosed = digitalRead(CLOSE_SENSOR_PIN) == LOW;

	if(isOpen && isClosed) {
		if(iot->serialEnabled()) {
			Serial.println("ERROR: Can't be both open and closed at the same time! Sensor failure!");
		}
		retval = GD_UNKNOWN;
		return retval;
	}

	if(!isOpen && isClosed) {
		retval = GD_CLOSED;
		return retval;
	}

	if(isOpen && !isClosed) {
		retval = GD_OPEN;
		return retval;
	}

	if(!isOpen && !isClosed) {

		if((_currentState == GD_OPEN) || (_currentState == GD_CLOSING)) {
			retval = GD_CLOSING;
			return retval;
		}

		if((_currentState == GD_CLOSED) || (_currentState == GD_OPENING)) {
			retval = GD_OPENING;
			return retval;
		}
	}

	retval = GD_UNKNOWN;
	return retval;
}

void toggleActionPin(unsigned int delayMS, unsigned int repeat)
{
	for(int i = 0; i < repeat; i++) {
		digitalWrite(OPEN_SWTICH_PIN, HIGH);
		delay(delayMS);
		digitalWrite(OPEN_SWTICH_PIN, LOW);
		delay(delayMS);
	}
}

void setup()
{
	iot = new CoogleIOT(LED_BUILTIN);

	iot->enableSerial(SERIAL_BAUD);
	iot->initialize();

	pinMode(OPEN_SWTICH_PIN, OUTPUT);
	pinMode(OPEN_SENSOR_PIN, INPUT_PULLUP);
	pinMode(CLOSE_SENSOR_PIN, INPUT_PULLUP);

	digitalWrite(OPEN_SWTICH_PIN, LOW);

	if(iot->mqttActive()) {
		mqtt = iot->getMQTTClient();

		mqtt->setCallback(mqttCallbackHandler);
		mqtt->subscribe(GARAGE_DOOR_ACTION_TOPIC);
		mqtt->publish(GARAGE_DOOR_STATUS_TOPIC, getDoorStateAsString(_currentState).c_str(), true);

		if(iot->serialEnabled()) {
			Serial.println("Garage Door Opener Initialized");
		}

	} else {
		if(iot->serialEnabled()) {
			Serial.println("ERROR: MQTT Not Initialized!");
		}
	}
}

void loop()
{
	GarageDoorState liveState;

	iot->loop();

	if(iot->mqttActive()) {
		liveState = getGarageDoorState();

		if(liveState != _currentState) {
			mqtt->publish(GARAGE_DOOR_STATUS_TOPIC, getDoorStateAsString(liveState).c_str(), true);
			_currentState = liveState;
		}
	}

}

void mqttCallbackHandler(char *topic, byte *payload, unsigned int length)
{
	String action;
	char *payloadStr;

	// This is annoying, payload is not null-terminated as a string,
	// and the String class doesn't let us initialize it with a defined-length
	// so we need to create our own first.

	payloadStr = (char *)malloc(length + 1);
	memcpy(payloadStr, payload, length);
	payloadStr[length] = NULL;
	action = String(payloadStr);
	free(payloadStr);

	if(action.toInt() > 0) {

		if(iot->serialEnabled()) {
			Serial.printf("Toggling at frequency of %d ms (5 iterations)\n", action.toInt());
		}

		toggleActionPin(action.toInt(), 5);
		return;
	}

	if(action.equals("open") || action.equals("close")) {

		if(iot->serialEnabled()) {
			Serial.println("Toggled at button-press frequency");
		}

		toggleActionPin(1000, 1);
		return;
	} else {
		if(iot->serialEnabled()) {
			Serial.printf("Unknown Command issued: %s\n", action.c_str());
		}
	}
}
```

## API

CoogleIOT is an evolving code base, so this API may change before this document is updated to reflect that. The best source is the source. When possible CoogleIOT uses a fluent interface, allowing you to chain method calls together:

```
  // Chaining method calls together
  iot->enableSerial(115200)
     ->initialize();
```

`void CoogleIOT::CoogleIOT(status_led_pin = NULL)` 
The library constructor. You may provide an optional pin to use for a status LED which will be used to indicate different
states the device can be in (i.e. WiFi initializing, etc.)

`bool CoogleIOT::initialize()`
Must be called in `setup()` of your sketch to initialize the library and it's components

`void CoogleIOT::loop()`
Must be called in `loop()` of your sketch

`CoogleIOT& CoogleIOT::enableSerial(int baud = 115200)`
Enables Serial output from the IOT library. Will initialize the `Serial` object for you at the baud rate specified if not
already initialized.

`PubSubClient* CoogleIOT::getMQTTClient()`
Return a pointer to the built in PubSubClient to use in your sketch

`bool CoogleIOT::serialEnabled()`
Returns true if Serial is enabled

`CoogleIOT& CoogleIOT::flashStatus(speed_in_ms, repeat = 5)`
Flashes the defined pin / LED at a speed, repeating as defined (5 times by default)

`CoogleIOT& CoogleIOT::flashSOS()`
Flashes the status pin / LED in an SOS pattern (useful to indicate an error)

`CoogleIOT& CoogleIOT::resetEEProm()`
Resets the EEPROM memory used by CoogleIOT to NULL, effectively "factory resetting" the device

`void CoogleIOT::restartDevice()`
Restarts the device. Due to [This Bug](https://github.com/esp8266/Arduino/issues/1722), you must physically press the restart button on the ESP8266 after flashing via Serial. If you fail to do that, this command will hang the device.

`String CoogleIOT::filterAscii()`
Filters the provided string of anything that is not a printable ASCII character

`bool CoogleIOT::verifyFlashConfiguration()`
Verifies the Flash configuration for the device (what the device supports, vs. what the device is set as in your sketch) is
correct.

`CoogleIOT& CoogleIOT::syncNTPTime(int offsetSeconds, int daylightOffsetSeconds)`
Synchronizes and sets the local device date / time based on NTP servers. Must have a working WiFi connection to use this
method. The first parameter is the number of seconds local time is offset from UTC time (i.e. -5 hrs in seconds is America/New York). The second parameter is the number of seconds to offset based on daylight savings.

`String CoogleIOT::getWiFiStatus()`
Returns a string representing the current state of the WiFi Client

`bool CoogleIOT::mqttActive()`
Returns true/false indicating if the MQTT client is active and ready to use or not

`bool CoogleIOT::dnsActive()`
Returns true/false if the integrated captive portal DNS is enabled or not

`bool CoogleIOT::ntpActive()`
Returns true/false if the NTP client is online and synchronizing with NTP time servers

`bool CoogleIOT::firmwareClientActive()`
Returns true/false if the periodic firmware client (that will download a new firmware from a web server) is active or not. If active, the Firmware client will check every 30 minutes for a new firmware at the configured URL

`bool CoogleIOT::apStatus()`
Returns true/false if the AP of the device is active or not.

`void CoogleIOT::checkForFirmwareUpdate()`
Performs a check against the specified Firmware Server endpoint for a new version of this device's firmware. If a new version exists it performs the upgrade.

The following getters/setters are pretty self explainatory. Each getter will return a `String` object of the value from EEPROM (or another primiative data type), with a matching setter:

`String CoogleIOT::getRemoteAPName()`
`CoogleIOT& CoogleIOT::setRemoteAPName(String)`
`String CoogleIOT::getRemoteAPPassword()`
`CoogleIOT& CoogleIOT::setRemoteAPPassword(String)`
`String CoogleIOT::getMQTTHostname()`
`CoogleIOT& CoogleIOT::setMQTTHostname(String)`
`String CoogleIOT::getMQTTUsername()`
`CoogleIOT& CoogleIOT::setMQTTUsername(String)`
`String CoogleIOT::getMQTTPassword()`
`CoogleIOT& CoogleIOT::setMQTTPassword(String)`
`String CoogleIOT::getMQTTClientId()`
`CoogleIOT& CoogleIOT::setMQTTClientId()`
`int CoogleIOT::getMQTTPort()`
`CoogleIOT& CoogleIOT::setMQTTPort(int)`
`String CoogleIOT::getAPName()`
`CoogleIOT& CoogleIOT::setAPName(String)`
`String CoogleIOT::getAPPassword()`
`CoogleIOT& CoogleIOT::setAPPassword(String)`
`String CoogleIOT::getFirmwareUpdateUrl()`
`CoogleIOT& CoogleIOT::setFirmwareUpdateUrl(String)`

## CoogleIOT Firmware Configuration

The Firmware default values and settings are defined in the `CoogleIOTConfig.h` file and can be overriden by providing new `#define` statements

`#define COOGLEIOT_STATUS_INIT 500`
Defines the status LED flash speed in MS for initial initialization

`#define COOGLEIOT_STATUS_WIFI_INIT 250`
Defines the status LED flash speed for WiFi initialization

`#define COOGLEIOT_STATUS_MQTT_INIT 100`
Defines the status LED flash speed for MQTT initialization

`#define COOGLEIOT_AP "COOGLEIOT_`
Defines the prepended string used for the default AP name. The remainder of the AP name will be a randomly generated number (i.e. COOGLEIOT_234934)

`#define COOGLEIOT_AP_DEFAULT_PASSWORD "coogleiot"`
The default AP password

`#define COOGLEIOT_DEFAULT_MQTT_CLIENT_ID "coogleIoT"`
The default MQTT client ID

`#define COOGLEIOT_DEFAULT_MQTT_PORT 1883`
The default MQTT Port

`#define COOGLEIOT_TIMEZONE_OFFSET ((3600 * 5) * -1)`
The default NTP Timezone offset (America/New York)

`#define COOGLEIOT_DAYLIGHT_OFFSET 0`
The default NTP Daylight Offset

`
#define COOGLEIOT_NTP_SERVER_1 "pool.ntp.org"
#define COOGLEIOT_NTP_SERVER_2 "time.nist.gov"
#define COOGLEIOT_NTP_SERVER_3 "time.google.com"
`
The three default NTP servers to attempt to synchronize with

`#define COOGLEIOT_FIRMWARE_UPDATE_CHECK_MS 54000000  // 15 Minutes in Milliseconds`
The frequency that we will check for a new Firmware Update if the server is configured. Defaults to 15 minutes.

`#define COOGLEIOT_DNS_PORT 53`
The default DNS port

`#define COOGLE_EEPROM_EEPROM_SIZE 1024`
The amount of EEPROM memory allocated to CoogleIOT, 1kb default. 

*IMPORTANT NOTE:* 
Do _NOT_ reduce this value below it's default value unless you really know what you are doing, otherwise you will break the firmware.

`#define COOGLEIOT_WEBSERVER_PORT 80`
The default Webserver port for the configuration system

`#define COOGLEIOT_DEBUG`
If defined, it will enable debugging mode for CoogleIOT which will dump lots of debugging data to the Serial port (if enabled)
