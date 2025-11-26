#ifndef LEDCONTROLLER_H
#define LEDCONTROLLER_H

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include "Config.h"

class LEDController {
private:
  Adafruit_NeoPixel* strip;
  int numLeds;
  int dataPin;
  Config* config;

  // Arrays de posições dos LEDs
  const int* horas_dezena;
  const int* horas_unidade;
  const int* minutos_dezena;
  const int* minutos_unidade;
  const int* segundos_dezena;
  const int* segundos_unidade;

  int tamanho_horas_dezena;
  int tamanho_horas_unidade;
  int tamanho_minutos_dezena;
  int tamanho_minutos_unidade;
  int tamanho_segundos_dezena;
  int tamanho_segundos_unidade;

public:
  LEDController(int numLeds, int dataPin, Config* configPtr);
  ~LEDController();

  // Inicialização
  void begin();
  void setLEDArrays(
    const int* hd, int szHd,
    const int* hu, int szHu,
    const int* md, int szMd,
    const int* mu, int szMu,
    const int* sd, int szSd,
    const int* su, int szSu
  );

  // Controle de brilho
  void setBrightness(int brightness);
  int getBrightness() const;

  // Exibição de hora
  void displayTime(int hours, int minutes, int seconds);
  void displayBinary(const int* ledArray, int size, int number, uint32_t color);

  // Controle geral
  void clear();
  void show();

  // Cores
  uint32_t colorFromHex(String hex);
  void setPixelColor(int index, uint32_t color);

  // Animações
  void animationStartup();
  void animationShutdown();

  // Debug
  void printStatus();
};

#endif