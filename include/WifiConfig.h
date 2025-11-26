#ifndef WIFICONFIG_H
#define WIFICONFIG_H

#include <Arduino.h>
#include <wifimanager.h>
#include <ESPAsyncWebServer.h>
#include <ESPmDNS.h>

class WifiConfig {
private:
  WIFIMANAGER* wifiManager;
  AsyncWebServer* webServer;
  bool mdnsStarted;
  const char* mdnsName;

public:
  WifiConfig(const char* mdnsHostname = "relogiobinario");

  // Inicializar WiFi Manager
  void init(AsyncWebServer* server);

  // Iniciar mDNS
  void startMDNS();

  // Tentar iniciar mDNS novamente (útil para chamar no loop)
  void updateMDNS();

  // Getters
  bool isMdnsStarted() const { return mdnsStarted; }
  bool isConnected() const { return WiFi.status() == WL_CONNECTED; }
  String getSSID() const { return WiFi.SSID(); }
  String getIP() const { return WiFi.localIP().toString(); }
  String getMdnsUrl() const;

  // Debug
  void printWifiStatus();
};

#endif