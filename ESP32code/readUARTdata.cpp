#include "readUARTdata.h"
#include <Arduino.h>

// --- ausiliary functions ---

// maps the input value in a larger range (reciver side)
int sevenBitsToRange(const uint8_t &value, int minF, int maxF) {
  return minF + value * (maxF - minF) / 127; // 127 max value for 7 bits
}

// remove control bit from first data recived
uint8_t unSetControlBit(const uint8_t &firstData){
  return firstData - 128;
}

// cleans the 7th bit of RXTemp to obtain the 6 bit of the temperature 
int getTemperature(uint8_t RXTemp){
  RXTemp << 2;
  return RXTemp >> 2;
}

// return 1 or 0 to indicate the state of the pump
int getDataPump(uint8_t RXTemp){
  return RXTemp >> 6;
}

// ---------------------------

void readUart2(uint8_t &RXMoisture, uint8_t &RXLight, uint8_t &RXTemp){
  uint8_t tmp = 0;
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
}

void decodeData(const uint8_t &RXMoisture, const uint8_t &RXLight, const uint8_t &RXTemp, // input
                  int &dataMoisture, int &dataLight, int &dataTemp, int &dataPump){ // output

  dataMoisture = unSetControlBit(RXMoisture);
  dataLight = sevenBitsToRange(RXLight, 0, 20000);
  dataTemp = getTemperature(RXTemp);
  dataPump = getDataPump(RXTemp);
  
}
