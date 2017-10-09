#ifndef GARAGEDOOR_OPENER_H
#define GARAGEDOOR_OPENER_H

#define SERIAL_BAUD 115200

#define OPEN_SENSOR_PIN 2   // The pin that detects when the door is closed
#define CLOSE_SENSOR_PIN 5   // The pin that detects when the door is open
#define OPEN_SWTICH_PIN 14    // The pin that activates the open / close door action
#define LIGHT_SWITCH_PIN 4  // The pin that turns the light on / off

#define GARAGE_DOOR_STATUS_TOPIC "/status/garage-door"
#define GARAGE_DOOR_ACTION_TOPIC_DOOR "/garage-door/door"
#define GARAGE_DOOR_ACTION_TOPIC_LIGHT "/garage-door/light"
#define GARAGE_DOOR_MQTT_CLIENT_ID "garage-door"

#include <Arduino.h>

typedef enum {
    GD_OPEN,
    GD_CLOSED,
    GD_OPENING,
    GD_CLOSING,
    GD_UNKNOWN
} GarageDoorState;

#endif
