#include<Wire.h>
#include <ESP8266WiFi.h>

// old version, with "delay" (not utilizing deep sleep modes)
// wiring for bme280 from berrybase.de: 3v3->VCC   G->GND   DI->SCL    D2->SDA      (CSB, SDO not connected)

const char* ssid = "WIFI SSID HERE";
const char* password = "WIFI PWD HERE";
 
const char* server = "api.thingspeak.com";
String apiKey = "THINGSPEAK WRITE API KEY HERE";

const char* wl_status_to_string(wl_status_t status) {
  switch (status) {
    case WL_NO_SHIELD: return "WL_NO_SHIELD";
    case WL_IDLE_STATUS: return "WL_IDLE_STATUS";
    case WL_NO_SSID_AVAIL: return "WL_NO_SSID_AVAIL";
    case WL_SCAN_COMPLETED: return "WL_SCAN_COMPLETED";
    case WL_CONNECTED: return "WiFi connected";
    case WL_CONNECT_FAILED: return "WL_CONNECT_FAILED";
    case WL_CONNECTION_LOST: return "WL_CONNECTION_LOST";
    case WL_DISCONNECTED: return "WL_DISCONNECTED";
  }
}


WiFiClient client;

 
// BME280 I2C address is 0x76(108)
#define Addr 0x76
 
void setup()
{
// Initialise I2C communication as MASTER
Wire.begin();
// Initialise Serial communication, set baud rate = 9600
Serial.begin(9600);

WiFi.begin(ssid, password);


}
 
void loop()
{
unsigned int b1[24];
unsigned int data[8];
unsigned int dig_H1 = 0;
for(int i = 0; i < 24; i++)
{
// Start I2C Transmission
Wire.beginTransmission(Addr);
// Select data register
Wire.write((136+i));
// Stop I2C Transmission
Wire.endTransmission();
 
// Request 1 byte of data
Wire.requestFrom(Addr, 1);
 
// Read 24 bytes of data
if(Wire.available() == 1)
{
b1[i] = Wire.read();
}
}
 
// Convert the data
// temp coefficients
unsigned int dig_T1 = (b1[0] & 0xff) + ((b1[1] & 0xff) * 256);
int dig_T2 = b1[2] + (b1[3] * 256);
int dig_T3 = b1[4] + (b1[5] * 256);
 
// pressure coefficients
unsigned int dig_P1 = (b1[6] & 0xff) + ((b1[7] & 0xff ) * 256);
int dig_P2 = b1[8] + (b1[9] * 256);
int dig_P3 = b1[10] + (b1[11] * 256);
int dig_P4 = b1[12] + (b1[13] * 256);
int dig_P5 = b1[14] + (b1[15] * 256);
int dig_P6 = b1[16] + (b1[17] * 256);
int dig_P7 = b1[18] + (b1[19] * 256);
int dig_P8 = b1[20] + (b1[21] * 256);
int dig_P9 = b1[22] + (b1[23] * 256);
 
// Start I2C Transmission
Wire.beginTransmission(Addr);
// Select data register
Wire.write(161);
// Stop I2C Transmission
Wire.endTransmission();
 
// Request 1 byte of data
Wire.requestFrom(Addr, 1);
 
// Read 1 byte of data
if(Wire.available() == 1)
{
dig_H1 = Wire.read();
}
 
for(int i = 0; i < 7; i++)
{
// Start I2C Transmission
Wire.beginTransmission(Addr);
// Select data register
Wire.write((225+i));
// Stop I2C Transmission
Wire.endTransmission();
 
// Request 1 byte of data
Wire.requestFrom(Addr, 1);
 
// Read 7 bytes of data
if(Wire.available() == 1)
{
b1[i] = Wire.read();
}
}
 
// Convert the data
// humidity coefficients
int dig_H2 = b1[0] + (b1[1] * 256);
unsigned int dig_H3 = b1[2] & 0xFF ;
int dig_H4 = (b1[3] * 16) + (b1[4] & 0xF);
int dig_H5 = (b1[4] / 16) + (b1[5] * 16);
int dig_H6 = b1[6];
 
// Start I2C Transmission
Wire.beginTransmission(Addr);
// Select control humidity register
Wire.write(0xF2);
// Humidity over sampling rate = 1
Wire.write(0x01);
// Stop I2C Transmission
Wire.endTransmission();
 
// Start I2C Transmission
Wire.beginTransmission(Addr);
// Select control measurement register
Wire.write(0xF4);
// Normal mode, temp and pressure over sampling rate = 1
Wire.write(0x27);
// Stop I2C Transmission
Wire.endTransmission();
 
// Start I2C Transmission
Wire.beginTransmission(Addr);
// Select config register
Wire.write(0xF5);
// Stand_by time = 1000ms
Wire.write(0xA0);
// Stop I2C Transmission
Wire.endTransmission();
 
for(int i = 0; i < 8; i++)
{
// Start I2C Transmission
Wire.beginTransmission(Addr);
// Select data register
Wire.write((247+i));
// Stop I2C Transmission
Wire.endTransmission();
 
// Request 1 byte of data
Wire.requestFrom(Addr, 1);
 
// Read 8 bytes of data
if(Wire.available() == 1)
{
data[i] = Wire.read();
}
}
 
// Convert pressure and temperature data to 19-bits
long adc_p = (((long)(data[0] & 0xFF) * 65536) + ((long)(data[1] & 0xFF) * 256) + (long)(data[2] & 0xF0)) / 16;
long adc_t = (((long)(data[3] & 0xFF) * 65536) + ((long)(data[4] & 0xFF) * 256) + (long)(data[5] & 0xF0)) / 16;
// Convert the humidity data
long adc_h = ((long)(data[6] & 0xFF) * 256 + (long)(data[7] & 0xFF));
 
// Temperature offset calculations
double var1 = (((double)adc_t) / 16384.0 - ((double)dig_T1) / 1024.0) * ((double)dig_T2);
double var2 = ((((double)adc_t) / 131072.0 - ((double)dig_T1) / 8192.0) *
(((double)adc_t)/131072.0 - ((double)dig_T1)/8192.0)) * ((double)dig_T3);
double t_fine = (long)(var1 + var2);
double cTemp = (var1 + var2) / 5120.0;
double fTemp = cTemp * 1.8 + 32;
 
// Pressure offset calculations
var1 = ((double)t_fine / 2.0) - 64000.0;
var2 = var1 * var1 * ((double)dig_P6) / 32768.0;
var2 = var2 + var1 * ((double)dig_P5) * 2.0;
var2 = (var2 / 4.0) + (((double)dig_P4) * 65536.0);
var1 = (((double) dig_P3) * var1 * var1 / 524288.0 + ((double) dig_P2) * var1) / 524288.0;
var1 = (1.0 + var1 / 32768.0) * ((double)dig_P1);
double p = 1048576.0 - (double)adc_p;
p = (p - (var2 / 4096.0)) * 6250.0 / var1;
var1 = ((double) dig_P9) * p * p / 2147483648.0;
var2 = p * ((double) dig_P8) / 32768.0;
double pressure = (p + (var1 + var2 + ((double)dig_P7)) / 16.0) / 100;
 
// Humidity offset calculations
double var_H = (((double)t_fine) - 76800.0);
var_H = (adc_h - (dig_H4 * 64.0 + dig_H5 / 16384.0 * var_H)) * (dig_H2 / 65536.0 * (1.0 + dig_H6 / 67108864.0 * var_H * (1.0 + dig_H3 / 67108864.0 * var_H)));
double humidity = var_H * (1.0 - dig_H1 * var_H / 524288.0);
if(humidity > 100.0)
{
humidity = 100.0;
}
else if(humidity < 0.0)
{
humidity = 0.0;
}
 
// Output data to serial monitor
Serial.print("Temperature in Celsius : ");
Serial.print(cTemp);
Serial.println(" C");
Serial.print("Temperature in Fahrenheit : ");
Serial.print(fTemp);
Serial.println(" F");
Serial.print("Pressure : ");
Serial.print(pressure);
Serial.println(" hPa");
Serial.print("Relative Humidity : ");
Serial.print(humidity);
Serial.println(" RH");
Serial.print(wl_status_to_string(WiFi.status()));
Serial.print(WiFi.localIP());
Serial.println(" = LAN IP Adress");


if (client.connect(server,80)) {
String postStr = apiKey;
postStr +="&field1=";
postStr += String(cTemp);
postStr +="&field2=";
postStr += String(humidity);
postStr +="&field3=";
postStr += String(pressure);
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
delay(20000);
}
