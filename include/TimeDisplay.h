#ifndef TIMEDISPLAY_H
#define TIMEDISPLAY_H

#include <Arduino.h>
#include "LEDController.h"

class TimeDisplay {
private:
  LEDController* ledController;
  
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

  // Método auxiliar
  void displayBinaryColumn(const int* ledArray, int size, int number, uint32_t color);

public:
  TimeDisplay(LEDController* ledCtrl);
  
  // Configurar arrays de LEDs
  void setLEDArrays(
    const int* hd, int szHd,
    const int* hu, int szHu,
    const int* md, int szMd,
    const int* mu, int szMu,
    const int* sd, int szSd,
    const int* su, int szSu
  );

  // Exibir hora completa
  void displayTime(int hours, int minutes, int seconds);
  
  // Exibir apenas horas
  void displayHours(int hours);
  
  // Exibir apenas minutos
  void displayMinutes(int minutes);
  
  // Exibir apenas segundos
  void displaySeconds(int seconds);
  
  // Limpar display
  void clear();
  
  // Debug
  void printStatus();
};

#endif