#include <WiFiManager.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <HTTPClient.h>
#include <ESPmDNS.h>

#include <string.h>

// defining serial comunication and led pins
#define RXD2 16
#define TXD2 17
#define LED 2

// variable to manage incoming data
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

// node server address
String serverNode = "http://192.168.29.205:3000";

// defining this board as a server for listen to StartPump and StopPump request from node server (or web browser)
WebServer server(8080);


void setup() {
  // led as output debug
  pinMode(LED, OUTPUT);

  // serial for debug and ad comunication
  Serial.begin(115200);
  Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2);

  // wifimanager obj that manage wifi connection
  WiFiManager wm;

  // for resetting wm settings on startup
  // wm.resetSettings();

  // connect to wifi
  bool res;
  res = wm.autoConnect("AutoConnectAP");

  if (!res) {
    Serial.println("Failed to connect");
    // ESP.restart();
  } else {
    Serial.println("connected");
  }

  // startin mDNS to use esp32 instead of its IP address
  if (MDNS.begin("esp32")) {
    Serial.println("MDNS responder started");
  }

  // defining handler functions for HTTP requests
  server.on("/StartPump", handleStart);
  server.on("/StopPump", handleStop);
  server.onNotFound(handleNotFound);

  // start server
  server.begin();
  Serial.println("HTTP server started");
}


// 5 seconds delay
unsigned long timerDelay = 5000;
unsigned long lastTime = 0;

void loop() {

  // In this way we can create different timer but millis() will overflow afret aprox. 49 days
  // An idea is to separate the reading phase and the sending phase with 2 different timer: don't know if it is a good idea, mabye it's useless.
  // It's better to implement a way to manage the overflow (for example control if lastTime is bigger than millis() then use the unsigned long MAX value to calculate delay)
  if ((millis() - lastTime) > timerDelay){
  
    // reading data from UART
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

    /*
    RX data schema
    
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


    //unsetting control bit -> returning to percentage
    dataMoisture = unSetControlBit(RXMoisture);
    //decompressing data
    dataLight = sevenBitsToRange(RXLight, 0, 20000);
    dataTMP = dataTemp;
    // 1 or 0 to indicate whether or not the pump is activated
    dataPump = dataTMP >> 6;
  
    // send msg to node server only if the state of pump is changed
    if (dataPump != oldDataPump) {
      // send message to server
      if (dataPump == 1) {
        sendStartPump();
      } else if (dataPump == 0) {
        sendStopPump();
      } else {
        // error
      }
      oldDataPump = dataPump;
    }
  
    // send data to server
    sendData();
    /*
    //debugging print
    Serial.println(dataMoisture);
    Serial.println(dataLight);
    Serial.println(dataTemp);
    Serial.println("");
    */
    lastTime = millis();
  }

  // handle client
  server.handleClient();
}

// HTTP request handlers
void handleStart() {
  digitalWrite(LED, HIGH);
  delay(500);
  digitalWrite(LED, LOW);
  Serial.println("pump started");
  // send message via serial comunication
  Serial2.print(255); // 1111 1111
  Serial.println(255); // debug
  server.send(200, "text/plain", "OK");
}

void handleStop() {
  digitalWrite(LED, HIGH);
  delay(500);
  digitalWrite(LED, LOW);
  Serial.println("pump stopped");
  // send message via serial comunication
  Serial2.print(0); // 0000 0000
  Serial.println(0); // debug
  server.send(200, "text/plain", "OK");
}

void handleNotFound() {
  server.send(404, "text/plain", "ERROR: Not Found");
}

//maps the input value in a larger range (reciver side)
int sevenBitsToRange(uint8_t value, int minF, int maxF) {
  return minF + value * (maxF - minF) / 127; // 127 max value for 7 bits
}

// remove control bit from first data recived
uint8_t unSetControlBit(uint8_t firstData){
  return firstData - 128;
}

// send sensors data to node server
void sendData() {

  HTTPClient http;

  // open a http connection between this board and ndoe server
  String serverPath = serverNode + "/addSensorsData";
  http.begin(serverPath.c_str());

  // add HTTP POST header
  http.addHeader("Content-Type", "application/json");

  // create json string
  String data_json = "{\"soil_moisture\":\"" + (String)dataMoisture + "\",\"brightness\":\"" + (String)dataLight + "\",\"temperature\":\"" + (String)dataTemp + "\"}";

  // Send HTTP POST request
  int httpResponseCode = http.POST(data_json);

  // close http connection
  http.end();

}

// send HTTP message to node server if pump state change
void sendStartPump() {

  HTTPClient http;

  // open a http connection between this board and node server
  String serverPath = serverNode + "/alertStartPump";
  http.begin(serverPath.c_str());

  // Send HTTP GET request
  int httpResponseCode = http.GET();

  // close http connection
  http.end();
}

void sendStopPump() {

  HTTPClient http;

  // open a http connection between this board and ndoe server
  String serverPath = serverNode + "/alertStopPump";
  http.begin(serverPath.c_str());

  // Send HTTP GET request
  int httpResponseCode = http.GET();

  // close http connection
  http.end();
}
