//funziona

#include <stdio.h>
#include <string.h>
#include <ESP8266WiFi.h>#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <LiquidCrystal_I2C.h>
#include <ArduinoJson.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

LiquidCrystal_I2C lcd(0x3F,16,2);  // set the LCD address to 0x3F for a 16 chars and 2 line display

const char* ssid = "TeleZic";
const char* password = "dagiu2016";
const long utcOffsetInSeconds = 3600;
char daysOfTheWeek[7][12] = {"Dom", "Lun", "Mar", "Mer", "Gio", "Ven", "Sab"};
byte upchar[8] = {
	0b00000,
	0b00100,
	0b01110,
	0b11111,
	0b00100,
	0b00100,
	0b00100,
	0b00000
};

byte downchar[8] = {
	0b00000,
	0b00100,
	0b00100,
	0b00100,
	0b11111,
	0b01110,
	0b00100,
	0b00000
};


String link1 = "https://finnhub.io/api/v1/quote?symbol=";
String api = "&token=cf2ma6iad3ian9c2nsngcf2ma6iad3ian9c2nso0";
String stocks[4] = {"TSLA", "AAPL", "META", "MSFT"};

WiFiUDP ntpUDP;                                                   // ntp for real time
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds); //


void setup() {

  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  lcd.init();
  lcd.clear();         
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("Connecting...");
  lcd.print(" ");
  lcd.createChar(0, upchar);
  lcd.createChar(1, downchar);

  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
  timeClient.begin();
  }
}

void loop() {
  if(WiFi.status()== WL_CONNECTED){
      WiFiClientSecure client;
      client.setInsecure();
      HTTPClient http;
      

      for(int i = 0; i < 4; i++) {


        String g_request = link1 + stocks[i] + api;
        ///Serial.println(g_request);
        
        // Your Domain name with URL path or IP address with path
        http.begin(client, g_request.c_str());

    
        // If you need Node-RED/server authentication, insert user and password below
        //http.setAuthorization("REPLACE_WITH_SERVER_USERNAME", "REPLACE_WITH_SERVER_PASSWORD");
          
        // Send HTTP GET request
        int httpResponseCode = http.GET();
        
        if (httpResponseCode>0) {
          timeClient.update();
          Serial.print("HTTP Response code: ");
          Serial.println(httpResponseCode);
          Serial.print(daysOfTheWeek[timeClient.getDay()]);
          Serial.println("  ");
          Serial.print(timeClient.getHours());
          Serial.print(":");
          Serial.print(timeClient.getMinutes());
          Serial.println(" ");
          StaticJsonDocument<200> doc;
          String str = http.getString(); //str=payload
          Serial.println(str);
          int str_len = str.length() + 1;
          char json[str_len];
          str.toCharArray(json, str_len);
          ///Serial.println(char_array[0]);
          DeserializationError error = deserializeJson(doc, json);
          float c = doc["c"];
          float dp = doc["dp"];
          Serial.println(c);
          Serial.println(dp);
          
          lcd.setCursor(0,1);
          lcd.print(dp);
          lcd.print("%");
          lcd.write(' ');
          lcd.setCursor(8,1);
          lcd.print(c);
          lcd.print("$");
          lcd.write(' ');
          lcd.setCursor(0,0);
          lcd.print(timeClient.getHours());
          lcd.print(":");
          lcd.print(timeClient.getMinutes());
          lcd.print(" ");
          lcd.print(daysOfTheWeek[timeClient.getDay()]);
          lcd.print(" ");
          lcd.print(stocks[i]);
          if (dp > 0) {
            lcd.setCursor(14,0);
            lcd.write(byte(0));
          }
          else {
            lcd.setCursor(14,0);
            lcd.write(byte(1));
          }
        }
        else {
          Serial.print("Error code: ");
          Serial.println(httpResponseCode);
          lcd.setCursor(0,1);
          lcd.print("Request error");
        }
        delay(2000);
      }
      http.end();
    }
    else {
      Serial.println("WiFi Disconnected");
      lcd.setCursor(0,1);
      lcd.print("WiFi Disconnected");
    }
}