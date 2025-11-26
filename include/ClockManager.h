#ifndef CLOCKMANAGER_H
#define CLOCKMANAGER_H

#include <Arduino.h>
#include "NTPManager.h"
#include "TimeDisplay.h"
#include "ButtonManager.h"

class ClockManager {
private:
  NTPManager* ntpManager;
  TimeDisplay* timeDisplay;
  ButtonManager* buttonManager;

  // Constantes de modo
  static const int MODE_TIME = 0;
  static const int MODE_HOURS = 1;
  static const int MODE_MINUTES = 2;
  static const int MODE_SECONDS = 3;

public:
  ClockManager(NTPManager* ntpMgr, TimeDisplay* timeDisp, ButtonManager* btnMgr);
  
  // Atualizar display baseado no modo
  void update();
  
  // Métodos para controle de modo
  void setMode(int mode);
  int getMode() const;
  void nextMode();
  void previousMode();
  
  // Getters de tempo
  int getHours() const;
  int getMinutes() const;
  int getSeconds() const;
  
  // Exibição direta
  void displayFullTime();
  void displayHoursOnly();
  void displayMinutesOnly();
  void displaySecondsOnly();
  
  // Debug
  void printStatus();
};

#endif