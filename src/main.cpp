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
WebServerManager webServerManager(&webServer, &config, &ledController);

bool ledStatus = false;

void flipLed();
void updateClock();

Ticker timerStatusLed(flipLed, 1000);
Ticker timerMostraHora(updateClock, 1000);

void setup() {
  if (!LittleFS.begin(true)) {
    Serial.println("ERRO FATAL: Falha ao montar o LittleFS. Verifique as ferramentas e a particao.");
    return;
  }
  Serial.println("LittleFS montado com sucesso.");

  Serial.begin(115200);
  Serial.println("Hello, world!");

  Serial.println("Starting WiFi Manager...");
  wifiConfig.init(&webServer);

  // Carregar configurações
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

  // Inicializar NTPManager
  ntpManager.begin();

  // Inicializar ButtonManager
  buttonManager.init();

  // Inicializar WebServer Manager
  webServerManager.begin();

  timerStatusLed.start();
  timerMostraHora.start();

  wifiConfig.startMDNS();

  ProjectConfig::printConfig();
  config.printConfig();
  wifiConfig.printWifiStatus();
  ledController.printStatus();
  timeDisplay.printStatus();
  buttonManager.printStatus();
  ntpManager.printStatus();
  clockManager.printStatus();
  webServerManager.printRoutes();
}

void loop() {
  // Atualizar botões
  buttonManager.update();

  // Atualizar NTP
  ntpManager.update();

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