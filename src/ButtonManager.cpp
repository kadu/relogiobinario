#include "ButtonManager.h"

// Variáveis globais para passar contexto aos callbacks
ButtonManager* g_buttonManager = nullptr;

// Funções callback globais (necessárias para OneButton)
void globalButton1Click() {
  if (g_buttonManager) g_buttonManager->handleButton1Click();
}

void globalButton2Click() {
  if (g_buttonManager) g_buttonManager->handleButton2Click();
}

void globalButton3Click() {
  if (g_buttonManager) g_buttonManager->handleButton3Click();
}

ButtonManager::ButtonManager(int pin1, int pin2, int pin3, TimeDisplay* timeDisp, LEDController* ledCtrl)
  : timeDisplay(timeDisp),
    ledController(ledCtrl),
    displayMode(MODE_TIME) {

  button1 = new OneButton(pin1, true);
  button2 = new OneButton(pin2, true);
  button3 = new OneButton(pin3, true);

  g_buttonManager = this;
}

ButtonManager::~ButtonManager() {
  if (button1) delete button1;
  if (button2) delete button2;
  if (button3) delete button3;
}

void ButtonManager::init() {
  // Anexar callbacks
  button1->attachClick(globalButton1Click);
  button2->attachClick(globalButton2Click);
  button3->attachClick(globalButton3Click);

  Serial.println("ButtonManager initialized");
}

void ButtonManager::handleButton1Click() {
  Serial.println("Button 1 clicked");

  // Aumentar brilho
  int currentBrightness = ledController->getBrightness();
  int newBrightness = currentBrightness + 25;
  if (newBrightness > 255) newBrightness = 255;

  ledController->setBrightness(newBrightness);
  Serial.print("Brightness increased to: ");
  Serial.println(newBrightness);
}

void ButtonManager::handleButton2Click() {
  Serial.println("Button 2 clicked");

  // Alternar modo de exibição
  displayMode++;
  if (displayMode > MODE_SECONDS) displayMode = MODE_TIME;

  Serial.print("Display mode changed to: ");
  Serial.println(displayMode);

  // Aqui você pode disparar diferentes exibições baseado no modo
  // Isso será tratado no loop principal
}

void ButtonManager::handleButton3Click() {
  Serial.println("Button 3 clicked");

  // Diminuir brilho
  int currentBrightness = ledController->getBrightness();
  int newBrightness = currentBrightness - 25;
  if (newBrightness < 10) newBrightness = 10;

  ledController->setBrightness(newBrightness);
  Serial.print("Brightness decreased to: ");
  Serial.println(newBrightness);
}

void ButtonManager::update() {
  button1->tick();
  button2->tick();
  button3->tick();
}

void ButtonManager::printStatus() {
  Serial.println("=== ButtonManager Status ===");
  Serial.print("Button 1 Pin: configured");
  Serial.println();
  Serial.print("Button 2 Pin: configured");
  Serial.println();
  Serial.print("Button 3 Pin: configured");
  Serial.println();
  Serial.print("Current Display Mode: ");
  Serial.println(displayMode);
  Serial.println("============================");
}