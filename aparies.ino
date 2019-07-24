#include <dht.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <ESP8266WebServer.h>
dht DHT;

#define DHT11_PIN D7
char ssid[]="OPPO F7";
char pass[]="kunal$$2000";
const char *host = "api.thingspeak.com";
String apiKey = "Q0A29FWN6G5XL3QH";
int measurePin = A0;

unsigned int samplingTime = 280;
unsigned int deltaTime = 40;
unsigned int sleepTime = 9680;

float voMeasured = 0;
float calcVoltage = 0;
float dustDensity = 0;

void setup(){
  Serial.begin(9600);
  //pinMode(ledPower,OUTPUT);
  pinMode(D0,OUTPUT);
  pinMode(D1,OUTPUT);
  pinMode(D2,OUTPUT);
  pinMode(D7,INPUT);
  pinMode(A0,INPUT);
    WiFi.disconnect();
  WiFi.begin(ssid,pass);
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop()
{ WiFiClient client;          
  const int httpPort = 80; 
 if(!client.connect(host, httpPort)){
   Serial.println("Connection Failed");
   delay(300);
   return; 
 }
  float t=DHT.temperature;
  float h=DHT.humidity;
  int chk = DHT.read11(DHT11_PIN);
  Serial.print("Temperature = ");
  Serial.println(DHT.temperature);
  Serial.print("Humidity = ");
  Serial.println(DHT.humidity);


  digitalWrite(D0,HIGH);
  delay(500);
  float mq9sensor_volt = 0;
  float mq9R0 = 0.42;//0.72
  float mq9RS_gas = 0; // Get value of RS in a GAS
  float mq9ratio = 0; // Get ratio RS_GAS/RS_air
  double mq9ppm = 0;
  int mq9sensorValue = analogRead(A0);
  mq9sensor_volt=(float)mq9sensorValue/1024*5.0;
  mq9RS_gas = (5.0-mq9sensor_volt)/mq9sensor_volt; // omit *RL
 
  //-Replace the name "R0" with the value of R0 in the demo of First Test -/
  mq9ratio = mq9RS_gas/mq9R0;  // ratio = RS/R0 
  //-----------------------------------------------------------------------/
 
  Serial.print("sensor_volt = ");
  Serial.println(mq9sensor_volt);
  Serial.print("RS_ratio = ");
  Serial.println(mq9RS_gas);
  Serial.print("Rs/R0 = ");
  Serial.println(mq9ratio);
  mq9ppm = pow(10,((log10(mq9ratio)-1.284)/(-0.459)));
  Serial.print("CO concentration(ppm) = ");
  Serial.println(mq9ppm);
 
  Serial.print("\n\n");
  digitalWrite(D0,LOW);
  delay(500);


  digitalWrite(D1,HIGH);
  delay(500);
  float mq135sensor_volt = 0;
  float mq135RS_gas = 0; // Get value of RS in a GAS
  float mq135ratio = 0; // Get ratio RS_GAS/RS_air
  double mq135ppm = 0;
  float mq135R0 = 2.47;
  int mq135sensorValue = analogRead(A0);
  mq135sensor_volt=(float)mq135sensorValue/1024*5.0;
  mq135RS_gas = (5.0-mq135sensor_volt)/mq135sensor_volt; // omit *RL
 
 //-Replace the name "R0" with the value of R0 in the demo of First Test -/
  mq135ratio = mq135RS_gas/mq135R0;  // ratio = RS/R0 
  //-----------------------------------------------------------------------/
 
  Serial.print("sensor_volt = ");
  Serial.println(mq135sensor_volt);
  Serial.print("RS_ratio = ");
  Serial.println(mq135RS_gas);
  Serial.print("Rs/R0 = ");
  Serial.println(mq135ratio);
  mq135ppm = pow(10,(log10(mq135ratio)-0.96)/(-0.459));
  Serial.print("CO2 concentration(ppm) = ");
  Serial.println(mq135ppm);
  digitalWrite(D0,LOW);
  delay(500);
  Serial.print("\n\n");


  digitalWrite(D2,HIGH);
  delay(500);
  //digitalWrite(ledPower,LOW);
  delayMicroseconds(samplingTime);

  voMeasured = analogRead(measurePin);

  delayMicroseconds(deltaTime);
  //digitalWrite(ledPower,HIGH);
  delayMicroseconds(sleepTime);

  calcVoltage = voMeasured*(5.0/1024);
  dustDensity = (calcVoltage-0.9)/6.5;

  if ( dustDensity < 0)
  {
    dustDensity = 0.00;
  }

  Serial.print("Raw Signal Value (0-1023):");
  Serial.println(voMeasured);

  Serial.print("Voltage:");
  Serial.println(calcVoltage);

  Serial.print("Dust Density:");
  Serial.println(dustDensity);
  digitalWrite(D2,LOW);
  delay(500);
  Serial.println("\n\n");
   String postStr = apiKey;
    postStr +="&field1=";
    postStr += (String)(t);
    postStr +="&field2=";
    postStr += (String)(h);
    postStr+="&field3="+(String)mq135ppm;
    postStr+="&field4="+(String)mq9ppm;
    postStr+="&field5="+(String)dustDensity;
    postStr += "\r\n\r\n";
    client.print("POST /update HTTP/1.1\n");
    client.print("Host: api.thingspeak.com\n");
    client.print("Connection: close\n");
    client.print("X-THINGSPEAKAPIKEY: "+apiKey+"\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(postStr.length());
    client.print("\n\n");
    client.print(postStr);
 
  int timeout=0;
 while((!client.available()) && (timeout < 1000))     //Wait 5 seconds for data
 {
   delay(10);  //Use this with time out
   timeout++;
 }
 
//---------------------------------------------------------------------
 //If data is available before time out read it.
 if(timeout < 500)
 {
     while(client.available()){
        Serial.println(client.readString()); //Response from ThingSpeak       
     }
 }
 else
 {
     Serial.println("Request timeout..");
 }
client.stop();
delay(7000);
  delay(1000);
}
