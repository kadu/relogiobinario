#include "WebServerManager.h"

WebServerManager::WebServerManager(AsyncWebServer* webServerPtr, Config* configPtr, LEDController* ledControllerPtr)
  : server(webServerPtr),
    config(configPtr),
    ledController(ledControllerPtr) {
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
  
  // Aqui você precisa acessar timeClient - será passado como parâmetro ou global
  // Por enquanto retornando estrutura básica
  root["status"] = "clock_endpoint";
  
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

  // Atualizar configurações
  config->setNtpServer(obj["ntpServer"] | "pool.ntp.org");
  config->setTimeZoneOffset(obj["finalTimeZone"] | -3);
  config->setLedBrightness(obj["ledBrightness"] | 10);
  config->setColorHour(obj["colorHour"] | "#D2691E");
  config->setColorMinute(obj["colorMinute"] | "#D2691E");
  config->setColorSecond(obj["colorSecond"] | "#D2691E");

  // Salvar no arquivo
  config->saveToFile();

  // Atualizar brilho em tempo real
  ledController->setBrightness(config->getLedBrightness());

  Serial.println("Settings updated and saved");
  
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