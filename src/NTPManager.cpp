#include "NTPManager.h"

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

  // Configurar com valores do config
  setNTPServer(config->getNtpServer().c_str());
  setTimeOffset(config->getTimeZoneOffset() * 3600);

  timeClient->begin();
  _isInitialized = true;

  Serial.println("NTPManager initialized");
  printStatus();
}

void NTPManager::update() {
  if (_isInitialized && timeClient) {
    timeClient->update();
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