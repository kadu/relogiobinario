#ifndef NTPMANAGER_H
#define NTPMANAGER_H

#include <Arduino.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include "Config.h"

class NTPManager {
private:
  NTPClient* timeClient;
  WiFiUDP* wifiUdp;
  Config* config;
  bool _isInitialized;

public:
  NTPManager(Config* configPtr);
  ~NTPManager();

  // Inicialização
  void begin();

  // Atualizar hora (chamar no loop)
  void update();

  // Getters de tempo
  int getHours() const;
  int getMinutes() const;
  int getSeconds() const;
  unsigned long getEpochTime() const;
  String getFormattedTime() const;

  // Configuração
  void setNTPServer(const char* serverName);
  void setTimeOffset(int offsetSeconds);
  void setUpdateInterval(unsigned long ms);

  // Getters de informações
  bool isInitialized() const { return _isInitialized; }
  String getNTPServer() const;
  int getTimeOffset() const;

  // Debug
  void printStatus();
};

#endif