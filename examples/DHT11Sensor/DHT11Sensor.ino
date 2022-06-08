  
///////////////////////////////////////////////////////////////////////////////////
// Get measurements from a DHT-11 Temperature/Humidity sensor and send them to an
// MQTT broker using the CoogleIOT library. 
//
// Remember to change the symbol MQTT_MAX_PACKET_SIZE in PubSubCient.h to a higher 
// value (1024 at least)
//
#include <Arduino.h>
#include <time.h>


//
// MQTT Message tags
// 
#define DHT11_MESSAGE_VERSION  "1.0"
#define DHT11_MEASUREMENT_NAME "DHT"

char* DHT11_SENSOR_TOPIC = "sensor/data/DHT";  


//
// CoogleIot
//
#include <CoogleIOT.h>

const unsigned long SERIAL_BAUD = 115200;

String CLIENT_ID;
String CLIENT_ADDRESS("");

CoogleIOT *iot;
PubSubClient *mqtt;


//
// DHT Humidity / Temperature sensors library for ESP microcontrollers
//
#include <DHTesp.h>

#ifndef ESP8266
#pragma message(DHTesp LIBRARY IS FOR ESP8266 ONLY!)
#error USE AN ESP8266 BOARD.
#endif

#define DHT_SENSOR_PIN 2   // DHT Sensor attached to GPIO2
const String CLIENT_PREFIX = "DHT_SENSOR_";
const unsigned long DHT_MIN_MEASUREMENT_INTERVAL = 20000;  // 20s
unsigned long dht_measurement_interval = DHT_MIN_MEASUREMENT_INTERVAL;  

DHTesp dht;

//
// ArduinoJson
//
// TO DO: A version of ArduinoJson.h is included in CoogleIOT src directory. 
//        Should migrate to a recent version to avoid incompatibilities with 
//        applications using it.
//
// See: https://arduinojson.org/v5/assistant/
//
          
#include <ArduinoJson.h>

// Sizes estimated with ample margin. 
#define DHT11_JSON_BUFFER_SIZE        800   // Memory used by ArduinoJson library
#define DHT11_MAX_JSON_MESSAGE_LEN    640   // Ouput message max size

static const char notavailable[] = "N/A";
const char* na(const char *data) {return data? data: notavailable; }

//
// Prototypes
//
bool publish_c_str(char *topic, char *message, bool retain = false);
bool publish_JSON(char *topic, JsonObject& jso, bool retain = false);
bool publish_measurement(float temp, float hum);

///////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////// SETUP / ///////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////

void setup()
{
  unsigned long rseed = micros();
  //
  // Initialize CoogleIOT
  // 
  Serial.begin(SERIAL_BAUD);
	iot = new CoogleIOT(LED_BUILTIN);

  rseed = +micros();  // add randomness
  
  iot->enableSerial(SERIAL_BAUD);
  
  Serial.println("Initializing CoogleIOT");
  iot->initialize(); 
  
  iot->logPrintf(INFO, "CoogleIOT Initialized. Local AP %s @ %s", iot->getAPName().c_str(),  WiFi.softAPIP().toString().c_str());

  rseed +=micros(); // some more ramdomness

  // If client Id es the library default (first boot) or the user has changed it to "*",
  // generate new Client Id and app specific data tags to specify sensor location
  rseed += micros();
  String cli_id = iot->getMQTTClientId();
  if ( (cli_id == COOGLEIOT_DEFAULT_MQTT_CLIENT_ID) || (cli_id == "") || ( cli_id == "*") ) {
    randomSeed(rseed);
    long num_id = random(999999999);
    String CLIENT_ID = CLIENT_PREFIX + String(num_id);
    iot->setMQTTClientId(CLIENT_ID);
    iot->logPrintf(INFO, "Client ID set to %s", CLIENT_ID.c_str());
    iot->setMQTTSpecific1Name("Location");
    iot->setMQTTSpecific2Name("Room");
  } else {
    CLIENT_ID = iot->getMQTTClientId();
  }

  // If WiFi connection unsuscessfull, wait up to 10 minutes for the user to reconfigure
  // and them procceed
  if (WiFi.status() == WL_CONNECTED) {
    CLIENT_ADDRESS = WiFi.localIP().toString();
    iot->logPrintf(INFO, "Connected to remote AP %s @ %s", iot->getRemoteAPName().c_str(), CLIENT_ADDRESS.c_str());
  } else {
    iot->logPrintf(WARNING, "Could not connect to SSID %s. Please reconfigure", iot->getRemoteAPName().c_str());
    // Give 10 minutes to reconfig
    unsigned int m = millis();
    while ((millis()-m)<(10*60*1000)) {
      iot->loopWebServer();
    }
    // Continue. If still unable to connect, CoogleIOT will eventually give up and reboot
  } 
 
  // Get MQTT Client
	if(iot->mqttActive()) {
    iot->logPrintf(INFO, "MQTT Active with client ID: %s", CLIENT_ID.c_str() );
		mqtt = iot->getMQTTClient();
	} else {
    // If MQTT connection unsuscessfull, wait up to 10 minutes for the user to reconfigure
    // and them procceed
		iot->error("MQTT Not initialized. Please reconfigure");
    unsigned int n = millis();
    // Give 10 minutes to reconfig. 
    while ((millis()-n)<(10*60*1000)) {
      iot->loopWebServer();
    }
    // Continue. If still unable to connect, CoogleIOT will eventually give up and reboot
	}
  
  //
  // DHT
  //
  // dht.setup(DHT_SENSOR_PIN, DHTesp::DHT22); // Connect DHT sensor 
  dht.setup(DHT_SENSOR_PIN, DHTesp::DHT11); // Connect DHT sensor
  
  dht_measurement_interval = DHT_MIN_MEASUREMENT_INTERVAL;
  if (dht_measurement_interval < dht.getMinimumSamplingPeriod()) {
   dht_measurement_interval = dht.getMinimumSamplingPeriod();
  }
    
  //
  // Finished 
  //
  iot->info("Setup complete");
}

///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////// MAIN LOOP ///////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////

unsigned long last_measurement_millis = millis();

void loop()
{
  bool mqtt_online = iot->mqttActive();      //<<<<<<<<<<<<<<<<<<<<
 
  ////////////////////////////////////////////////
  // Important, do CoogleIOT housekeeping stuff //
  ////////////////////////////////////////////////
  
  iot->loop();   
  
  //
  // Check measurement interval has elapsed
  //
  if ( (millis()-last_measurement_millis) <  dht_measurement_interval) {
    return;
  }
  last_measurement_millis = millis();
  
  //
  // Read from sensor and publish
  //
  float temperature = dht.getTemperature();
  float humidity = dht.getHumidity();

  Serial.print("MQTT: "); Serial.print(mqtt_online? "ONLINE": "OFFLINE");
  Serial.print("\tDHT Status: ");
  Serial.print(dht.getStatusString());
  Serial.print("\tHumidity: ");
  Serial.print(humidity , 1);
  Serial.print("%\tTemp: ");
  Serial.print(temperature, 1);
  Serial.print(" C\t");
  Serial.print(dht.toFahrenheit(temperature), 1);
  Serial.print(" F\tHeat Index: ");
  Serial.println(dht.computeHeatIndex(temperature, humidity, false), 1);

   if (mqtt_online) {
    publish_measurement(temperature, humidity);
   } 

}

///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////// AUXILIARY FUNCTIONS /////////////////////////////
///////////////////////////////////////////////////////////////////////////////////

//
// Publish C string
//
bool publish_c_str(char *topic, char *message, bool retain) {
   // MQTT Available
  if(!iot->mqttActive()) {
    return false;
  } else {
    if (!mqtt->publish(topic, message, retain)) {
      iot->info("Can't publish C string, publish error");
      return false;
    }
  }
  return true;
}

//
// Publish JSON Message
//
bool publish_JSON(char *topic, JsonObject& obj, bool retain) {
  
  // Compute the length of the minified JSON document
  int lenmin = obj.measureLength();
  if (lenmin > DHT11_MAX_JSON_MESSAGE_LEN) {
    return false;
  }

  // Produce a minified JSON document
  char message[DHT11_MAX_JSON_MESSAGE_LEN];
  int real_len = obj.printTo(message);
  if (real_len != lenmin) {
    iot->logPrintf(ERROR, "ArduinoJson printTo length mismatch: %d / %d", real_len, lenmin);  
    return false;
  }

  // Publish
  return publish_c_str(topic, message, retain);
 
}

//
// Publish measured values
//


bool publish_measurement(float temperature, float humidity) {
    
  int len = 0;  
  StaticJsonBuffer<DHT11_JSON_BUFFER_SIZE> jb;
  JsonObject& obj = jb.createObject();

  //
  // Add measurement name, sensor identification and location (app specific data 1 & 2)
  //
  obj["msg_version"] = DHT11_MESSAGE_VERSION;
  obj["measurement"] = DHT11_MEASUREMENT_NAME;
  String n1 = iot->getMQTTSpecific1Name();
  String n2 = iot->getMQTTSpecific2Name();
  obj["sensor_id"] = CLIENT_ID;
  obj["address"] = CLIENT_ADDRESS;
  if (n1 != "")
    obj[n1] = iot->getMQTTAppSpecific1();
  if (n2 != "")
    obj[n2] = iot->getMQTTAppSpecific2(); 

  //
  // Add Timestamps
  //
  time_t tnow = time(NULL);
  obj.set("unix_time", (unsigned long)tnow);
  obj.set("timestamp",ctime(&tnow));
  
  //
  // Add measured data
  //
  obj.set("temperature", temperature);
  if (!obj.set("humidity", humidity)) {
    iot->error("Can't publish message. JSON Buffer too small?");
    return false;
  }
  
  //
  // publish
  //
  return publish_JSON(DHT11_SENSOR_TOPIC, obj, true);
}
