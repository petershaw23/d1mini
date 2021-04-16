#include <DHT.h>
#include <ESP8266WiFi.h>
 
// replace with your channels thingspeak API key and your SSID and password
String apiKey = "XXXXXX";
const char* ssid = "Bud!Box2ghz";
const char* password = "XXXX";
const char* server = "api.thingspeak.com";
const int sleepSeconds = 300;
 
#define DHTPIN D2
#define DHTTYPE DHT22 
DHT dht(DHTPIN, DHTTYPE);
WiFiClient client;
 
void setup() 
{
Serial.begin(115200);
delay(10);
dht.begin();
 
WiFi.begin(ssid, password);
 
Serial.println();
Serial.println();
Serial.print("Connecting to ");
Serial.println(ssid);
 
WiFi.begin(ssid, password);
 
while (WiFi.status() != WL_CONNECTED) 
{
delay(500);
Serial.print(".");
}
Serial.println("");
Serial.println("WiFi connected");
 


int sensorValue = analogRead(A0);
float voltage = sensorValue / 196.5;
Serial.print("Voltage: ");
Serial.print(voltage);
Serial.print(" | ");
Serial.print("Sensor: ");
Serial.println(sensorValue);

 
float h = dht.readHumidity();
float t = dht.readTemperature();
if (isnan(h) || isnan(t)) 
{
Serial.println("Failed to read from DHT sensor!");
return;
}
 
if (client.connect(server,80)) {
String postStr = apiKey;
postStr +="&field1=";
postStr += String(t);
postStr +="&field2=";
postStr += String(h);
postStr +="&field3=";
postStr += String(voltage);
postStr += "\r\n\r\n\r\n";
 
client.print("POST /update HTTP/1.1\n");
client.print("Host: api.thingspeak.com\n");
client.print("Connection: close\n");
client.print("X-THINGSPEAKAPIKEY: "+apiKey+"\n");
client.print("Content-Type: application/x-www-form-urlencoded\n");
client.print("Content-Length: ");
client.print(postStr.length());
client.print("\n\n");
client.print(postStr);
 
Serial.print("Temperature: ");
Serial.print(t);
Serial.print(" degrees Celsius Humidity: ");
Serial.print(h);
Serial.println("Sending data to Thingspeak");
}
client.stop();
 


// delay(20000);
ESP.deepSleep(sleepSeconds * 1000000);
}


void loop() 
{
}
