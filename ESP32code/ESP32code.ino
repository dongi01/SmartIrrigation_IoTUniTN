#include <WiFiManager.h>
#include <ESPmDNS.h>

#include <string.h>

#include "httpServer.h"
#include "httpClient.h"
#include "readUARTdata.h"

// max value for unsigned long
#define ULONG_MAX 18446744073709551615

// defining serial comunication and led pins
#define RXD2 16
#define TXD2 17

// variables to manage incoming data
uint8_t RXMoisture = 0;
uint8_t RXLight = 0;
uint8_t RXTemp = 0;

int dataMoisture = 0;
int dataLight = 0;
int dataTemp = 0;
int dataPump = 0;
int oldDataPump = dataPump;

// --- SETUP ---
void setup() {
  // serial for debug and ad comunication
  Serial.begin(115200);
  Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2);

  // wifimanager obj that manage wifi connection with GUI
  WiFiManager wm;

  // connect to wifi
  bool res;
  res = wm.autoConnect("AutoConnectAP");

  if (!res) {
    Serial.println("Failed to connect");
    ESP.restart();
  } else {
    Serial.println("connected");
  }

  // startin mDNS to use esp32 instead of its IP address
  if (MDNS.begin("esp32")) {
    Serial.println("MDNS responder started");
  }

  // defining handler functions for HTTP requests
  defineHandlers();

  // start server
  startServer();
  
  Serial.println("HTTP server started");
}


// 5 seconds delay
const unsigned long timerDelay = 5000;
unsigned long lastTime = 0;

// --- LOOP ---
void loop() {

  // read every [timeDelay] seconds
  // if condition to manage the overflow of millins() after reaching ULONG_MAX(49 day approx.)
  if(millis() > lastTime){
    if ((millis() - lastTime) > timerDelay){
      readManageData();
      lastTime = millis();
    }
  } else { // when millis() overflow
      if ((millis() + (ULONG_MAX - lastTime)) > timerDelay){
      readManageData();
      lastTime = millis();
    }
  }

  // handle client
  handleClientHTTP();
}



// reading and managing data
void readManageData() {
  
  // reading data from UART2
  readUart2(RXMoisture, RXLight, RXTemp);

  // undecode row byte in usefull data
  decodeData(RXMoisture, RXLight, RXTemp, // input
              dataMoisture, dataLight, dataTemp, dataPump); // output

  // send msg to node server only if the state of pump is changed
  if (dataPump != oldDataPump) {
    // send message to server
    if (dataPump == 1) {
      sendStartPump();
    } else if (dataPump == 0) {
      sendStopPump();
    }
    oldDataPump = dataPump;
  }

  // send data to server
  sendData(dataMoisture, dataLight, dataTemp);

  //debugging print
  Serial.println(dataMoisture);
  Serial.println(dataLight);
  Serial.println(dataTemp);
  Serial.println(dataPump);
  
  Serial.println("");
}
