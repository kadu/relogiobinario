#include "LEDController.h"

LEDController::LEDController(int numLeds, int dataPin, Config* configPtr)
  : numLeds(numLeds),
    dataPin(dataPin),
    config(configPtr),
    horas_dezena(nullptr),
    horas_unidade(nullptr),
    minutos_dezena(nullptr),
    minutos_unidade(nullptr),
    segundos_dezena(nullptr),
    segundos_unidade(nullptr) {
  strip = new Adafruit_NeoPixel(numLeds, dataPin, NEO_GRB + NEO_KHZ800);
}

LEDController::~LEDController() {
  if (strip) {
    delete strip;
  }
}

void LEDController::begin() {
  strip->begin();
  strip->clear();
  strip->setBrightness(config->getLedBrightness());
  strip->show();
  Serial.println("LED Controller initialized");
}

void LEDController::setLEDArrays(
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
}

void LEDController::setBrightness(int brightness) {
  strip->setBrightness(brightness);
  strip->show();
}

int LEDController::getBrightness() const {
  return strip->getBrightness();
}

uint32_t LEDController::colorFromHex(String hex) {
  if (hex.startsWith("#"))
    hex = hex.substring(1);
  long number = strtol(hex.c_str(), NULL, 16);
  byte r = (number >> 16) & 0xFF;
  byte g = (number >> 8) & 0xFF;
  byte b = number & 0xFF;
  return strip->Color(r, g, b);
}

void LEDController::displayBinary(const int* ledArray, int size, int number, uint32_t color) {
  for (int i = 0; i < size; i++) {
    if (bitRead(number, i))
      strip->setPixelColor(ledArray[i], color);
  }
}

void LEDController::displayTime(int hours, int minutes, int seconds) {
  clear();

  displayBinary(segundos_unidade, tamanho_segundos_unidade, seconds % 10, colorFromHex(config->getColorSecond()));
  displayBinary(segundos_dezena, tamanho_segundos_dezena, seconds / 10, colorFromHex(config->getColorSecond()));
  displayBinary(minutos_unidade, tamanho_minutos_unidade, minutes % 10, colorFromHex(config->getColorMinute()));
  displayBinary(minutos_dezena, tamanho_minutos_dezena, minutes / 10, colorFromHex(config->getColorMinute()));
  displayBinary(horas_unidade, tamanho_horas_unidade, hours % 10, colorFromHex(config->getColorHour()));
  displayBinary(horas_dezena, tamanho_horas_dezena, hours / 10, colorFromHex(config->getColorHour()));

  show();
}

void LEDController::clear() {
  strip->clear();
}

void LEDController::show() {
  strip->show();
}

void LEDController::setPixelColor(int index, uint32_t color) {
  strip->setPixelColor(index, color);
}

void LEDController::animationStartup() {
  for (size_t i = 0; i < numLeds; i++) {
    strip->setPixelColor(i, colorFromHex(config->getColorHour()));
    strip->show();
    delay(50);
  }
  delay(150);
  for (size_t i = 0; i < numLeds; i++) {
    strip->setPixelColor(numLeds - i, colorFromHex("#000000"));
    strip->show();
    delay(50);
  }
  delay(300);
  clear();
  show();
}

void LEDController::animationShutdown() {
  for (size_t i = 0; i < numLeds; i++) {
    strip->setPixelColor(i, colorFromHex("#FF0000"));
    strip->show();
    delay(30);
  }
  delay(200);
  clear();
  show();
}

void LEDController::printStatus() {
  Serial.println("=== LED Controller Status ===");
  Serial.print("Number of LEDs: ");
  Serial.println(numLeds);
  Serial.print("Data Pin: ");
  Serial.println(dataPin);
  Serial.print("Brightness: ");
  Serial.println(getBrightness());
  Serial.println("=============================");
}