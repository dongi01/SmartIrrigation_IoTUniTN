#ifndef __HTTPCLIENT_H__
#define __HTTPCLIENT_H__

/*
functions that send data to node server:
- sendData will send the 3 data recived as paramether
- sendStartPump will send a HTTP GET message to http://<serverNodeIP>:3000/alertStartPump
- sendStopPump will send a HTTP GET message to http://<serverNodeIP>:3000/alertStopPump
*/

void sendData(const int &dataMoisture, const int &dataLight, const int &dataTemp);
void sendStartPump();
void sendStopPump();


#endif
