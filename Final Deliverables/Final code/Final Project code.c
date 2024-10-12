#include <WiFi.h>//library for wifi
#include <PubSubClient.h>//library for MQtt
#include <WiFi.h>//library for wifi
#include <PubSubClient.h>//library for MQtt
#include "DHT.h"// Library for dht11
#define DHTPIN 32 // what pin we're connected to
#define DHTTYPE DHT22 // define type of sensor DHT 11
const int ldrpin =34;
#define Pir 2
const float gama = 0.7;
const float rl10 = 50;
int led = 4;
int LED=15;
int amber=23;
int statusPir = 0;
DHT dht (DHTPIN, DHTTYPE);
void callback(char* subscribetopic, byte* payload, unsigned int payloadLength);
//-------credentials of IBM Accounts------
#define ORG "etuanl"//IBM ORGANITION ID
#define DEVICE_TYPE "streetlight"//Device type mentioned in ibm watson IOT Platform
#define DEVICE_ID "22123"//Device ID mentioned in ibm watson IOT Platform
#define TOKEN "22122322"//Token
String data3;
float h, t;
//-------- Customise the above values --------
char server[] = ORG ".messaging.internetofthings.ibmcloud.com";// Server Name
char publishTopic[] = "iot-2/evt/Data/fmt/json";// topic name and type of event perform
and format in which data to be send
char subscribetopic[] = "iot-2/cmd/command/fmt/String";// cmd REPRESENT command type
AND COMMAND IS TEST OF FORMAT STRING
char authMethod[] = "use-token-auth";// authentication method
char token[] = TOKEN;
char clientId[] = "d:" ORG ":" DEVICE_TYPE ":" DEVICE_ID;//client id
// 
WiFiClient wifiClient; // creating the instance for wificlient
PubSubClient client(server, 1883, callback ,wifiClient); //calling the predefined client
id by passing parameter like server id,portand wificredential
void setup()// configureing the ESP32
{
Serial.begin(115200);
pinMode(Pir, INPUT);
pinMode(ldrpin, INPUT);
pinMode(led, OUTPUT);
pinMode(LED, OUTPUT);
pinMode(amber, OUTPUT);
dht.begin();
wificonnect();
mqttconnect();
}
void loop()// Recursive Function
{
delay(10);
h = dht.readHumidity();
t = dht.readTemperature();
Serial.print("temp:");
Serial.println(t);
Serial.print("Humid:");
Serial.println(h);
statusPir = digitalRead(Pir);
int analogvalue = analogRead(ldrpin);
analogvalue = map(analogvalue, 4095, 0, 1024, 0);
float voltage = analogvalue / 1024.*5;
float resistance = 2000 * voltage / (1-voltage/5);
float luxv = pow(50*1e3*pow(10,0.7)/resistance,(1/0.7));
delay(10);
Serial.print("lux = ");
Serial.println(luxv);
if(statusPir==HIGH)
{
if(h<=70)
{
if(luxv<400)
{
digitalWrite(4, HIGH);
digitalWrite(15, HIGH);
delay(6000);
digitalWrite(4, LOW);
digitalWrite(15, LOW);
}
else if((luxv>400)&&(luxv<=1600))
{
digitalWrite(15, HIGH);
delay(6000);
digitalWrite(15, LOW);
}
}
else{
digitalWrite(23, HIGH);
delay(6000);
digitalWrite(23, LOW);
}
}
PublishData(t,h,luxv);
delay(1000);
if (!client.loop()) {
mqttconnect();
}
}
/*.....................................retrieving to
Cloud. ............................. */
void PublishData(float temp, float humid,float lux) {
mqttconnect();//function call for connecting to ibm
/*
creating the String in in form JSon to update the data to ibm cloud
*/
String payload = "{\"temp\":";
payload += temp;
payload += "," "\"Humid\":";
payload += humid;
payload += "," "\"luminance\":";
payload += lux;
payload += "}";
Serial.print("Sending payload: ");
Serial.println(payload);
if (client.publish(publishTopic, (char*) payload.c_str())) {
Serial.println("Publish ok");// if it sucessfully upload data on the cloud then it
will print publish ok in Serial monitor or else it will print publish failed
} else {
Serial.println("Publish failed");
}
}
void mqttconnect() {
if (!client.connected()) {
Serial.print("Reconnecting client to ");
Serial.println(server);
while (!!!client.connect(clientId, authMethod, token)) {
Serial.print(".");
delay(500);
}
initManagedDevice();
Serial.println();
}
}
void wificonnect() //function defination for wificonnect
{
Serial.println();
Serial.print("Connecting to ");
WiFi.begin("Wokwi-GUEST", "", 6);//passing the wifi credentials to establish the
connection
while (WiFi.status() != WL_CONNECTED) {
delay(500);
Serial.print(".");
}
Serial.println("");
Serial.println("WiFi connected");
Serial.println("IP address: ");
Serial.println(WiFi.localIP());
}
void initManagedDevice() {
if (client.subscribe(subscribetopic)) {
Serial.println((subscribetopic));
Serial.println("subscribe to cmd OK");
} else {
Serial.println("subscribe to cmd FAILED");
}
}
void callback(char* subscribetopic, byte* payload, unsigned int payloadLength)
{
Serial.print("callback invoked for topic: ");
Serial.println(subscribetopic);
for (int i = 0; i < payloadLength; i++) {
//Serial.print((char)payload[i]);
data3 += (char)payload[i];
}
Serial.println("data: "+ data3);
if(data3=="lighton")
{
Serial.println(data3);
}
else
{
Serial.println(data3);
}
data3="";
}
