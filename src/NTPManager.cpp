#include "NTPManager.h"
#include <WiFi.h>

NTPManager::NTPManager(Config* configPtr)
  : config(configPtr),
    _isInitialized(false) {

  wifiUdp = new WiFiUDP();
  timeClient = new NTPClient(*wifiUdp, "pool.ntp.org", -3 * 3600, 60000);
}

NTPManager::~NTPManager() {
  if (timeClient) delete timeClient;
  if (wifiUdp) delete wifiUdp;
}

void NTPManager::begin() {
  if (!config) {
    Serial.println("Error: Config not initialized");
    return;
  }

  // Aguardar WiFi estar conectado antes de inicializar NTP
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 50) {
    delay(100);
    attempts++;
  }

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Warning: WiFi not connected, NTP init delayed");
    return;
  }

  // Pequeno delay adicional para garantir que lwIP está pronto
  delay(500);

  // Configurar com valores do config
  setNTPServer(config->getNtpServer().c_str());
  setTimeOffset(config->getTimeZoneOffset() * 3600.0);

  Serial.println("Calling timeClient->begin()...");
  timeClient->begin();

  // Forçar primeira atualização imediatamente
  Serial.println("Forcing first NTP update...");
  for (int i = 0; i < 5; i++) {
    timeClient->update();
    delay(100);
    Serial.print("Attempt ");
    Serial.print(i + 1);
    Serial.print(" - Time: ");
    Serial.println(timeClient->getFormattedTime());
    if (timeClient->getEpochTime() > 1000000000) { // epoch > ~2001
      break;
    }
  }

  _isInitialized = true;

  Serial.println("NTPManager initialized successfully");
  printStatus();
}

void NTPManager::update() {
  if (_isInitialized && timeClient) {
    // A biblioteca NTPClient gerencia a atualização em background.
    // A chamada forceUpdate() pode ser usada para forçar, mas o update()
    // da biblioteca já decide se deve ou não buscar a hora.
    // Para evitar spam no Serial, esta chamada pode ser condicional ou removida.
    // timeClient->update();
  }
}

void NTPManager::forceUpdate() {
  if (_isInitialized && timeClient) {
    timeClient->forceUpdate();
  }
}

int NTPManager::getHours() const {
  if (_isInitialized && timeClient) {
    return timeClient->getHours();
  }
  return 0;
}

int NTPManager::getMinutes() const {
  if (_isInitialized && timeClient) {
    return timeClient->getMinutes();
  }
  return 0;
}

int NTPManager::getSeconds() const {
  if (_isInitialized && timeClient) {
    return timeClient->getSeconds();
  }
  return 0;
}

unsigned long NTPManager::getEpochTime() const {
  if (_isInitialized && timeClient) {
    return timeClient->getEpochTime();
  }
  return 0;
}

String NTPManager::getFormattedTime() const {
  if (_isInitialized && timeClient) {
    return timeClient->getFormattedTime();
  }
  return "00:00:00";
}

void NTPManager::setNTPServer(const char* serverName) {
  if (timeClient) {
    timeClient->setPoolServerName(serverName);
    Serial.print("NTP Server set to: ");
    Serial.println(serverName);
  }
}

void NTPManager::setTimeOffset(int offsetSeconds) {
  if (timeClient) {
    timeClient->setTimeOffset(offsetSeconds);
    Serial.print("Time offset set to: ");
    Serial.print(offsetSeconds / 3600);
    Serial.println(" hours");
  }
}

void NTPManager::setUpdateInterval(unsigned long ms) {
  if (timeClient) {
    timeClient->setUpdateInterval(ms);
    Serial.print("Update interval set to: ");
    Serial.print(ms);
    Serial.println(" ms");
  }
}

String NTPManager::getNTPServer() const {
  if (config) {
    return config->getNtpServer();  // usar o do config, não hardcoded
  }
  return "pool.ntp.org";
}

int NTPManager::getTimeOffset() const {
  if (config) {
    return config->getTimeZoneOffset();
  }
  return -3;
}

void NTPManager::printStatus() {
  Serial.println("=== NTPManager Status ===");
  Serial.print("Initialized: ");
  Serial.println(_isInitialized ? "Yes" : "No");
  Serial.print("Current Time: ");
  Serial.println(getFormattedTime());
  Serial.print("Time Offset: ");
  Serial.print(getTimeOffset());
  Serial.println(" hours");
  Serial.print("NTP Server: ");
  Serial.println(config->getNtpServer());
  Serial.println("========================");
}