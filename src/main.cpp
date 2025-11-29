#include <Arduino.h>
#include "ProjectConfig.h"
#include "Config.h"
#include "WifiConfig.h"
#include "LEDController.h"
#include "TimeDisplay.h"
#include "WebServerManager.h"
#include "ButtonManager.h"
#include "NTPManager.h"
#include "ClockManager.h"
#include <AsyncTCP.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <Ticker.h>
#include "LittleFS.h"
#include <ESPmDNS.h>

Config config;
WifiConfig wifiConfig("relogiobinario");
LEDController ledController(ProjectConfig::getNumLeds(), ProjectConfig::getDataPin(), &config);
TimeDisplay timeDisplay(&ledController);
ButtonManager buttonManager(
  ProjectConfig::getButtonPin01(),
  ProjectConfig::getButtonPin02(),
  ProjectConfig::getButtonPin03(),
  &timeDisplay,
  &ledController
);
NTPManager ntpManager(&config);
ClockManager clockManager(&ntpManager, &timeDisplay, &buttonManager);

AsyncWebServer webServer(80);
WebServerManager webServerManager(&webServer, &config, &ledController, &ntpManager);
void startNetworkServicesOnce();

bool ledStatus = false;
static bool networkServicesStarted = false;

void flipLed();
void updateClock();

Ticker timerStatusLed(flipLed, 1000);
Ticker timerMostraHora(updateClock, 1000);

void setup() {
  if (!LittleFS.begin(true)) return;
  Serial.begin(115200);

  // Desabilitar logs verbosos de WiFi/UDP
  esp_log_level_set("WiFiUdp", ESP_LOG_NONE);
  esp_log_level_set("lwip", ESP_LOG_NONE);

  config.loadFromFile();

  // Inicializar LEDs com pins do ProjectConfig
  ledController.setLEDArrays(
    ProjectConfig::getHorasDezenaPins(), ProjectConfig::getSizeHorasDezena(),
    ProjectConfig::getHorasUnidadePins(), ProjectConfig::getSizeHorasUnidade(),
    ProjectConfig::getMinutosDezenaPins(), ProjectConfig::getSizeMinutosDezena(),
    ProjectConfig::getMinutosUnidadePins(), ProjectConfig::getSizeMinutosUnidade(),
    ProjectConfig::getSegundosDezenaPins(), ProjectConfig::getSizeSegundosDezena(),
    ProjectConfig::getSegundosUnidadePins(), ProjectConfig::getSizeSegundosUnidade()
  );
  ledController.begin();
  ledController.setBrightness(config.getLedBrightness());
  ledController.animationStartup();

  // Configurar TimeDisplay
  timeDisplay.setLEDArrays(
    ProjectConfig::getHorasDezenaPins(), ProjectConfig::getSizeHorasDezena(),
    ProjectConfig::getHorasUnidadePins(), ProjectConfig::getSizeHorasUnidade(),
    ProjectConfig::getMinutosDezenaPins(), ProjectConfig::getSizeMinutosDezena(),
    ProjectConfig::getMinutosUnidadePins(), ProjectConfig::getSizeMinutosUnidade(),
    ProjectConfig::getSegundosDezenaPins(), ProjectConfig::getSizeSegundosDezena(),
    ProjectConfig::getSegundosUnidadePins(), ProjectConfig::getSizeSegundosUnidade()
  );

  ProjectConfig::printConfig();
  config.printConfig();

  // INICIALIZAR WIFI MANAGER ANTES DE CONECTAR
  Serial.println("Starting WiFi Manager...");
  wifiConfig.init(&webServer);

  // Inicializar ButtonManager
  buttonManager.init();
  buttonManager.setClockManager(&clockManager);

  // Inicializar WebServer Manager
  webServerManager.begin();

  timerStatusLed.start();
  timerMostraHora.start();

  wifiConfig.printWifiStatus();
  ledController.printStatus();
  timeDisplay.printStatus();
  buttonManager.printStatus();
  clockManager.printStatus();
  webServerManager.printRoutes();
}

void loop() {
  // tentar iniciar serviços de rede quando WiFi estiver pronto
  startNetworkServicesOnce();

  // Atualizar botões
  buttonManager.update();

  timerStatusLed.update();
  timerMostraHora.update();

  wifiConfig.updateMDNS();
}

void flipLed() {
  ledStatus = !ledStatus;
  digitalWrite(ProjectConfig::getStatusLedPin(), ledStatus);
}

void updateClock() {
  clockManager.update();
}

void startNetworkServicesOnce() {
  if (networkServicesStarted) return;
  if (WiFi.status() != WL_CONNECTED) return;

  // iniciar NTP e mDNS agora que WiFi/lwIP estão prontos
  ntpManager.begin();
  wifiConfig.startMDNS();
  networkServicesStarted = true;

  Serial.println("Network services started (NTP, mDNS)");
}