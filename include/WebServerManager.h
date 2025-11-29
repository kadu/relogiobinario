#ifndef WEBSERVERMANAGER_H
#define WEBSERVERMANAGER_H

#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include "Config.h"
#include "LEDController.h"
#include "LittleFS.h"
class NTPManager; // forward

class WebServerManager {
private:
  AsyncWebServer* server;
  Config* config;
  LEDController* ledController;
  NTPManager* ntpManager; // novo
  const int HTTP_PORT = 80;

  // Métodos auxiliares privados
  String getContentType(String filename);
  void handleFileRequest(AsyncWebServerRequest *request);
  void handleApiClockGET(AsyncWebServerRequest* request);
  void handleApiSettingsGET(AsyncWebServerRequest* request);
  void handleApiSettingsPOST(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total);
  void handleNtpDebugGET(AsyncWebServerRequest* request);  // NEW

public:
  // modificar construtor para receber NTPManager*
  WebServerManager(AsyncWebServer* webServerPtr, Config* configPtr, LEDController* ledControllerPtr, NTPManager* ntpMgrPtr);

  // Inicializar endpoints
  void setupRoutes();

  // Iniciar servidor
  void begin();

  // Debug
  void printRoutes();
};

#endif