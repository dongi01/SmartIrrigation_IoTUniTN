#ifndef __READUARTDATA_H__
#define __READUARTDATA_H__

#include <stdint.h>

/*RX data schema
  
  They are all uint8_t and data has been divived as follow:
  - c is the control bit -> used to know the first byte of the expected 3. RXMoisture::c = 1
  - w are the bits that bring moisture data
  - x are the bits that bring light data
  - y is the bit that brings information about the state of the pump
  - z are the bits that bring temperature data
  
  RXMoisture  c w w w  w w w w
  
  
  RXLight     c x x x  x x x x
  
  
  RXTemp      c y z z  z z z z
  
*/

void readUart2(uint8_t &RXMoisture, uint8_t &RXLight, uint8_t &RXTemp);
void decodeData(const uint8_t &RXMoisture, const uint8_t &RXLight, const uint8_t &RXTemp, // input
                  int &dataMoisture, int &dataLight, int &dataTemp, int &dataPump); // output

#endif
