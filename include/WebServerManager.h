#ifndef WEBSERVERMANAGER_H
#define WEBSERVERMANAGER_H

#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include "Config.h"
#include "LEDController.h"
#include "LittleFS.h"
#if __has_include("ArduinoJson.h")
  #include <ArduinoJson.h>
  #include <AsyncJson.h>
  #include <AsyncMessagePack.h>
#endif

class WebServerManager {
private:
  AsyncWebServer* server;
  Config* config;
  LEDController* ledController;
  const int HTTP_PORT = 80;

  // Métodos auxiliares privados
  String getContentType(String filename);
  void handleFileRequest(AsyncWebServerRequest *request);
  void handleApiClockGET(AsyncWebServerRequest* request);
  void handleApiSettingsGET(AsyncWebServerRequest* request);
  void handleApiSettingsPOST(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total);

public:
  WebServerManager(AsyncWebServer* webServerPtr, Config* configPtr, LEDController* ledControllerPtr);

  // Inicializar endpoints
  void setupRoutes();

  // Iniciar servidor
  void begin();

  // Debug
  void printRoutes();
};

#endif