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
