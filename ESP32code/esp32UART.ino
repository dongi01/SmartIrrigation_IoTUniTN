#define RXD2 16
#define TXD2 17

uint8_t tmp = 0;
uint8_t RXData1 = 0;
uint8_t RXData2 = 0;
uint8_t RXData3 = 0;

void setup() {
  // Note the format for setting a serial port is as follows: Serial2.begin(baud-rate, protocol, RX pin, TX pin);
  Serial.begin(115200);
  //Serial1.begin(115200, SERIAL_8N1, RXD2, TXD2);
  Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2);
}

void loop() { //Choose Serial1 or Serial2 as required

  while (Serial2.available() > 0) {
    
    tmp = Serial.read();
    while(tmp >> 7 != 1){
      tmp = Serial.read();
    }

    RXData1 = Serial.read();
    RXData2 = Serial2.read();
    RXData3 = Serial2.read();
  }

  Serial.println(RXData1);
  Serial.println(RXData2);
  Serial.println(RXData3);

  // if(RXData == 49){
  //   Serial2.write(5);
  // }
}