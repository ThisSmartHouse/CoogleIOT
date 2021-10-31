This example shows ho to use CooglIOT to send the readings from a sensor to a MQTT broker. The sensor used in
 the example is a (very unexpensive) DHT11 temperature/humidity sensor, but can be easily adapted to use 
 any other sensor. 

The example demonstrates the use of custom application specific fields and `loopWebServer()` to handle
unsuccessfull startups due to wrong configuration parameters. 
