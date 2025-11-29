#ifndef CLOCKMANAGER_H
#define CLOCKMANAGER_H

#include <Arduino.h>
#include "NTPManager.h"
#include "TimeDisplay.h"
#include "ButtonManager.h"

class ClockManager {
public:
  enum OperationMode {
    OP_MODE_CLOCK = 0,
    OP_MODE_POMODORO = 1
  };

private:
  NTPManager* ntpManager;
  TimeDisplay* timeDisplay;
  ButtonManager* buttonManager;

  OperationMode operationMode;

  // Pomodoro state
  bool pomodoroRunning;
  unsigned long pomodoroDurationMs;
  unsigned long pomodoroRemainingMs;
  unsigned long pomodoroLastMillis;

  // Constantes de modo do display (mantidas)
  static const int MODE_TIME = 0;
  static const int MODE_HOURS = 1;
  static const int MODE_MINUTES = 2;
  static const int MODE_SECONDS = 3;

public:
  ClockManager(NTPManager* ntpMgr, TimeDisplay* timeDisp, ButtonManager* btnMgr);

  // Atualizar display baseado no modo / operação
  void update();

  // Operation mode control
  void toggleOperationMode();
  void setOperationMode(OperationMode m);
  OperationMode getOperationMode() const;
  bool isPomodoroMode() const { return operationMode == OP_MODE_POMODORO; }

  // Pomodoro control
  void startPausePomodoro();
  void startPomodoro();
  void pausePomodoro();
  void resetPomodoro();
  bool isPomodoroRunning() const { return pomodoroRunning; }

  // Getters de tempo (encaminha para NTP)
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