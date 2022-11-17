#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <Arduino_JSON.h>
#include <ESP8266HTTPClient.h>

const uint16_t port = 3000;
const char *host = "192.168.1.146";
WiFiClient client;
void setup(){

    Serial.begin(115200);
    Serial.println("Connecting...\n");
    WiFi.mode(WIFI_STA);
    WiFi.begin("Wind3 HUB-8B194A", "3u1pa7ucv6yrxyb6"); // change it to your ussid and password
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
}

void loop(){

    if (!client.connect(host, port)){

        Serial.println("Connection to host failed");
        delay(1000);
        return;
    }
    Serial.println("Connected to server successful!");
    /*client.println("Hello From ESP8266");
    delay(250);*/
    //while (client.available() > 0)
    //{
        //char c = client.read();
        //Serial.write(c);
        client.write("{\"soil_moisture\":\"123.345\",\"brightness\":\"123.123\"}");
    //}
    Serial.print('\n');

    

    client.stop();
    delay(5000);
}