#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>
#include <ArduinoJson.h>

class Config {
private:
  // Variáveis privadas
  String ntpServer;
  int timeZoneOffset;
  int ledBrightness;
  String colorHour;
  String colorMinute;
  String colorSecond;

  const char* CONFIG_FILE = "/settings.json";

public:
  Config();

  // Métodos de leitura
  void loadFromFile();
  void saveToFile();

  // Getters
  String getNtpServer() const { return ntpServer; }
  int getTimeZoneOffset() const { return timeZoneOffset; }
  int getLedBrightness() const { return ledBrightness; }
  String getColorHour() const { return colorHour; }
  String getColorMinute() const { return colorMinute; }
  String getColorSecond() const { return colorSecond; }

  // Setters
  void setNtpServer(const String& server) { ntpServer = server; }
  void setTimeZoneOffset(int offset) { timeZoneOffset = offset; }
  void setLedBrightness(int brightness) { ledBrightness = brightness; }
  void setColorHour(const String& color) { colorHour = color; }
  void setColorMinute(const String& color) { colorMinute = color; }
  void setColorSecond(const String& color) { colorSecond = color; }

  // Método para carregar JSON
  void loadFromJson(const JsonObject& obj);

  // Método para salvar JSON
  void saveToJson(JsonDocument& doc);

  // Debug
  void printConfig();
};

#endif