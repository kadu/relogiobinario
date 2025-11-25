#include <Arduino.h>
#include <OneButton.h>
#include <AsyncTCP.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <wifimanager.h>
#include <Adafruit_NeoPixel.h>
#include <WiFiUdp.h>
#include <Ticker.h>
#include <NTPClient.h>
#include "LittleFS.h"
#if __has_include("ArduinoJson.h")
  #include <ArduinoJson.h>
  #include <AsyncJson.h>
  #include <AsyncMessagePack.h>
#endif

#define NUM_LEDS 20
#define DATA_PIN 3
#define LED_BRIGHTNESS 10

#define PIN_BUTTON01 0
#define PIN_BUTTON02 1
#define PIN_BUTTON03 2

void flipLed();
void displayBinary(const int *ledArray, int size, int number, uint32_t color);
uint32_t colorFromHex(String hex);
void loadSettings();

OneButton button1(PIN_BUTTON01, true);
OneButton button2(PIN_BUTTON02, true);
OneButton button3(PIN_BUTTON03, true);



WIFIMANAGER WifiManager;
AsyncWebServer webServer(80);
AsyncEventSource events("/events");
WiFiUDP wifiUdp;
NTPClient timeClient(wifiUdp, "pool.ntp.org", -3 * 3600, 60000); // GMT-3, update every minute

String COLOR_HOURS_HEX = "#D2691E"; // Chocolate
String COLOR_MINUTES_HEX = "#D2691E";
String COLOR_SECONDS_HEX = "#D2691E";

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
Adafruit_NeoPixel strip(NUM_LEDS, DATA_PIN, NEO_GRB + NEO_KHZ800);


void click1();
void click2();
void click3();
uint32_t colorFromHex(String hex);
void mostraHora();
String getContentType(String filename);
void handleFileRequest(AsyncWebServerRequest *request);

Ticker timerStatusLed(flipLed, 1000);
Ticker timerMostraHora(mostraHora, 1000);


void setup() {
  if (!LittleFS.begin(true)) {
    Serial.println("ERRO FATAL: Falha ao montar o LittleFS. Verifique as ferramentas e a particao.");
    return;
  }
  Serial.println("LittleFS montado com sucesso.");

  Serial.begin(115200);
  Serial.println("Hello, world!");

  button1.attachClick(click1);
  button2.attachClick(click2);
  button3.attachClick(click3);


  Serial.println("Starting WiFi Manager...");

  WifiManager.startBackgroundTask();        // Run the background task to take care of our Wifi
  WifiManager.fallbackToSoftAp(true);       // Run a SoftAP if no known AP can be reached
  WifiManager.attachWebServer(&webServer);  // Attach our API to the HTTP Webserver
  WifiManager.attachUI();                   // Attach the UI to the Webserver


  webServer.on("/api/clock", HTTP_GET, [](AsyncWebServerRequest* request) {
    AsyncJsonResponse* response = new AsyncJsonResponse();
    JsonObject root = response->getRoot().to<JsonObject>();
    root["time"] = timeClient.getFormattedTime();
    response->setLength();
    request->send(response);
  });

  webServer.on("/api/settings", HTTP_GET, [](AsyncWebServerRequest* request) {
    // Verificar se o arquivo existe
    if (!LittleFS.exists("/settings.json")) {
      request->send(404, "application/json", "{\"error\":\"Settings file not found\"}");
      return;
    }

    // Abrir e ler o arquivo
    File f = LittleFS.open("/settings.json", "r");
    if (!f) {
      request->send(500, "application/json", "{\"error\":\"Failed to open file\"}");
      return;
    }

    // Ler conteúdo do arquivo e enviar como resposta
    AsyncJsonResponse* response = new AsyncJsonResponse();
    DeserializationError error = deserializeJson(response->getRoot(), f);
    f.close();

    if (error) {
      request->send(500, "application/json", "{\"error\":\"Invalid JSON in file\"}");
      return;
    }

    response->setLength();
    request->send(response);
  });

  webServer.on("/api/settings", HTTP_POST,
    [](AsyncWebServerRequest *request){},
    NULL,
    [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
      JsonDocument outDoc;
      DeserializationError error = deserializeJson(outDoc, data, len);

      if (error) {
        request->send(400, "application/json", "{\"error\":\"Invalid JSON\"}");
        return;
      }

      JsonObject obj = outDoc.as<JsonObject>();

      // Ler valores do JSON (valores padrão caso falte)
      const char* ntpServer       = obj["ntpServer"] | "pool.ntp.org";
      const char* timeZoneSelect  = obj["timeZoneSelect"] | "-3";
      const char* customTimeZone  = obj["customTimeZone"] | "";
      const char* finalTimeZone   = obj["finalTimeZone"] | "-3";
      const char* ledBrightness   = obj["ledBrightness"] | "10";
      const char* colorHour       = obj["colorHour"] | "#D2691E";
      const char* colorMinute     = obj["colorMinute"] | "#D2691E";
      const char* colorSecond     = obj["colorSecond"] | "#D2691E";

      // Criar objeto JSON para salvar
      JsonDocument saveDoc;
      saveDoc["ntpServer"] = ntpServer;
      saveDoc["timeZoneSelect"] = timeZoneSelect;
      saveDoc["customTimeZone"] = customTimeZone;
      saveDoc["finalTimeZone"] = finalTimeZone;
      saveDoc["ledBrightness"] = ledBrightness;
      saveDoc["colorHour"] = colorHour;
      saveDoc["colorMinute"] = colorMinute;
      saveDoc["colorSecond"] = colorSecond;

      // Gravar no LittleFS
      File f = LittleFS.open("/settings.json", "w");
      if (!f) {
        request->send(500, "application/json", "{\"error\":\"failed to open file\"}");
        return;
      }
      if (serializeJson(saveDoc, f) == 0) {
        f.close();
        request->send(500, "application/json", "{\"error\":\"failed to write file\"}");
        return;
      }
      f.close();

      request->send(200, "application/json", "{\"status\":\"saved\"}");
      loadSettings();
    }
  );


  webServer.onNotFound(handleFileRequest);
  webServer.begin();
  Serial.println("HTTP server started on port 80");
  // End Webserver


  loadSettings();

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

  timeClient.begin();
  timerStatusLed.start();
  timerMostraHora.start();
}

void loop() {
  button1.tick();
  button2.tick();
  button3.tick();


  timeClient.update();
  timerStatusLed.update();
  timerMostraHora.update();
}

void click1() {
  Serial.println("Click 1");
}
void click2() {
  Serial.println("Click 2");
}
void click3() {
  Serial.println("Click 3");
}

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

void printTime() {
  Serial.println(timeClient.getFormattedTime());
}

void flipLed()
{
  ledStatus = !ledStatus;
  digitalWrite(8, ledStatus);
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

void displayBinary(const int *ledArray, int size, int number, uint32_t color)
{
  for (int i = 0; i < size; i++)
  {
    if (bitRead(number, i))
      strip.setPixelColor(ledArray[i], color);
  }
}

String getContentType(String filename) {
  if (filename.endsWith(".html")) return "text/html";
  if (filename.endsWith(".css")) return "text/css";
  if (filename.endsWith(".js")) return "application/javascript";
  if (filename.endsWith(".png")) return "image/png";
  if (filename.endsWith(".jpg")) return "image/jpeg";
  if (filename.endsWith(".ico")) return "image/x-icon";
  return "text/plain"; // Padrão para arquivos desconhecidos
}

void handleFileRequest(AsyncWebServerRequest *request) {
  // 1. Obter o caminho da URL
  String path = request->url();

  // Para o caminho raiz ("/") sempre tentamos servir o "index.html"
  if (path == "/") {
    path = "/index.html";
  }

  Serial.print("Tentativa de requisicao: ");
  Serial.println(path);

  // 2. Verificar se o arquivo existe no LittleFS
  if (LittleFS.exists(path)) {
    // 3. O arquivo existe: envia o conteúdo

    // Obter o tipo MIME
    String contentType = getContentType(path);

    // request->send() com LittleFS é a forma mais eficiente de servir arquivos grandes.
    // Ele gerencia automaticamente o cabeçalho Content-Length e a leitura do arquivo.
    request->send(LittleFS, path, contentType);
    Serial.println(" -> Arquivo servido com sucesso.");
  } else {
    // 4. O arquivo não existe: retorna 404
    request->send(404, "text/plain", "Erro 404: Arquivo nao encontrado no LittleFS.");
    Serial.print(" -> ERRO 404: Arquivo nao encontrado.");
  }
}

void loadSettings() {
  // 1. Verificar se o arquivo existe
  if (!LittleFS.exists("/settings.json")) {
    Serial.println("Arquivo de configurações não encontrado. Usando valores padrão.");
    return;
  }

  // 2. Abrir o arquivo para leitura
  File f = LittleFS.open("/settings.json", "r");
  if (!f) {
    Serial.println("Erro ao abrir arquivo de configurações.");
    return;
  }

  // 3. Deserializar o JSON do arquivo
  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, f);
  f.close();

  if (error) {
    Serial.print("Erro ao parsear JSON: ");
    Serial.println(error.c_str());
    return;
  }

  // 4. Extrair valores do JSON com conversão apropriada
  const char* ntpServer = doc["ntpServer"] | "pool.ntp.org";

  // Converter finalTimeZone para int
  int finalTimeZone = -3;
  if (doc.containsKey("finalTimeZone")) {
    if (doc["finalTimeZone"].is<int>()) {
      finalTimeZone = doc["finalTimeZone"].as<int>();
    } else {
      finalTimeZone = atoi(doc["finalTimeZone"].as<const char*>());
    }
  }

  // Converter ledBrightness para int
  int ledBrightness = 10;
  if (doc.containsKey("ledBrightness")) {
    if (doc["ledBrightness"].is<int>()) {
      ledBrightness = doc["ledBrightness"].as<int>();
    } else {
      ledBrightness = atoi(doc["ledBrightness"].as<const char*>());
    }
  }

  const char* colorHour = doc["colorHour"] | "#D2691E";
  const char* colorMinute = doc["colorMinute"] | "#D2691E";
  const char* colorSecond = doc["colorSecond"] | "#D2691E";

  // 5. Aplicar as configurações nas variáveis globais
  timeClient.setPoolServerName(ntpServer);
  timeClient.setTimeOffset(finalTimeZone * 3600); // converter para segundos
  strip.setBrightness(ledBrightness);
  strip.show();
  COLOR_HOURS_HEX = colorHour;
  COLOR_MINUTES_HEX = colorMinute;
  COLOR_SECONDS_HEX = colorSecond;

  // 6. Log de confirmação
  Serial.println("Configurações carregadas com sucesso!");
  Serial.print("NTP: ");
  Serial.println(ntpServer);
  Serial.print("TimeZone: ");
  Serial.println(finalTimeZone);
  Serial.print("Brilho: ");
  Serial.println(ledBrightness);
  Serial.print("Cor Horas: ");
  Serial.println(colorHour);
}