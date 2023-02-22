#ifndef __HTTPSERVER_H__
#define __HTTPSERVER_H__

/*
these functions act as wrapper functions to manage the server object defined in httpServer.cpp
In particular startServer starts the server, handleClientHTTP is called in a loop to respond to HTTP requests,
defineHandlers associate the link with the corrisponding handler function:
    /StartPump -> handleStart
    /StopPump -> handleStop
    /??? -> handleNotFound
*/

void startServer();
void handleClientHTTP();
void defineHandlers();
void handleStart();
void handleStop();
void handleNotFound();

#endif
