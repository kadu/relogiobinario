#include "WifiConfig.h"

WifiConfig::WifiConfig(const char* mdnsHostname)
  : wifiManager(nullptr),
    webServer(nullptr),
    mdnsStarted(false),
    mdnsName(mdnsHostname) {
}

void WifiConfig::init(AsyncWebServer* server) {
  webServer = server;

  Serial.println("Starting WiFi Manager...");

  wifiManager = new WIFIMANAGER();
  wifiManager->startBackgroundTask();        // Executa a tarefa de background para gerenciar WiFi
  wifiManager->fallbackToSoftAp(true);       // Cria SoftAP se não conseguir conectar a nenhuma rede conhecida
  wifiManager->attachWebServer(webServer);   // Anexa a API ao servidor HTTP
  wifiManager->attachUI();                   // Anexa a UI ao servidor
}

void WifiConfig::startMDNS() {
  if (WiFi.status() == WL_CONNECTED) {
    if (MDNS.begin(mdnsName)) {
      MDNS.addService("http", "tcp", 80);
      mdnsStarted = true;
      Serial.print("mDNS responder started: http://");
      Serial.print(mdnsName);
      Serial.println(".local");
    } else {
      Serial.println("Failed to start mDNS responder");
    }
  }
}

void WifiConfig::updateMDNS() {
  if (!mdnsStarted && WiFi.status() == WL_CONNECTED) {
    startMDNS();
  }
}

String WifiConfig::getMdnsUrl() const {
  if (mdnsStarted) {
    String url = "http://";
    url += mdnsName;
    url += ".local";
    return url;
  }
  return "";
}

void WifiConfig::printWifiStatus() {
  Serial.println("=== WiFi Status ===");
  Serial.print("Connected: ");
  Serial.println(isConnected() ? "Yes" : "No");
  Serial.print("SSID: ");
  Serial.println(getSSID());
  Serial.print("IP Address: ");
  Serial.println(getIP());
  Serial.print("mDNS Started: ");
  Serial.println(mdnsStarted ? "Yes" : "No");
  if (mdnsStarted) {
    Serial.print("URL: ");
    Serial.println(getMdnsUrl());
  }
  Serial.println("===================");
}