/*
Project:  BME280 Sensor data send over MQTT with a ESP8266 / NodeMCU with Deep-Sleep
Author:   Thomas Edlinger for www.edistechlab.com
Date:     Created 10.04.2020
Version:  V1.0
Arduino IDE V1.8.12 
Required libraries (Tools -> manage libraries)
 - PubSubClient by Nick O'Leary V2.7.0  
 - Adafruit BME280 Library V2.0.1
 - Adafruit Unified Sensor V1.1.2
Required Board (Tools -> Board -> Boards Manager...)
 - Board: esp8266 by ESP8266 Community V2.6.3
Wirering:
Bridge RST with GPIO16 (D0) - After the sketch is uploaded
BME280      NodeMCU
VCC         3.3V
GND         G
SCL         D1 / GPIO5
SDA         D2 / GPIO4
*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
  
            
                                       
//WIFI & MQTT:  
#define wifi_ssid "FRITZ!Box 7530 PA"
#define wifi_password "38579552509429597877"
#define mqtt_server "10.0.0.1"
#define mqtt_user "User1"         
#define mqtt_password ""     
#define temp1_topic  "arduino/temp1"
#define temp2_topic  "arduino/temp2"
#define returndatum_topic  "arduino/returndatum"   
#define MQTTupdate 60 //sek

#define ONE_WIRE_BUS 0
#define Anzahl_Sensoren_DS18B20 2 // Anzahl der angeschlossenen Sensoren - Mögliche Werte: '0','1','2',... 
#include <OneWire.h> 
#include <DallasTemperature.h>
#define DS18B20_Aufloesung 12
DeviceAddress DS18B20_Adressen;

OneWire oneWire(ONE_WIRE_BUS); 
DallasTemperature myDS18B20(&oneWire);  

WiFiClient espClient;  
PubSubClient client(espClient);  
//DHT dht(DHTPIN, DHTTYPE);   

double alt=0;
float calibration_value = -0.50;
int phval = 0; 
unsigned long int avgval; 
int buffer_arr[10],temp;
double ph_act=0, volt;
char datum[26],wert[2];
int a=0;
String strDatum="", strdate="";
float Temperatur[2]; 
unsigned long previousTime=0, lastResetTime=0;
const unsigned long RESET_INTERVAL = 30 * 60 * 1000; //30min

void callback(char* topic, byte* payload, unsigned int length) {

  Serial.print("Empfangen: ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  a=1;
 
  if(topic[8]=='d') {
      datum[0]='\0';
      for (int i=0;i<length;i++) {
        datum[i]=payload[i];
      }     
      strDatum="";
      for(int j=0;j<13; j++) {
          strDatum=strDatum+datum[j];
          
          //int integer=strDatum.toInt();
          //wert[j] = char(integer); 
         
      }

   }
     
}


void setup() {  
  
  Serial.begin(115200);   

  if ((Anzahl_Sensoren_DS18B20 > 0)) {
    myDS18B20.begin();
    Serial.print("Anzahl angeschlossener Sensoren: ");
    Serial.println(myDS18B20.getDeviceCount(), DEC);
    Serial.println();
 
    for(byte i=0 ;i < myDS18B20.getDeviceCount(); i++) {
      if(myDS18B20.getAddress(DS18B20_Adressen, i)) {
        myDS18B20.setResolution(DS18B20_Adressen, DS18B20_Aufloesung);
      }
    }
  }
  
  setup_wifi();  
  client.setServer(mqtt_server, 1883); 
  client.setCallback(callback);
  
  if (!client.connected()) {  
    
    reconnect();  
  }  
  
   
  
  //dht.begin();
  //Serial.println("ESP01 geht in deep sleep");  
  //ESP.deepSleep(durationSleep * 1e6);
  //delay(1000); 
  while(a==0) {
    client.loop(); 
  }
  
  myDS18B20.requestTemperatures();
            
    Temperatur[0] = myDS18B20.getTempCByIndex(0);
    delay(10);
    Temperatur[1] = myDS18B20.getTempCByIndex(1);
    delay(10);
    
    Serial.print(Temperatur[0]);
    Serial.println(" 'C");
    Serial.print(Temperatur[1]);
    Serial.println(" 'C");
    Serial.print("strDatum: ");
    Serial.println(strDatum);
  
    client.publish(temp1_topic, String(Temperatur[0]).c_str(), true); 
    delay(10);
    client.publish(temp2_topic, String(Temperatur[1]).c_str(), true); 
    delay(10);
    
    client.publish(returndatum_topic, strDatum.c_str(), true); 
    delay(10);
 }  


   
void loop() {  

  client.loop();

  unsigned long currentTime=millis();
  

  if(currentTime - previousTime >=600000) {

    myDS18B20.requestTemperatures();
            
    Temperatur[0] = myDS18B20.getTempCByIndex(0);
    delay(10);
    Temperatur[1] = myDS18B20.getTempCByIndex(1);
    delay(10);
    
    Serial.print(Temperatur[0]);
    Serial.println(" 'C");
    Serial.print(Temperatur[1]);
    Serial.println(" 'C");
    
    Serial.print("strDatum: ");
    Serial.println(strDatum);
  
    client.publish(temp1_topic, String(Temperatur[0]).c_str(), true); 
    delay(10);
    client.publish(temp2_topic, String(Temperatur[1]).c_str(), true); 
    delay(10);
    client.publish(returndatum_topic, strDatum.c_str(), true); 
    delay(10);
    previousTime=currentTime;
  }

  if (millis() >= RESET_INTERVAL) {
    Serial.println("Reset wird durchgeführt...");
    delay(1000);
    ESP.restart();  // Reset durchführen
  }
  
  

}  
   
void setup_wifi() {  
  delay(10);  
  // We start by connecting to a WiFi network  
  Serial.println();  
  Serial.print("Connecting to ");  
  Serial.println(wifi_ssid);  
  int _try = 0;
  WiFi.begin(wifi_ssid, wifi_password);  
  while (WiFi.status() != WL_CONNECTED) {  
     Serial.print("."); 
     delay(500);  
     _try++;      // Wenn nach 10 Versuchen nicht mit WiFi verbunden werden kann, deep-sleep
     if ( _try >= 10 ) {
          Serial.println("Kann nicht mit WiFi verbunden werden, gehe in deep-sleep");
          break;
          //ESP.deepSleep(durationSleep * 1e6);
      } 
  }  
  Serial.begin(115200); 
  Serial.println("");  
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");  
  Serial.println(WiFi.localIP());  
 }  
   
 void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str(), mqtt_user, mqtt_password)) {
      Serial.println("connected");
      Serial.println("Subscribe arduino/datum");
      client.subscribe("arduino/datum");
      // Once connected, publish an announcement...
      //client.publish(status_topic, "ESP01 alive");
    } else {
        Serial.print("failed, rc=");
        Serial.print(client.state());
        Serial.println(" try again in 5 seconds");
        // Wait 5 seconds before retrying
        delay(5000);
      }
   }
}
