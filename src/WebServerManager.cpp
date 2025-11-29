#include "WebServerManager.h"
#include "NTPManager.h"
#include <AsyncJson.h>
#include <ArduinoJson.h>

WebServerManager::WebServerManager(AsyncWebServer* webServerPtr, Config* configPtr, LEDController* ledControllerPtr, NTPManager* ntpMgrPtr)
  : server(webServerPtr),
    config(configPtr),
    ledController(ledControllerPtr),
    ntpManager(ntpMgrPtr) {
}

String WebServerManager::getContentType(String filename) {
  if (filename.endsWith(".html")) return "text/html";
  if (filename.endsWith(".css")) return "text/css";
  if (filename.endsWith(".js")) return "application/javascript";
  if (filename.endsWith(".png")) return "image/png";
  if (filename.endsWith(".jpg")) return "image/jpeg";
  if (filename.endsWith(".ico")) return "image/x-icon";
  if (filename.endsWith(".json")) return "application/json";
  return "text/plain";
}

void WebServerManager::handleApiClockGET(AsyncWebServerRequest* request) {
  AsyncJsonResponse* response = new AsyncJsonResponse();
  JsonObject root = response->getRoot().to<JsonObject>();

  if (ntpManager && ntpManager->isInitialized()) {
    root["time"] = ntpManager->getFormattedTime();
    root["hours"] = ntpManager->getHours();
    root["minutes"] = ntpManager->getMinutes();
    root["seconds"] = ntpManager->getSeconds();
    root["epoch"] = ntpManager->getEpochTime();
    root["timeZoneOffsetHours"] = ntpManager->getTimeOffset();
    root["ntpServer"] = ntpManager->getNTPServer();
  } else {
    root["error"] = "NTP not initialized";
    root["time"] = "00:00:00";
    root["timeZoneOffsetHours"] = config ? config->getTimeZoneOffset() : 0;
  }

  response->setLength();
  request->send(response);
}

void WebServerManager::handleApiSettingsGET(AsyncWebServerRequest* request) {
  if (!LittleFS.exists("/settings.json")) {
    request->send(404, "application/json", "{\"error\":\"Settings file not found\"}");
    return;
  }

  File f = LittleFS.open("/settings.json", "r");
  if (!f) {
    request->send(500, "application/json", "{\"error\":\"Failed to open file\"}");
    return;
  }

  AsyncJsonResponse* response = new AsyncJsonResponse();
  DeserializationError error = deserializeJson(response->getRoot(), f);
  f.close();

  if (error) {
    request->send(500, "application/json", "{\"error\":\"Invalid JSON in file\"}");
    return;
  }

  response->setLength();
  request->send(response);
}

void WebServerManager::handleApiSettingsPOST(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
  JsonDocument inDoc;
  DeserializationError error = deserializeJson(inDoc, data, len);

  if (error) {
    request->send(400, "application/json", "{\"error\":\"Invalid JSON\"}");
    return;
  }

  JsonObject obj = inDoc.as<JsonObject>();
  serializeJsonPretty(obj, Serial);

  // Extrair o brilho do JSON recebido
  int newBrightness = obj["ledBrightness"] | 50; // Padrão 128 (50%) se não vier no JSON

  config->setNtpServer(obj["ntpServer"] | "pool.ntp.org");
  config->setTimeZoneOffset(obj["finalTimeZone"].as<float>());
  config->setLedBrightness(newBrightness);
  config->setColorHour(obj["colorHour"] | "#D2691E");
  config->setColorMinute(obj["colorMinute"] | "#D2691E");
  config->setColorSecond(obj["colorSecond"] | "#D2691E");

  // salvar no arquivo
  config->saveToFile();

  // Aplicar o novo brilho imediatamente nos LEDs
  if (ledController) {
    ledController->setBrightness(newBrightness);
  }

  // atualizar NTPManager com novo servidor/offset
  if (ntpManager) {
    ntpManager->setNTPServer(config->getNtpServer().c_str());
    ntpManager->setTimeOffset(config->getTimeZoneOffset() * 3600.0);
    // Forçar atualização do NTP para aplicar o novo timezone imediatamente
    Serial.println("Forcing NTP update to apply new timezone...");
    ntpManager->forceUpdate();
  }

  Serial.println("Settings updated and saved");

  // apenas responder com sucesso, sem reinício
  request->send(200, "application/json", "{\"status\":\"saved\"}");
}

void WebServerManager::handleFileRequest(AsyncWebServerRequest *request) {
  String path = request->url();

  if (path == "/") {
    path = "/index.html";
  }

  Serial.print("Tentativa de requisicao: ");
  Serial.println(path);

  if (LittleFS.exists(path)) {
    String contentType = getContentType(path);
    request->send(LittleFS, path, contentType);
    Serial.println(" -> Arquivo servido com sucesso.");
  } else {
    request->send(404, "text/plain", "Erro 404: Arquivo nao encontrado no LittleFS.");
    Serial.print(" -> ERRO 404: Arquivo nao encontrado.");
  }
}

void WebServerManager::handleNtpDebugGET(AsyncWebServerRequest* request) {
  AsyncJsonResponse* response = new AsyncJsonResponse();
  JsonObject root = response->getRoot().to<JsonObject>();

  root["isInitialized"] = ntpManager ? ntpManager->isInitialized() : false;

  if (ntpManager && ntpManager->isInitialized()) {
    root["formattedTime"] = ntpManager->getFormattedTime();
    root["epochTime"] = (long)ntpManager->getEpochTime();
    root["hours"] = ntpManager->getHours();
    root["minutes"] = ntpManager->getMinutes();
    root["seconds"] = ntpManager->getSeconds();
    root["timeOffset"] = ntpManager->getTimeOffset();
    root["ntpServer"] = ntpManager->getNTPServer();

    // Validação: epoch > 2001
    unsigned long epoch = ntpManager->getEpochTime();
    root["epochValid"] = (epoch > 1000000000) ? "YES" : "NO (too low)";

  } else {
    root["error"] = "NTP not initialized";
  }

  response->setLength();
  request->send(response);
}

void WebServerManager::setupRoutes() {
  // GET /api/clock - Retorna hora atual
  server->on("/api/clock", HTTP_GET, [this](AsyncWebServerRequest* request) {
    this->handleApiClockGET(request);
  });

  // GET /api/settings - Retorna configurações salvas
  server->on("/api/settings", HTTP_GET, [this](AsyncWebServerRequest* request) {
    this->handleApiSettingsGET(request);
  });

  // POST /api/settings - Salva novas configurações
  server->on("/api/settings", HTTP_POST,
    [](AsyncWebServerRequest *request){},
    NULL,
    [this](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
      this->handleApiSettingsPOST(request, data, len, index, total);
    }
  );

  // NEW: GET /api/ntp-debug - Debug NTP
  server->on("/api/ntp-debug", HTTP_GET, [this](AsyncWebServerRequest* request) {
    this->handleNtpDebugGET(request);
  });

  // Handler para arquivos estáticos (deve ser por último)
  server->onNotFound([this](AsyncWebServerRequest *request) {
    this->handleFileRequest(request);
  });

  Serial.println("All routes configured");
}

void WebServerManager::begin() {
  setupRoutes();
  server->begin();
  Serial.print("HTTP server started on port ");
  Serial.println(HTTP_PORT);
}

void WebServerManager::printRoutes() {
  Serial.println("=== WebServer Routes ===");
  Serial.println("GET  /api/clock       - Retorna hora atual");
  Serial.println("GET  /api/settings    - Retorna configurações salvas");
  Serial.println("POST /api/settings    - Salva novas configurações");
  Serial.println("GET  /                - Retorna index.html");
  Serial.println("GET  /<file>          - Retorna arquivo do LittleFS");
  Serial.println("========================");
}