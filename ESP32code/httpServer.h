#ifndef __HTTPSERVER_H__
#define __HTTPSERVER_H__

void startServer();
void handleClientHTTP();
void defineHandlers();
void handleStart();
void handleStop();
void handleNotFound();

#endif
