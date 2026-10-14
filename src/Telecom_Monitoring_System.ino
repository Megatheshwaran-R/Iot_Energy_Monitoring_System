/**********************************************************************************
 * Project: IoT Energy Saving & Management of Telecom Operators and RCR
 * Hardware: NodeMCU (ESP8266) + DHT11 + Analog Gas/Flame Sensor + Relay Module
 * Cloud Platform: Blynk IoT
 **********************************************************************************/

#define BLYNK_TEMPLATE_ID "YOUR_TEMPLATE_ID"
#define BLYNK_TEMPLATE_NAME "Telecom"
#define BLYNK_AUTH_TOKEN "YOUR_AUTH_TOKEN"
#define BLYNK_PRINT Serial 

#include <ESP8266WiFi.h> 
#include <BlynkSimpleEsp8266.h> 
#include <DHT.h> 

char auth[] = BLYNK_AUTH_TOKEN; 
char ssid[] = "YOUR_WIFI_SSID";
char pass[] = "YOUR_WIFI_PASSWORD";

#define DHTPIN D2          
#define DHTTYPE DHT11     

DHT dht(DHTPIN, DHTTYPE); 
BlynkTimer timer; 
WidgetLCD lcd(V1); 

BLYNK_WRITE(V0) 
{ 
    int value = param.asInt(); 
    Serial.println(value); 
    if(value == 1) 
    { 
        digitalWrite(D8, HIGH); 
        Serial.println("LED ON"); 
        lcd.print(0, 0, "LIGHT ON- V=12V");   
        lcd.print(0, 1, " I=52mA,P=12W  ");   
        delay(2000);  
        lcd.clear(); 
    } 
    if(value == 0) 
    { 
        digitalWrite(D8, LOW); 
        Serial.println("LED OFF"); 
        lcd.print(0, 0, "LIGHT ON, V=12V  ");   
        lcd.print(0, 1, " I=0mA,P=0 W  ");   
        delay(2000);  
        lcd.clear(); 
    } 
} 

void sendSensor() 
{ 
    float h = dht.readHumidity(); 
    float t = dht.readTemperature(); 
  
    if (isnan(h) || isnan(t))  
    { 
        Serial.println("Failed to read from DHT sensor!"); 
        return; 
    } 

    Blynk.virtualWrite(V5, t); 
    Blynk.virtualWrite(V6, h); 
 
    Serial.println("TEMPERATURE="); 
    Serial.print(t); 
    Serial.println("HUMIDITY="); 
    Serial.print(h); 

    if(t > 34) 
    { 
        Blynk.logEvent("temp_alert","Temp above 34 degree"); 
        digitalWrite(D7, HIGH); 
        lcd.print(0, 0, "HIGH TEMP");   
        lcd.print(0, 1, "HIGH SPEED-FAN  ");   
        delay(2000);  
        lcd.clear(); 
    } 
    else  
    { 
        digitalWrite(D7, LOW); 
        lcd.print(0, 0, "NORMAL TEMP");   
        lcd.print(0, 1, "LOW SPEED-FAN  ");   
        delay(2000);  
        lcd.clear(); 
    } 
} 

void air(void) 
{ 
    unsigned int sensorvalue = analogRead(A0); 
    Serial.println("Gas ="); 
    Serial.print(sensorvalue); 
 
    Blynk.virtualWrite(V4, sensorvalue); 
    if(sensorvalue <= 65) 
    { 
        Blynk.logEvent("fire_alert","FIRE-BE ALERT"); 
        digitalWrite(D6, HIGH);  
        digitalWrite(D5, HIGH);  
     
        lcd.print(0, 0, "FIRE-BE ALERT ");   
        lcd.print(0, 1, " EMERGENCY     ");   
        delay(2000);  
        lcd.clear(); 
    } 
    else 
    { 
        digitalWrite(D6, LOW); 
        digitalWrite(D5, LOW); 
        lcd.print(0, 0, "NO FIRE ");   
        lcd.print(0, 1, "***NORMAL***");   
        delay(2000);  
        lcd.clear(); 
    } 
} 

void setup() 
{ 
    Serial.begin(9600); 
    pinMode(D8, OUTPUT); 
    pinMode(D7, OUTPUT); 
    pinMode(D6, OUTPUT); 
    pinMode(D5, OUTPUT);  

    Blynk.begin(auth, ssid, pass); 
    dht.begin(); 

    timer.setInterval(1000L, sendSensor); 
} 

void loop() 
{ 
    Blynk.run(); 
    timer.run(); 
   
    lcd.print(0, 0, "Telecom Monitor");   
    lcd.print(0, 1, "   Using IoT             ");   
    delay(2000);  
    lcd.clear(); 
    air(); 
} 
