#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <EEPROM.h>
#define ONE_WIRE_BUS 0
#define Anzahl_Sensoren_DS18B20 2 // Anzahl der angeschlossenen Sensoren - Mögliche Werte: '0','1','2',... 
#include <OneWire.h> 
#include <DallasTemperature.h>
#define DS18B20_Aufloesung 12
DeviceAddress DS18B20_Adressen;

OneWire oneWire(ONE_WIRE_BUS); 
DallasTemperature myDS18B20(&oneWire);             

#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_WIDTH 128 // OLED display Breite, in pixels
#define SCREEN_HEIGHT 64 // OLED display Höhe, in pixels
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);  
                                       
//WIFI & MQTT:  
#define wifi_ssid "A1-2c10f1"
#define wifi_password "h0fxdic5qfsu"
#define mqtt_server "10.0.0.1"
#define mqtt_user "User1"         
#define mqtt_password ""     
#define ph_topic  "arduino/ph"
#define temp1_topic  "arduino/temp1"
#define temp2_topic  "arduino/temp2"
#define returndatum_topic  "arduino/returndatum"
#define MQTTupdate 60 //sek

//#define durationSleep  3 //900   // Sekunden - 15min.

WiFiClient espClient;  
PubSubClient client(espClient);  
//DHT dht(DHTPIN, DHTTYPE);   

double alt=0;
float calibration_value = -0.50;
int phval = 0; 
unsigned long int avgval; 
int buffer_arr[10],temp;
double ph_act=0, volt;
float Temperatur[2]; 
byte set_on_payload[2];
byte set_off_payload[2];
char datum[26],wert[2];

bool flag=0;
unsigned long vergangeneZeit=0, previousMillis=0;
String strDatum="", strdate="";
//char date;

void callback(char* topic, byte* payload, unsigned int length) {
  //Serial.print("Message arrived [");
  Serial.print(topic);
  display.setTextSize(1);
  display.setCursor(98, 7);
  
  if(topic[13]=='n') {
    set_on_payload[0]=' ';
    set_on_payload[1]=' ';
    EEPROM.write(1, ' ');
    for (int i=0;i<length;i++) {
      set_on_payload[i]=payload[i];
      EEPROM.write(i, char(set_on_payload[i]));  
      //Serial.print((char)payload[i]);
      //display.print((char)payload[i]);
      //display.display();
    }
  }
  if(topic[13]=='f') {
    set_off_payload[0]=' ';
    set_off_payload[1]=' ';
    EEPROM.write(0x0F+1, ' ');
    for (int i=0;i<length;i++) {
      set_off_payload[i]=payload[i];
      EEPROM.write(0x0F+i, char(set_off_payload[i]));  
      //Serial.print((char)payload[i]);
      //display.print((char)payload[i]);
      //display.display();
    }
  } 
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
      Serial.println("Datum:"); 
      Serial.print(strDatum); 
      Serial.println(); 
   }

  EEPROM.commit();
     
}

    
void setup() {  
  
  Serial.begin(115200);   
  EEPROM.begin(512);  //Initialize EEPROM

  for(int i=0;i<2;i++){
    set_on_payload[i]=char(EEPROM.read(i));
    delay(100);
    set_off_payload[i]=char(EEPROM.read(0x0F+i));
    delay(100);
  }
    
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

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); // I2C address = 0x3C
  delay(500);
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("Connecting to ");
  display.println(wifi_ssid);
  display.display();
  delay(100);

  setup_wifi();  
  client.loop();

  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.print("Welcome to");
  display.setCursor(18, 22);
  display.print("Reichi's");
  display.setCursor(40, 50);
  display.print("Pool");
  display.display(); 
  delay(1500);
  display.clearDisplay();

 }   //end setup


   
void loop() {  

  alt=ph_act;
  for(int i=0;i<10;i++) 
  { 
     buffer_arr[i]=analogRead(A0);
     delay(30);
  }
  for(int i=0;i<9;i++)
  {
     for(int j=i+1;j<10;j++)
     {
       if(buffer_arr[i]>buffer_arr[j])
       {
         temp=buffer_arr[i];
         buffer_arr[i]=buffer_arr[j];
         buffer_arr[j]=temp;
       }
     }
  }
  avgval=0;
  for(int i=2;i<8;i++)
    avgval+=buffer_arr[i];
  
  float volt=(float)avgval*3.3/1024/6; 
  ph_act = 5.70 * volt + calibration_value;

  Serial.print("pH Val: ");
  Serial.println(ph_act);

  myDS18B20.requestTemperatures();
          
  Temperatur[0] = myDS18B20.getTempCByIndex(0);
  delay(10);
  Temperatur[1] = myDS18B20.getTempCByIndex(1);
  delay(10);
  
  Serial.print(Temperatur[0]);
  Serial.println(" 'C");
  Serial.print(Temperatur[1]);
  Serial.println(" 'C");

    client.publish(ph_topic, String(ph_act).c_str(), true); 
    delay(10);
    client.publish(temp1_topic, String(Temperatur[0]).c_str(), true); 
    delay(10);
    client.publish(temp2_topic, String(Temperatur[1]).c_str(), true); 
    delay(10);

    client.publish(returndatum_topic, strDatum.c_str(), true); 
    delay(10);

  if(flag==0){
    
    WiFi.disconnect();
    flag=1;
  }

  
  vergangeneZeit = millis() - previousMillis;
  

  Serial.println(previousMillis);
  Serial.println(vergangeneZeit);
  
  if(vergangeneZeit>(MQTTupdate*1e3)){
    flag=0;
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    display.println("Connecting to ");
    display.println(wifi_ssid);
    display.display();
    delay(1000);
    setup_wifi();
    Serial.print(vergangeneZeit);
    previousMillis = millis();
    
  }
   


  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(0, 0);
  display.print("pH:");
  display.setTextSize(5);
  display.setCursor(20, 23);
  display.println(ph_act,1);
  display.setTextSize(1);
  display.setCursor(115,0);
  for (int i=0;i<2;i++) {
      display.print((char)set_on_payload[i]);
  }
  display.print(" ");
  display.setCursor(115,8);
  for (int i=0;i<2;i++) {
      display.print((char)set_off_payload[i]);
  }  
  display.print(" ");
  display.display();
  
  for(int j=0; j<35; j++) {
    client.loop();
    delay(100);
  }
  
  
  

  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(0, 0);
  display.print("Pool(");
  display.print((char)247);
  display.print("C)");
  display.setTextSize(5);
  display.setCursor(5, 23);
  display.print(Temperatur[0],1);
  display.setTextSize(1);
  display.setCursor(115,0);
  for (int i=0;i<2;i++) {
      display.print((char)set_on_payload[i]);
  }
  display.print(" ");
  display.setCursor(115,8);
  for (int i=0;i<2;i++) {
      display.print((char)set_off_payload[i]);
  }  
  display.print(" ");
  display.display();
  for(int j=0; j<35; j++) {
    client.loop();
    delay(100);
  }

  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(0, 0);
  display.print("Solar(");
  display.print((char)247);
  display.print("C)");
  display.setTextSize(5);
  display.setCursor(5, 23);
  display.print(Temperatur[1],1);
  display.setTextSize(1);
  display.setCursor(115,0);
  for (int i=0;i<2;i++) {
      
      display.print((char)set_on_payload[i]);
  }
  display.print(" ");
  display.setCursor(115,8);
  for (int i=0;i<2;i++) {
      display.print((char)set_off_payload[i]);
  }  
  display.print(" ");
  display.display();
  for(int j=0; j<35; j++) {
    client.loop();
    delay(100);
  }
  
  /*display.setTextSize(1);
  display.setCursor(80, 7);
  display.setCursor(74, 18);

*/
  

    
}  
   
void setup_wifi() {  
  delay(10);  
  // We start by connecting to a WiFi network  
  Serial.println();  
  Serial.print("Connecting to ");  
  Serial.println(wifi_ssid);  
    
  int _try = 0, a=0, b=0;
  WiFi.begin(wifi_ssid, wifi_password);  

  
  while (WiFi.status() != WL_CONNECTED) {  
     Serial.print("."); 
     display.print(".");
     display.display();
     delay(500);  
     _try++;      
     if ( _try >= 20 ) {
          a=1;
          break;
          //ESP.deepSleep(durationSleep * 1e6);
      } 
  }
    
  if (a==0) {
      Serial.println("");  
      Serial.println("Connected!");  
      Serial.println("IP: ");  
      Serial.println(WiFi.localIP());  
      
      display.clearDisplay();
      display.setTextSize(2);
      display.setCursor(0, 0);
      display.println("Connected!");
      display.println("IP: ");
      display.println(WiFi.localIP());
      display.display();
      delay(1500);
      
      client.setServer(mqtt_server, 1883);  
      client.setCallback(callback);
            
      display.clearDisplay();
      display.setCursor(0, 0);
      display.println("Connecting\n"); 
      display.setCursor(20, 25);
      display.setTextSize(3);
      display.print("MQTT");
      display.display();
      _try=0;
      display.setTextSize(1);
      display.setCursor(0, 50);
      while(!client.connected()) {  
           reconnect();  
          
          _try++;      
         
           if ( _try >= 20 ) {
                break;
                b=1;
          } 
      }  
       
      for(int j=0; j<150; j++) {
          client.loop();
          delay(10);
      }
      

      display.clearDisplay();
      display.setTextSize(2);
      display.setCursor(0, 0);
          
      if (b==0) {  
          display.println("MQTT\nConnected!");
      }

      if (b==1) {  
          display.println("MQTT\nError!");
      }
      display.display();
      
      delay(1500); 
   }
   if (a==1) {
      Serial.println("");  
      Serial.println("NO WIFI!");  
      Serial.println(WiFi.localIP());  
      
      display.clearDisplay();
      display.setTextSize(2);
      display.setCursor(0, 0);
      display.println("NO WIFI!");
      display.println(WiFi.localIP());
      display.display();
      client.loop(); 
      delay(1500); 
   }
  
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
      client.subscribe("arduino/set_on");
      client.subscribe("arduino/set_off");
      client.subscribe("arduino/datum");
      // Once connected, publish an announcement...
      //client.publish(status_topic, "ESP01 alive");
    } else {
        Serial.print("failed, rc=");
        Serial.print(client.state());
        Serial.println(" try again in 3 seconds");
        // Wait 5 seconds before retrying
        delay(3000);
      }
   }
}
