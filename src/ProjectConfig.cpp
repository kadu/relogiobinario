#include "ProjectConfig.h"

// Definição dos arrays de pins dos LEDs
const int ProjectConfig::HORAS_DEZENA[] = {10, 11};
const int ProjectConfig::HORAS_UNIDADE[] = {15, 14, 13, 12};
const int ProjectConfig::MINUTOS_DEZENA[] = {16, 17, 18};
const int ProjectConfig::MINUTOS_UNIDADE[] = {7, 8, 9, 19};
const int ProjectConfig::SEGUNDOS_DEZENA[] = {6, 5, 4};
const int ProjectConfig::SEGUNDOS_UNIDADE[] = {0, 1, 2, 3};

void ProjectConfig::printConfig() {
  Serial.println("=== Project Configuration ===");
  Serial.print("Number of LEDs: ");
  Serial.println(NUM_LEDS);
  Serial.print("Data Pin: ");
  Serial.println(DATA_PIN);
  Serial.print("Button 1 Pin: ");
  Serial.println(PIN_BUTTON01);
  Serial.print("Button 2 Pin: ");
  Serial.println(PIN_BUTTON02);
  Serial.print("Button 3 Pin: ");
  Serial.println(PIN_BUTTON03);
  Serial.print("Status LED Pin: ");
  Serial.println(STATUS_LED_PIN);
  Serial.println("==============================");
  
  Serial.println("=== LED Array Configuration ===");
  Serial.print("Horas Dezena: ");
  for (int i = 0; i < SIZE_HORAS_DEZENA; i++) {
    Serial.print(HORAS_DEZENA[i]);
    if (i < SIZE_HORAS_DEZENA - 1) Serial.print(", ");
  }
  Serial.println();
  
  Serial.print("Horas Unidade: ");
  for (int i = 0; i < SIZE_HORAS_UNIDADE; i++) {
    Serial.print(HORAS_UNIDADE[i]);
    if (i < SIZE_HORAS_UNIDADE - 1) Serial.print(", ");
  }
  Serial.println();
  
  Serial.print("Minutos Dezena: ");
  for (int i = 0; i < SIZE_MINUTOS_DEZENA; i++) {
    Serial.print(MINUTOS_DEZENA[i]);
    if (i < SIZE_MINUTOS_DEZENA - 1) Serial.print(", ");
  }
  Serial.println();
  
  Serial.print("Minutos Unidade: ");
  for (int i = 0; i < SIZE_MINUTOS_UNIDADE; i++) {
    Serial.print(MINUTOS_UNIDADE[i]);
    if (i < SIZE_MINUTOS_UNIDADE - 1) Serial.print(", ");
  }
  Serial.println();
  
  Serial.print("Segundos Dezena: ");
  for (int i = 0; i < SIZE_SEGUNDOS_DEZENA; i++) {
    Serial.print(SEGUNDOS_DEZENA[i]);
    if (i < SIZE_SEGUNDOS_DEZENA - 1) Serial.print(", ");
  }
  Serial.println();
  
  Serial.print("Segundos Unidade: ");
  for (int i = 0; i < SIZE_SEGUNDOS_UNIDADE; i++) {
    Serial.print(SEGUNDOS_UNIDADE[i]);
    if (i < SIZE_SEGUNDOS_UNIDADE - 1) Serial.print(", ");
  }
  Serial.println();
  Serial.println("===============================");
}