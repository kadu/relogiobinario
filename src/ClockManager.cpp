#include "ClockManager.h"

ClockManager::ClockManager(NTPManager* ntpMgr, TimeDisplay* timeDisp, ButtonManager* btnMgr)
  : ntpManager(ntpMgr),
    timeDisplay(timeDisp),
    buttonManager(btnMgr) {
}

void ClockManager::update() {
  if (!ntpManager || !timeDisplay || !buttonManager) {
    Serial.println("Error: ClockManager dependencies not initialized");
    return;
  }

  int hours = getHours();
  int minutes = getMinutes();
  int seconds = getSeconds();
  int mode = getMode();

  switch(mode) {
    case MODE_TIME:
      displayFullTime();
      break;
    case MODE_HOURS:
      displayHoursOnly();
      break;
    case MODE_MINUTES:
      displayMinutesOnly();
      break;
    case MODE_SECONDS:
      displaySecondsOnly();
      break;
    default:
      displayFullTime();
  }
}

void ClockManager::setMode(int mode) {
  if (buttonManager) {
    buttonManager->setDisplayMode(mode);
  }
}

int ClockManager::getMode() const {
  if (buttonManager) {
    return buttonManager->getDisplayMode();
  }
  return MODE_TIME;
}

void ClockManager::nextMode() {
  int currentMode = getMode();
  int nextMode = (currentMode + 1) % 4; // 4 modos: 0, 1, 2, 3
  setMode(nextMode);
  Serial.print("Display mode changed to: ");
  Serial.println(nextMode);
}

void ClockManager::previousMode() {
  int currentMode = getMode();
  int prevMode = (currentMode - 1 + 4) % 4; // 4 modos com wrap-around
  setMode(prevMode);
  Serial.print("Display mode changed to: ");
  Serial.println(prevMode);
}

int ClockManager::getHours() const {
  if (ntpManager) {
    return ntpManager->getHours();
  }
  return 0;
}

int ClockManager::getMinutes() const {
  if (ntpManager) {
    return ntpManager->getMinutes();
  }
  return 0;
}

int ClockManager::getSeconds() const {
  if (ntpManager) {
    return ntpManager->getSeconds();
  }
  return 0;
}

void ClockManager::displayFullTime() {
  if (timeDisplay) {
    timeDisplay->displayTime(getHours(), getMinutes(), getSeconds());
  }
}

void ClockManager::displayHoursOnly() {
  if (timeDisplay) {
    timeDisplay->displayHours(getHours());
  }
}

void ClockManager::displayMinutesOnly() {
  if (timeDisplay) {
    timeDisplay->displayMinutes(getMinutes());
  }
}

void ClockManager::displaySecondsOnly() {
  if (timeDisplay) {
    timeDisplay->displaySeconds(getSeconds());
  }
}

void ClockManager::printStatus() {
  Serial.println("=== ClockManager Status ===");
  Serial.print("Current Mode: ");
  Serial.println(getMode());
  Serial.print("Current Time: ");
  Serial.print(getHours());
  Serial.print(":");
  Serial.print(getMinutes());
  Serial.print(":");
  Serial.println(getSeconds());
  Serial.println("===========================");
}