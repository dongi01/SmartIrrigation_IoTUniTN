#include <string.h>
#include <WiFiManager.h>
#include <HTTPClient.h>

#define RXD2 16
#define TXD2 17

uint8_t tmp = 0;
uint8_t RXMoisture = 0;
uint8_t RXLight = 0;
uint8_t RXTemp = 0;

int dataMoisture = 0;
int dataLight = 0;
int dataTemp = 0;
int dataPump = 0;
int dataTMP = 0;
int oldDataPump = dataPump;

String serverName = "http://IP_ADDRESS:3000";  // server address

void setup() {
  // Note the format for setting a serial port is as follows: Serial2.begin(baud-rate, protocol, RX pin, TX pin);
  Serial.begin(115200);
  //Serial1.begin(115200, SERIAL_8N1, RXD2, TXD2);
  Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2);
}

void loop() { //Choose Serial1 or Serial2 as required

  while (Serial2.available() > 0) {
    
    //Wait byte with control bit = 1
    tmp = Serial2.read();
    while(!(tmp >> 7 == 1)){
      tmp = Serial2.read();
    }

    //Saving ordered data
    RXMoisture = tmp;
    RXLight = Serial2.read();
    RXTemp = Serial2.read();
  }

  // Serial.println(RXMoisture);
  // Serial.println(RXLight);
  // Serial.println(RXTemp);

  dataMoisture = unSetControlBit(RXMoisture); //unsetting control bit -> returning to percentage
  //decompressing data
  dataLight = sevenBitsToRange(RXLight, 0, 20000);
  dataTMP = dataTemp;
  dataPump = dataTMP >> 6; // 1 or 0 to indicate whether or not the pump is activated

  if (dataPump != oldDataPump) {
    // send message to server
    if (dataPump == 1) {
      sendStartPump();
    } else if (dataPump == 0) {
      sendStopPump();
    } else {
      // errore
    }
    oldDataPump = dataPump;
  }
  


  Serial.println(dataMoisture);
  Serial.println(dataLight);
  Serial.println(dataTemp);
  Serial.println("");

  // if(RXTemp == 29){
  //   Serial2.write(5);
  // }

  delay(1000);
}

// -------------functions--------------

//maps the input value in a larger range (reciver side)
int sevenBitsToRange(uint8_t value, int minF, int maxF) {
  return minF + value * (maxF - minF) / 127; // 127 max value for 7 bits
}

uint8_t unSetControlBit(uint8_t firstData){
  return firstData - 128;
}

void sendStartPump() {

  HTTPClient http;

  // open a http connection between this board and node server
  String serverPath = serverName + "/alertStartPump";
  http.begin(serverPath.c_str());

  // Send HTTP GET request
  int httpResponseCode = http.GET();

  // close http connection
  http.end();
}

void sendStopPump() {

  HTTPClient http;

  // open a http connection between this board and ndoe server
  String serverPath = serverName + "/alertStopPump";
  http.begin(serverPath.c_str());

  // Send HTTP GET request
  int httpResponseCode = http.GET();

  // close http connection
  http.end();
}