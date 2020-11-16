#include <MsTimer2.h>
#include <SoftwareSerial.h>
#include <Arduino.h>
#include <dht11.h>
#define DHT11PIN A0
#define Sensor_AO A1                                  
#define Sensor_DO 2
#define led 7
char comdata[10] = {0};
unsigned int sensorValue = 0;
dht11 DHT11;
char msg[20];
char temp[10];
char humi[10];

void setup()
{
  pinMode(Sensor_DO, INPUT);
  pinMode(led,OUTPUT);
  Serial.begin(9600);
  digitalWrite(led,LOW);
  MsTimer2::set(50, LED_action);  
  MsTimer2::start();
}
void loop()
{
  sensorValue = analogRead(Sensor_AO);
 //Serial.print("Harmful gas concentration: ");
//Serial.println(sensorValue);
  int chk = DHT11.read(DHT11PIN);
 //Serial.print("Humidity (%): ");
//湿度读取
 //Serial.println((float)DHT11.humidity);
//温度读取
  //Serial.print("Temperature (oC): ");
  //Serial.println((float)DHT11.temperature);
  //Serial.print("\n");
  
      
  memset(msg, 0, 20);
  memset(temp, 0, 10);
  memset(humi, 0, 10);
  
  dtostrf((float)DHT11.humidity, 4, 2, humi);
  dtostrf((float)DHT11.temperature, 4, 2, temp);

 
  if(sensorValue > 500)
    sprintf(msg, "%s#%s#a", humi, temp); //大于500,发送a
  else
    sprintf(msg, "%s#%s#b", humi, temp); //小于500,发送b
  
  Serial.print(msg);

  delay(2000); 
}

int i = 0;
int len = 0;
void LED_action()
{
  if(Serial.available() <= 0)
    return;
  i = 0;
  while(Serial.available() > 0)
  {
    comdata[i] = Serial.read();
    i++;
  }  
  if(comdata[0] == 'o' && comdata[1] == 'n')
  {
    digitalWrite(led, HIGH);
  }
  if(comdata[0] == 'o' && comdata[1] == 'f')
  {
    digitalWrite(led, LOW);
  }
}

  
