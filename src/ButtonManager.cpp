#include "ButtonManager.h"
#include "ClockManager.h"

// Variáveis globais para passar contexto aos callbacks
ButtonManager* g_buttonManager = nullptr;

// Funções callback globais (necessárias para OneButton)
void globalButton1Click() {
  if (g_buttonManager) g_buttonManager->handleButton1Click();
}

void globalButton2Click() {
  if (g_buttonManager) g_buttonManager->handleButton2Click();
}

// callback para long press (início do long press)
void globalButton2LongPress() {
  if (g_buttonManager) g_buttonManager->handleButton2LongPress();
}

void globalButton3Click() {
  if (g_buttonManager) g_buttonManager->handleButton3Click();
}

ButtonManager::ButtonManager(int pin1, int pin2, int pin3, TimeDisplay* timeDisp, LEDController* ledCtrl)
  : timeDisplay(timeDisp),
    ledController(ledCtrl),
    clockManager(nullptr),
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
  button2->attachLongPressStart(globalButton2LongPress); // attach long press start
  button3->attachClick(globalButton3Click);

  Serial.println("ButtonManager initialized");
}

void ButtonManager::handleButton1Click() {
  Serial.println("Button 1 clicked");
  // exemplo: aumentar brilho
  int currentBrightness = ledController->getBrightness();
  int newBrightness = currentBrightness + 25;
  if (newBrightness > 255) newBrightness = 255;
  ledController->setBrightness(newBrightness);
  Serial.print("Brightness increased to: ");
  Serial.println(newBrightness);
}

void ButtonManager::handleButton2Click() {
  Serial.println("Button 2 clicked (short)");
  // curto: altera modos internos do display (hora/hora/min/sec)
  displayMode++;
  if (displayMode > MODE_SECONDS) displayMode = MODE_TIME;
  Serial.print("Display mode changed to: ");
  Serial.println(displayMode);
}

void ButtonManager::handleButton2LongPress() {
  Serial.println("Button 2 long press detected - toggle operation mode");
  // Alterna modo de operação CLOCK <-> POMODORO via ClockManager
  if (clockManager) {
    clockManager->toggleOperationMode();
  }
}

void ButtonManager::handleButton3Click() {
  Serial.println("Button 3 clicked");
  // Se ClockManager estiver presente e estiver em modo Pomodoro, start/pause
  if (clockManager && clockManager->isPomodoroMode()) {
    clockManager->startPausePomodoro();
    return;
  }

  // Caso padrão: diminuir brilho
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
  Serial.print("Current Display Mode: ");
  Serial.println(displayMode);
  Serial.print("ClockManager linked: ");
  Serial.println(clockManager ? "Yes" : "No");
  Serial.println("============================");
}