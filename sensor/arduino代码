#include <SoftwareSerial.h>
#include <Arduino.h>
#include <dht11.h>
#define DHT11PIN A0
#define Sensor_AO A1
#define Sensor_DO 2
#define led 7
SoftwareSerial mySerial(10,11); // TX, RX
unsigned int sensorValue = 0;
dht11 DHT11;
typedef struct message
{
  float tem;
  float hum;
  double ser;
}msg;
void setup()
{
  pinMode(Sensor_DO, INPUT);
  Serial.begin(9600);
}
void loop()
{
  sensorValue = analogRead(Sensor_AO);
  double ppm;
  ppm = pow(11.5428 * 35.904 * sensorValue/(25.5-5.1* sensorValue),0.6549);
  Serial.println((double)ppm, 2);
  Serial.print("Harmful gas concentration(ppm): ");
  Serial.println(ppm);
  if (ppm > 13)
  {  
    digitalWrite(led, HIGH);
  }
  int chk = DHT11.read(DHT11PIN);
  Serial.print("Humidity (%): ");
//湿度读取
  Serial.println((float)DHT11.humidity, 2);
//温度读取
  Serial.print("Temperature (oC): ");
  Serial.println((float)DHT11.temperature, 2);
  Serial.print("\n");
  if(DHT11.humidity < 50 || DHT11.temperature > 26)
  {
    digitalWrite(led, HIGH);
  }
  msg m;
  m.tem = DHT11.temperature;
  m.hum = DHT11.humidity;
  m.ser = sensorValue;
  byte *tobyte = (byte*)&m;
  while(mySerial.available())
  {
    Serial.write(tobyte,sizeof(m));
  }
  delay(3000);
}
//ppm = pow(11.5428 * 35.904 * sensorValue/(25.5-5.1* sensorValue),0.6549);