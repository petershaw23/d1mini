/**********************************************
 * Catalin Batrinu bcatalin@gmail.com 
 * Read temperature and pressure from BMP280
 * and send it to thingspeaks.com
**********************************************/

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <ESP8266WiFi.h>
#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_BME280 bme; // I2C
// replace with your channel’s thingspeak API key,
String apiKey = "UUIR9GYBLABLALBA";
const int sleepSeconds = 300;
const char* ssid = "ssid";
const char* password = "pswsdasd";
const char* server = "api.thingspeak.com";
WiFiClient client;


/**************************  
 *   S E T U P
 **************************/
void setup() {
  Serial.begin(9600);
  
  if (!bme.begin(0x76)) {  
    Serial.println("Could not find a valid BMP280 sensor, check wiring!");
    while (1);
  }
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

  Serial.print("T=");
  Serial.print(bme.readTemperature());
  Serial.print(" *C");

  if (client.connect(server,80))
  {
    String postStr = apiKey;
    postStr +="&field3=";
    postStr += String(bme.readTemperature());
    postStr +="&field5=";
    postStr += String(bme.readHumidity());
    postStr +="&field7=";
    postStr += String(bme.readPressure());
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
    }
    client.stop(); 
    // deep sleep here
    Serial.printf("Sleep for %d seconds\n\n", sleepSeconds);
    ESP.deepSleep(sleepSeconds * 1000000);

}

  /**************************  
 *  L O O P
 **************************/
void loop() {
// nothing here    
}
