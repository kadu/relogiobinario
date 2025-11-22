#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <Ticker.h>
#include <OneButton.h>
#include <WiFiManager.h>
#include <ESPAsyncWebServer.h>
#include <ElegantOTA.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <NTPClient.h>

// ------------------- LEDS -------------------
#define NUM_LEDS 20
#define DATA_PIN 3
#define PIN_BUTTON01 0
#define PIN_BUTTON02 1
#define PIN_BUTTON03 2
#define LED_BRIGHTNESS 10

// ---------- Protótipos ----------
void mostraHora();
void mostraIp();
void displayBinary(const int *ledArray, int size, int number, uint32_t color);
void flipLed();
void click1();
void click2();
void click3();
void configWifi();
String mostraHora2();
uint32_t colorFromHex(String hex); // conversor HEX -> NeoPixel

// ---------- CONFIGURAÇÃO FÁCIL ----------
// Pode trocar direto aqui para qualquer cor em HEX (#RRGGBB)
String COLOR_HOURS_HEX = "#D2691E"; // Chocolate
String COLOR_MINUTES_HEX = "#D2691E";
String COLOR_SECONDS_HEX = "#D2691E";

Adafruit_NeoPixel strip(NUM_LEDS, DATA_PIN, NEO_GRB + NEO_KHZ800);

Ticker timerStatusLed(flipLed, 1000);
Ticker timerMostraHora(mostraHora, 1000);
Ticker timerIPPrint(mostraIp, 5000);

// ---------- Arrays das colunas ----------
const int horas_dezena[] = {10, 11};
const int horas_unidade[] = {15, 14, 13, 12};
const int minutos_dezena[] = {16, 17, 18};
const int minutos_unidade[] = {7, 8, 9, 19};
const int segundos_dezena[] = {6, 5, 4};
const int segundos_unidade[] = {0, 1, 2, 3};

const int *colunas[] = {horas_dezena, horas_unidade, minutos_dezena, minutos_unidade, segundos_dezena, segundos_unidade};
const int colSizes[] = {2, 4, 3, 4, 3, 4};
const int NUM_COLS = 6;

bool ledStatus = false;

OneButton button1(PIN_BUTTON01, true);
OneButton button2(PIN_BUTTON02, true);
OneButton button3(PIN_BUTTON03, true);

WiFiManager wm;
AsyncWebServer server(80);
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "a.st1.ntp.br", -3 * 3600, 60000);

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<html>
<head>
  <title>ESP32 Web Server</title>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
</head>
<body>
  <h1>Relógio Binário</h1>
  <p>Status: <span id="status">Carregando...</span></p>
  <script>
    setInterval(() => {
      fetch("/status").then(r => r.text()).then(t => document.getElementById("status").innerText = t);
    }, 1000);
  </script>
</body>
</html>
)rawliteral";

void setup()
{
  Serial.begin(115200);

  // Inicializa NeoPixel
  strip.begin();
  strip.setBrightness(LED_BRIGHTNESS);
  strip.clear();
  strip.show();

  for (size_t i = 0; i < NUM_LEDS; i++)
  {
    strip.setPixelColor(i, colorFromHex("#0000FF")); // Azul durante a inicialização
    strip.show();
    delay(50);
  }
  delay(150);
  for (size_t i = 0; i < NUM_LEDS; i++)
  {
    strip.setPixelColor(NUM_LEDS - i, colorFromHex("#000000")); // preto durante a inicialização
    strip.show();
    delay(50);
  }
  delay(300);
  strip.clear();
  strip.show();

  pinMode(8, OUTPUT); // LED onboard do C3 no GPIO8
  timerStatusLed.start();
  timerMostraHora.start();
  timerIPPrint.start();

  button1.attachClick(click1);
  button2.attachClick(click2);
  button3.attachClick(click3);
  button3.attachLongPressStop(configWifi);

  wm.setHostname("RelogioBinario");
  wm.setConfigPortalBlocking(true);
  wm.setClass("invert");
  wm.setConfigPortalTimeout(30);

  bool res = wm.autoConnect("RelogioBinario");

  if (!res)
  {
    Serial.println("Failed to connect or hit timeout");
  }
  else
  {
    Serial.println("connected...yeey :)");
  }

  // if (!MDNS.begin("RelogioBinario"))
  // {
  //   Serial.println("Error setting up MDNS responder!");
  // }

  // Rotas do servidor
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send_P(200, "text/html", index_html); });

  server.on("/status", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(200, "text/html", timeClient.getFormattedTime()); });

  server.on("/vai", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(204, "text/html", mostraHora2()); });

  ElegantOTA.begin(&server);
  server.begin();
  timeClient.begin();
  mostraIp();
}

void loop()
{
  timerStatusLed.update();
  timerMostraHora.update();
  timerIPPrint.update();
  button1.tick();
  button2.tick();
  button3.tick();
  ElegantOTA.loop();
  timeClient.update();
}

String mostraHora2()
{
  strip.clear();
  int s = timeClient.getSeconds();
  displayBinary(segundos_unidade, 4, s % 10, colorFromHex(COLOR_SECONDS_HEX));
  strip.show();
  timerStatusLed.pause();
  return "Teste";
}

void mostraHora()
{
  strip.clear();
  int s = timeClient.getSeconds();
  int m = timeClient.getMinutes();
  int h = timeClient.getHours();
  displayBinary(segundos_unidade, 4, s % 10, colorFromHex(COLOR_SECONDS_HEX));
  displayBinary(segundos_dezena, 3, s / 10, colorFromHex(COLOR_SECONDS_HEX));
  displayBinary(minutos_unidade, 4, m % 10, colorFromHex(COLOR_MINUTES_HEX));
  displayBinary(minutos_dezena, 3, m / 10, colorFromHex(COLOR_MINUTES_HEX));
  displayBinary(horas_unidade, 4, h % 10, colorFromHex(COLOR_HOURS_HEX));
  displayBinary(horas_dezena, 2, h / 10, colorFromHex(COLOR_HOURS_HEX));
  strip.show();
}

void mostraIp()
{
  Serial.println("Conectado ao WiFi! *************************");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

void displayBinary(const int *ledArray, int size, int number, uint32_t color)
{
  for (int i = 0; i < size; i++)
  {
    if (bitRead(number, i))
      strip.setPixelColor(ledArray[i], color);
  }
}

void flipLed()
{
  ledStatus = !ledStatus;
  digitalWrite(8, ledStatus);
}

void click1() { Serial.println("Click 1"); }
void click2() { Serial.println("Click 2"); }
void click3() { Serial.println("Click 3"); }

void configWifi()
{
  wm.resetSettings();
  ESP.restart();
}

// ---------- Função conversor HEX (#RRGGBB) ----------
uint32_t colorFromHex(String hex)
{
  if (hex.startsWith("#"))
    hex = hex.substring(1);
  long number = strtol(hex.c_str(), NULL, 16);
  byte r = (number >> 16) & 0xFF;
  byte g = (number >> 8) & 0xFF;
  byte b = number & 0xFF;
  return strip.Color(r, g, b);
}
