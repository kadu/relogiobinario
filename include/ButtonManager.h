#ifndef BUTTONMANAGER_H
#define BUTTONMANAGER_H

#include <Arduino.h>
#include <OneButton.h>
#include "TimeDisplay.h"
#include "LEDController.h"

typedef void (*ButtonCallback)();

class ButtonManager {
private:
  OneButton* button1;
  OneButton* button2;
  OneButton* button3;

  TimeDisplay* timeDisplay;
  LEDController* ledController;

  // Variável de controle de modo
  int displayMode;
  const int MODE_TIME = 0;
  const int MODE_HOURS = 1;
  const int MODE_MINUTES = 2;
  const int MODE_SECONDS = 3;

public:
  ButtonManager(int pin1, int pin2, int pin3, TimeDisplay* timeDisp, LEDController* ledCtrl);
  ~ButtonManager();

  // Inicializar botões
  void init();

  // Atualizar estado dos botões (chamar no loop)
  void update();

  // Métodos públicos para callbacks
  void handleButton1Click();
  void handleButton2Click();
  void handleButton3Click();

  // Getters
  int getDisplayMode() const { return displayMode; }

  // Setters
  void setDisplayMode(int mode) { displayMode = mode; }

  // Debug
  void printStatus();
};

#endif