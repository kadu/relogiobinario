#include "Config.h"
#include "LittleFS.h"

Config::Config()
  : ntpServer("pool.ntp.org"),
    timeZoneOffset(-3.0),
    ledBrightness(10),
    colorHour("#D2691E"),
    colorMinute("#D2691E"),
    colorSecond("#D2691E") {
}

void Config::loadFromFile() {
  if (!LittleFS.exists(CONFIG_FILE)) {
    Serial.println("Arquivo de configurações não encontrado. Usando valores padrão.");
    return;
  }

  File f = LittleFS.open(CONFIG_FILE, "r");
  if (!f) {
    Serial.println("Erro ao abrir arquivo de configurações.");
    return;
  }

  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, f);
  f.close();

  if (error) {
    Serial.print("Erro ao parsear JSON: ");
    Serial.println(error.c_str());
    return;
  }

  loadFromJson(doc.as<JsonObject>());
  printConfig();
}

void Config::saveToFile() {
  JsonDocument doc;
  saveToJson(doc);

  File f = LittleFS.open(CONFIG_FILE, "w");
  if (!f) {
    Serial.println("Erro ao abrir arquivo de configurações para escrita.");
    return;
  }

  if (serializeJson(doc, f) == 0) {
    Serial.println("Erro ao gravar configurações no arquivo.");
  } else {
    Serial.println("Configurações salvas com sucesso!");
  }
  f.close();
}

void Config::loadFromJson(const JsonObject& obj) {
  ntpServer = obj["ntpServer"] | "pool.ntp.org";

  // Usar o operador "ou" com um valor padrão. A biblioteca ArduinoJson
  // fará a conversão de tipo (string para int) automaticamente se necessário.
  timeZoneOffset = obj["finalTimeZone"] | -3.0;
  ledBrightness = obj["ledBrightness"] | 10;

  colorHour = obj["colorHour"] | "#D2691E";
  colorMinute = obj["colorMinute"] | "#D2691E";
  colorSecond = obj["colorSecond"] | "#D2691E";
}

void Config::saveToJson(JsonDocument& doc) {
  doc["ntpServer"] = ntpServer;
  doc["finalTimeZone"] = timeZoneOffset;
  doc["ledBrightness"] = ledBrightness;
  doc["colorHour"] = colorHour;
  doc["colorMinute"] = colorMinute;
  doc["colorSecond"] = colorSecond;
}

void Config::printConfig() {
  Serial.println("=== Configurações Carregadas ===");
  Serial.print("NTP Server: ");
  Serial.println(ntpServer);
  Serial.print("TimeZone Offset: ");
  Serial.println(timeZoneOffset);
  Serial.print("LED Brightness: ");
  Serial.println(ledBrightness);
  Serial.print("Cor Horas: ");
  Serial.println(colorHour);
  Serial.print("Cor Minutos: ");
  Serial.println(colorMinute);
  Serial.print("Cor Segundos: ");
  Serial.println(colorSecond);
  Serial.println("================================");
}