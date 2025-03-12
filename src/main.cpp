#include <Arduino.h>
#include <HTTPClient.h>
#include "env.h"
#include <WiFi.h>
#include <ArduinoJson.h>
#define LIGHT_PIN 2

int rain = 0.0;

void setup(){
    pinMode(LIGHT_PIN,OUTPUT);
    digitalWrite(LIGHT_PIN,LOW);

    Serial.begin(115200);
    WiFi.begin(SSID,PASS);

    while (WiFi.status()!= WL_CONNECTED)
    {
        delay(500);
        Serial.print("."); //waits for wifi connection to then jump out the while loop
    }

    Serial.println("Connected to the WiFi Network with IP address: ");
    Serial.println(WiFi.localIP());
    
}

void loop(){
    if(WiFi.status() == WL_CONNECTED) {
        HTTPClient http;

        http.begin(ENDPOINT); //just like localhost8000 used previously

        JsonDocument object;
        String request_body;
    
        object["temperature"] = 25.0;  
        object["humidity"] = 60.0;
        object["rainfall"] = rain;
        object["cloud"] = "Partly cloudy";
        object.shrinkToFit();
    
        serializeJson(object, request_body);

        http.GET(); 

        int responseCode = http.GET(); // -1 if request was unsuccessful 

        if (responseCode <= 0){
            Serial.print("An error occurred with this code: ");
            Serial.println(responseCode);
            http.end(); //close open connections 
            return;
        }
        Serial.print("HTTP Response Code: ");
        Serial.println(responseCode);

        String response = http.getString();
        Serial.println(response);
        http.end();

        DeserializationError error = deserializeJson(object, response);  //response  string to object json 

        if(error){  //error occured
            Serial.print("Deserialization failed: ");
            Serial.println(error.c_str());
            return;
        }

        int light_state = object["rainfall"];
        
        if(light_state > 0){
          digitalWrite(LIGHT_PIN, HIGH);
        }
        else digitalWrite(LIGHT_PIN, LOW);


    }
}