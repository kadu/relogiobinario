#include "ClockManager.h"

ClockManager::ClockManager(NTPManager* ntpMgr, TimeDisplay* timeDisp, ButtonManager* btnMgr)
  : ntpManager(ntpMgr),
    timeDisplay(timeDisp),
    buttonManager(btnMgr),
    operationMode(OP_MODE_CLOCK),
    pomodoroRunning(false),
    pomodoroDurationMs(25UL * 60UL * 1000UL), // padrão 25 minutos
    pomodoroRemainingMs(pomodoroDurationMs),
    pomodoroLastMillis(0) {
}

void ClockManager::update() {
  if (!ntpManager || !timeDisplay || !buttonManager) {
    Serial.println("Error: ClockManager dependencies not initialized");
    return;
  }

  if (operationMode == OP_MODE_CLOCK) {
    // comportamento normal do relógio
    displayFullTime();
    return;
  }

  // modo POMODORO
  unsigned long now = millis();
  if (pomodoroRunning) {
    if (pomodoroLastMillis == 0) pomodoroLastMillis = now;
    unsigned long elapsed = now - pomodoroLastMillis;
    if (elapsed > 0) {
      if (elapsed >= pomodoroRemainingMs) {
        pomodoroRemainingMs = 0;
        pomodoroRunning = false;
        pomodoroLastMillis = 0;
        // sinal de término (pode animar LEDs)
        timeDisplay->clear();
        // por enquanto mostrar 00:00
        timeDisplay->displayTime(0, 0, 0);
        Serial.println("Pomodoro finished");
      } else {
        pomodoroRemainingMs -= elapsed;
        pomodoroLastMillis = now;
      }
    }
  }

  // Exibir tempo restante como MM:SS (usando displayTime(0,MM,SS))
  unsigned long remainingSec = pomodoroRemainingMs / 1000UL;
  int minutes = remainingSec / 60UL;
  int seconds = remainingSec % 60UL;
  timeDisplay->displayTime(0, minutes, seconds);
}

void ClockManager::toggleOperationMode() {
  if (operationMode == OP_MODE_CLOCK) {
    setOperationMode(OP_MODE_POMODORO);
  } else {
    setOperationMode(OP_MODE_CLOCK);
  }
  Serial.print("Operation mode now: ");
  Serial.println(operationMode == OP_MODE_POMODORO ? "POMODORO" : "CLOCK");
}

void ClockManager::setOperationMode(ClockManager::OperationMode m) {
  operationMode = m;
  // Ao entrar em modo pomodoro, resetar timer se necessário
  if (operationMode == OP_MODE_POMODORO) {
    resetPomodoro();
  } else {
    // voltar ao modo relógio limpa estado pomodoro
    pomodoroRunning = false;
    pomodoroRemainingMs = pomodoroDurationMs;
    pomodoroLastMillis = 0;
  }
}

ClockManager::OperationMode ClockManager::getOperationMode() const {
  return operationMode;
}

void ClockManager::startPausePomodoro() {
  if (operationMode != OP_MODE_POMODORO) return;

  if (pomodoroRunning) {
    pausePomodoro();
  } else {
    startPomodoro();
  }
}

void ClockManager::startPomodoro() {
  if (pomodoroRemainingMs == 0) {
    // se terminou, reset antes de iniciar
    pomodoroRemainingMs = pomodoroDurationMs;
  }
  pomodoroRunning = true;
  pomodoroLastMillis = millis();
  Serial.println("Pomodoro started");
}

void ClockManager::pausePomodoro() {
  pomodoroRunning = false;
  pomodoroLastMillis = 0;
  Serial.println("Pomodoro paused");
}

void ClockManager::resetPomodoro() {
  pomodoroRunning = false;
  pomodoroRemainingMs = pomodoroDurationMs;
  pomodoroLastMillis = 0;
  Serial.println("Pomodoro reset");
}

int ClockManager::getHours() const {
  if (ntpManager) return ntpManager->getHours();
  return 0;
}

int ClockManager::getMinutes() const {
  if (ntpManager) return ntpManager->getMinutes();
  return 0;
}

int ClockManager::getSeconds() const {
  if (ntpManager) return ntpManager->getSeconds();
  return 0;
}

void ClockManager::displayFullTime() {
  timeDisplay->displayTime(getHours(), getMinutes(), getSeconds());
}

void ClockManager::displayHoursOnly() {
  timeDisplay->displayHours(getHours());
}

void ClockManager::displayMinutesOnly() {
  timeDisplay->displayMinutes(getMinutes());
}

void ClockManager::displaySecondsOnly() {
  timeDisplay->displaySeconds(getSeconds());
}

void ClockManager::printStatus() {
  Serial.println("=== ClockManager Status ===");
  Serial.print("Operation Mode: ");
  Serial.println(operationMode == OP_MODE_POMODORO ? "POMODORO" : "CLOCK");
  Serial.print("Pomodoro running: ");
  Serial.println(pomodoroRunning ? "Yes" : "No");
  Serial.print("Pomodoro remaining (s): ");
  Serial.println(pomodoroRemainingMs / 1000UL);
  Serial.println("===========================");
}