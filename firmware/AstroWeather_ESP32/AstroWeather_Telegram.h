#pragma once

// SPDX-License-Identifier: AGPL-3.0-or-later
// Copyright (C) 2026 Tarun and contributors

#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <time.h>

WiFiClientSecure telegramClient;
UniversalTelegramBot *telegramBot = nullptr;
unsigned long lastTelegramPeriodic = 0;
unsigned long lastTelegramSuccess = 0;
bool telegramStateInitialised = false;
bool previousTelegramSafe = false;
bool previousTelegramRain = false;

bool telegramReady() {
  return settings.telegramEnabled &&
         settings.telegramToken.length() > 0 &&
         settings.telegramChatId.length() > 0 &&
         WiFi.status() == WL_CONNECTED &&
         time(nullptr) > 1700000000;
}

String telegramTimestamp() {
  struct tm timeInfo;
  if (!getLocalTime(&timeInfo, 50)) return "time unavailable";
  char buffer[24];
  strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M", &timeInfo);
  return String(buffer);
}

String telegramWeatherMessage(const String &heading) {
  String message;
  message.reserve(600);
  message += heading + "\n";
  message += "AstroWeather India - " + telegramTimestamp() + "\n\n";
  message += "Temperature: " + numOrNull(airC, 1) + " C\n";
  message += "Humidity: " + numOrNull(humidity, 1) + " %\n";
  message += "Dew point: " + numOrNull(dewC, 1) + " C\n";
  message += "Dew margin: " + numOrNull(airC - dewC, 1) + " C\n";
  message += "Cloud cover: " + numOrNull(cloudPercent, 0) + " %\n";
  message += "Clarity: " + numOrNull(clarityScore, 0) + " / 100\n";
  message += "SQM estimate: ";
  message += isfinite(sqmEstimate) ? String(sqmEstimate, 2) : "not calibrated";
  message += "\nEstimated Bortle: ";
  message += bortleEstimate > 0 ? String(bortleEstimate) : "unavailable";
  message += "\nRain: ";
  message += rainDetected ? "WET" : "dry";
  message += "\nStatus: ";
  message += advisorySafe() ? "ADVISORY SAFE" : "ADVISORY UNSAFE";
  return message;
}

bool telegramSend(const String &message) {
  if (!telegramReady() || !telegramBot) return false;
  bool sent = telegramBot->sendMessage(settings.telegramChatId, message, "");
  if (sent) lastTelegramSuccess = millis();
  return sent;
}

bool telegramSendTest() {
  return telegramSend(telegramWeatherMessage("TEST MESSAGE"));
}

void telegramBegin() {
  if (!settings.telegramEnabled || !settings.telegramToken.length()) return;
  telegramClient.setCACert(TELEGRAM_CERTIFICATE_ROOT);
  telegramClient.setHandshakeTimeout(15);
  telegramBot = new UniversalTelegramBot(settings.telegramToken, telegramClient);
  configTime(settings.utcOffsetMinutes * 60, 0,
             "pool.ntp.org", "time.nist.gov");
  struct tm initialTime;
  getLocalTime(&initialTime, 5000);
  lastTelegramPeriodic = millis();
}

void telegramLoop() {
  if (!telegramReady() || !telegramBot) return;

  bool safeNow = advisorySafe();
  bool rainNow = rainDetected;
  if (!telegramStateInitialised) {
    previousTelegramSafe = safeNow;
    previousTelegramRain = rainNow;
    telegramStateInitialised = true;
    if (settings.notifyStartup)
      telegramSend(telegramWeatherMessage("STATION ONLINE"));
    return;
  }

  if (rainNow && !previousTelegramRain && settings.notifyRain) {
    telegramSend(telegramWeatherMessage("RAIN DETECTED"));
  } else if (!safeNow && previousTelegramSafe && settings.notifyUnsafe) {
    telegramSend(telegramWeatherMessage("WEATHER UNSAFE"));
  } else if (safeNow && !previousTelegramSafe && settings.notifyRecovery) {
    telegramSend(telegramWeatherMessage("CONDITIONS RECOVERED"));
  }
  previousTelegramSafe = safeNow;
  previousTelegramRain = rainNow;

  if (settings.reportMinutes > 0) {
    unsigned long interval = (unsigned long)settings.reportMinutes * 60000UL;
    if (millis() - lastTelegramPeriodic >= interval) {
      telegramSend(telegramWeatherMessage("REGULAR WEATHER REPORT"));
      lastTelegramPeriodic = millis();
    }
  }
}
