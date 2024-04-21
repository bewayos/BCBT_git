#include <dht11.h>

// Define sensor pins for each group
#define DHT11PIN1 7
#define DHT11PIN2 8
#define DHT11PIN3 9
#define PHOTOPIN1 A2
#define PHOTOPIN2 A3
#define PHOTOPIN3 A4

// Sensor instances
dht11 DHT11_1;
dht11 DHT11_2;
dht11 DHT11_3;

void setup() {
  Serial.begin(9600);
}

void loop() {
  Serial.println();

  // Read data from all sensors in group 1
  int chk1 = DHT11_1.read(DHT11PIN1);
  int lightLevel1 = analogRead(PHOTOPIN1);
  printSensorData(1, DHT11_1.humidity, DHT11_1.temperature, lightLevel1);

  // Read data from all sensors in group 2
  int chk2 = DHT11_2.read(DHT11PIN2);
  int lightLevel2 = analogRead(PHOTOPIN2);
  printSensorData(2, DHT11_2.humidity, DHT11_2.temperature, lightLevel2);

  // Read data from all sensors in group 3
  int chk3 = DHT11_3.read(DHT11PIN3);
  int lightLevel3 = analogRead(PHOTOPIN3);
  printSensorData(3, DHT11_3.humidity, DHT11_3.temperature, lightLevel3);

  delay(2000);
}

void printSensorData(int group, int humidity, int temperature, int lightLevel) {
  Serial.print("Group ");
  Serial.print(group);
  Serial.print(" - Humidity (%): ");
  Serial.println((float)humidity, 2);
  Serial.print("Group ");
  Serial.print(group);
  Serial.print(" - Temperature  (C): ");
  Serial.println((float)temperature, 2);
  Serial.print("Group ");
  Serial.print(group);
  Serial.print(" - Light Level: ");
  Serial.println(lightLevel);
}
