#ifndef PROJECTCONFIG_H
#define PROJECTCONFIG_H

#include <Arduino.h>

class ProjectConfig {
private:
  // Hardware pins
  static const int NUM_LEDS = 20;
  static const int DATA_PIN = 3;
  static const int PIN_BUTTON01 = 0;
  static const int PIN_BUTTON02 = 1;
  static const int PIN_BUTTON03 = 2;
  static const int STATUS_LED_PIN = 8;

  // LED Arrays para display binário
  static const int HORAS_DEZENA[];
  static const int HORAS_UNIDADE[];
  static const int MINUTOS_DEZENA[];
  static const int MINUTOS_UNIDADE[];
  static const int SEGUNDOS_DEZENA[];
  static const int SEGUNDOS_UNIDADE[];

  // Tamanhos dos arrays
  static const int SIZE_HORAS_DEZENA = 2;
  static const int SIZE_HORAS_UNIDADE = 4;
  static const int SIZE_MINUTOS_DEZENA = 3;
  static const int SIZE_MINUTOS_UNIDADE = 4;
  static const int SIZE_SEGUNDOS_DEZENA = 3;
  static const int SIZE_SEGUNDOS_UNIDADE = 4;

public:
  // Getters para Hardware Pins
  static int getNumLeds() { return NUM_LEDS; }
  static int getDataPin() { return DATA_PIN; }
  static int getButtonPin01() { return PIN_BUTTON01; }
  static int getButtonPin02() { return PIN_BUTTON02; }
  static int getButtonPin03() { return PIN_BUTTON03; }
  static int getStatusLedPin() { return STATUS_LED_PIN; }

  // Getters para LED Arrays
  static const int* getHorasDezenaPins() { return HORAS_DEZENA; }
  static const int* getHorasUnidadePins() { return HORAS_UNIDADE; }
  static const int* getMinutosDezenaPins() { return MINUTOS_DEZENA; }
  static const int* getMinutosUnidadePins() { return MINUTOS_UNIDADE; }
  static const int* getSegundosDezenaPins() { return SEGUNDOS_DEZENA; }
  static const int* getSegundosUnidadePins() { return SEGUNDOS_UNIDADE; }

  // Getters para tamanhos dos arrays
  static int getSizeHorasDezena() { return SIZE_HORAS_DEZENA; }
  static int getSizeHorasUnidade() { return SIZE_HORAS_UNIDADE; }
  static int getSizeMinutosDezena() { return SIZE_MINUTOS_DEZENA; }
  static int getSizeMinutosUnidade() { return SIZE_MINUTOS_UNIDADE; }
  static int getSizeSegundosDezena() { return SIZE_SEGUNDOS_DEZENA; }
  static int getSizeSegundosUnidade() { return SIZE_SEGUNDOS_UNIDADE; }

  // Debug
  static void printConfig();
};

#endif