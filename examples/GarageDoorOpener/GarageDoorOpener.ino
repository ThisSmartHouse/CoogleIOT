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

	iot->warn("Garage Door State Value Unknown!");

	return String("unknown");
}

GarageDoorState getGarageDoorState()
{
	bool isClosed, isOpen;
	GarageDoorState retval = GD_UNKNOWN;

	isOpen = digitalRead(OPEN_SENSOR_PIN) == LOW;
	isClosed = digitalRead(CLOSE_SENSOR_PIN) == LOW;


	if(isOpen && isClosed) {
		iot->error("Can't be both open and closed at the same time! Sensor failure!");

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

void triggerDoor()
{
	iot->info("Triggering Garage Door Open");
	digitalWrite(OPEN_SWTICH_PIN, LOW);
	delay(200);
	digitalWrite(OPEN_SWTICH_PIN, HIGH);
}

void triggerLight()
{
	iot->info("Triggering Garage Door Light");
	digitalWrite(LIGHT_SWITCH_PIN, LOW);
	delay(200);
	digitalWrite(LIGHT_SWITCH_PIN, HIGH);

}

void setup()
{
	iot = new CoogleIOT(LED_BUILTIN);

	iot->enableSerial(SERIAL_BAUD)
        .setMQTTClientId(GARAGE_DOOR_MQTT_CLIENT_ID)
	    .initialize();

	pinMode(OPEN_SWTICH_PIN, OUTPUT);
	pinMode(LIGHT_SWITCH_PIN, OUTPUT);
	pinMode(OPEN_SENSOR_PIN, INPUT_PULLUP);
	pinMode(CLOSE_SENSOR_PIN, INPUT_PULLUP);

	digitalWrite(OPEN_SWTICH_PIN, HIGH);
	digitalWrite(LIGHT_SWITCH_PIN, HIGH);

	if(iot->mqttActive()) {
		mqtt = iot->getMQTTClient();

		mqtt->setCallback(mqttCallbackHandler);

		iot->logPrintf(INFO, "Subscribed to Door-Open Topic: %s", GARAGE_DOOR_ACTION_TOPIC_DOOR);
		iot->logPrintf(INFO, "Subscribed to Light-Activate Topic: %s", GARAGE_DOOR_ACTION_TOPIC_LIGHT);

		mqtt->subscribe(GARAGE_DOOR_ACTION_TOPIC_DOOR);
		mqtt->subscribe(GARAGE_DOOR_ACTION_TOPIC_LIGHT);

		mqtt->publish(GARAGE_DOOR_STATUS_TOPIC, getDoorStateAsString(_currentState).c_str(), true);

		iot->info("Garage Door Opener Initialized");

	} else {
		iot->error("MQTT Not initialized, Garage Door Opener Inactive");
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

	if(strcmp(topic, GARAGE_DOOR_ACTION_TOPIC_DOOR) == 0) {

		iot->info("Handling Garage Door Action Request");
		iot->flashStatus(200, 1);
		triggerDoor();

	} else if(strcmp(topic, GARAGE_DOOR_ACTION_TOPIC_LIGHT) == 0) {

		iot->info("Handing Garage Door Light Request");
		iot->flashStatus(200, 2);
		triggerLight();

	}
}
