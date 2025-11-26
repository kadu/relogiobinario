#include "TimeDisplay.h"
#include "Config.h"

TimeDisplay::TimeDisplay(LEDController* ledCtrl)
  : ledController(ledCtrl),
    horas_dezena(nullptr),
    horas_unidade(nullptr),
    minutos_dezena(nullptr),
    minutos_unidade(nullptr),
    segundos_dezena(nullptr),
    segundos_unidade(nullptr) {
}

void TimeDisplay::setLEDArrays(
    const int* hd, int szHd,
    const int* hu, int szHu,
    const int* md, int szMd,
    const int* mu, int szMu,
    const int* sd, int szSd,
    const int* su, int szSu) {
  horas_dezena = hd;
  tamanho_horas_dezena = szHd;
  horas_unidade = hu;
  tamanho_horas_unidade = szHu;
  minutos_dezena = md;
  tamanho_minutos_dezena = szMd;
  minutos_unidade = mu;
  tamanho_minutos_unidade = szMu;
  segundos_dezena = sd;
  tamanho_segundos_dezena = szSd;
  segundos_unidade = su;
  tamanho_segundos_unidade = szSu;
  
  Serial.println("TimeDisplay LED arrays configured");
}

void TimeDisplay::displayBinaryColumn(const int* ledArray, int size, int number, uint32_t color) {
  for (int i = 0; i < size; i++) {
    if (bitRead(number, i)) {
      ledController->setPixelColor(ledArray[i], color);
    }
  }
}

void TimeDisplay::displayTime(int hours, int minutes, int seconds) {
  // Obter referência ao config para pegar cores
  extern Config config;
  
  ledController->clear();
  
  // Exibir segundos
  displayBinaryColumn(segundos_unidade, tamanho_segundos_unidade, seconds % 10, 
                      ledController->colorFromHex(config.getColorSecond()));
  displayBinaryColumn(segundos_dezena, tamanho_segundos_dezena, seconds / 10, 
                      ledController->colorFromHex(config.getColorSecond()));
  
  // Exibir minutos
  displayBinaryColumn(minutos_unidade, tamanho_minutos_unidade, minutes % 10, 
                      ledController->colorFromHex(config.getColorMinute()));
  displayBinaryColumn(minutos_dezena, tamanho_minutos_dezena, minutes / 10, 
                      ledController->colorFromHex(config.getColorMinute()));
  
  // Exibir horas
  displayBinaryColumn(horas_unidade, tamanho_horas_unidade, hours % 10, 
                      ledController->colorFromHex(config.getColorHour()));
  displayBinaryColumn(horas_dezena, tamanho_horas_dezena, hours / 10, 
                      ledController->colorFromHex(config.getColorHour()));
  
  ledController->show();
}

void TimeDisplay::displayHours(int hours) {
  extern Config config;
  
  ledController->clear();
  displayBinaryColumn(horas_unidade, tamanho_horas_unidade, hours % 10, 
                      ledController->colorFromHex(config.getColorHour()));
  displayBinaryColumn(horas_dezena, tamanho_horas_dezena, hours / 10, 
                      ledController->colorFromHex(config.getColorHour()));
  ledController->show();
}

void TimeDisplay::displayMinutes(int minutes) {
  extern Config config;
  
  ledController->clear();
  displayBinaryColumn(minutos_unidade, tamanho_minutos_unidade, minutes % 10, 
                      ledController->colorFromHex(config.getColorMinute()));
  displayBinaryColumn(minutos_dezena, tamanho_minutos_dezena, minutes / 10, 
                      ledController->colorFromHex(config.getColorMinute()));
  ledController->show();
}

void TimeDisplay::displaySeconds(int seconds) {
  extern Config config;
  
  ledController->clear();
  displayBinaryColumn(segundos_unidade, tamanho_segundos_unidade, seconds % 10, 
                      ledController->colorFromHex(config.getColorSecond()));
  displayBinaryColumn(segundos_dezena, tamanho_segundos_dezena, seconds / 10, 
                      ledController->colorFromHex(config.getColorSecond()));
  ledController->show();
}

void TimeDisplay::clear() {
  ledController->clear();
  ledController->show();
}

void TimeDisplay::printStatus() {
  Serial.println("=== TimeDisplay Status ===");
  Serial.println("Horas dezena: configured");
  Serial.println("Horas unidade: configured");
  Serial.println("Minutos dezena: configured");
  Serial.println("Minutos unidade: configured");
  Serial.println("Segundos dezena: configured");
  Serial.println("Segundos unidade: configured");
  Serial.println("==========================");
}