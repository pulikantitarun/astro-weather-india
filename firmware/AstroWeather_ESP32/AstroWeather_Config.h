#pragma once

// SPDX-License-Identifier: AGPL-3.0-or-later
// Copyright (C) 2026 Tarun and contributors

#include <Preferences.h>

#ifndef DEFAULT_ADMIN_PASSWORD
#define DEFAULT_ADMIN_PASSWORD "astroadmin"
#endif

#ifndef DEFAULT_SETUP_AP_PASSWORD
#define DEFAULT_SETUP_AP_PASSWORD "astroclear"
#endif

struct AstroSettings {
  String hostname = "astroweather";
  String wifiSsid;
  String wifiPassword;
  String setupApPassword = DEFAULT_SETUP_AP_PASSWORD;
  String adminPassword = DEFAULT_ADMIN_PASSWORD;

  bool telegramEnabled = false;
  String telegramToken;
  String telegramChatId;
  uint16_t reportMinutes = 60;
  bool notifyUnsafe = true;
  bool notifyRecovery = true;
  bool notifyRain = true;
  bool notifyStartup = true;
  int utcOffsetMinutes = 330; // India Standard Time by default.

  uint16_t rainLatchMinutes = 10;
};

AstroSettings settings;
unsigned long settingsRestartAt = 0;
String settingsCsrfToken;
bool telegramSendTest();

bool isPlaceholderCredential(const String &value) {
  return value.length() == 0 || value.startsWith("YOUR_");
}

String htmlEscape(const String &input) {
  String output;
  output.reserve(input.length() + 16);
  for (size_t i = 0; i < input.length(); ++i) {
    switch (input[i]) {
      case '&': output += F("&amp;"); break;
      case '<': output += F("&lt;"); break;
      case '>': output += F("&gt;"); break;
      case '"': output += F("&quot;"); break;
      case '\'': output += F("&#39;"); break;
      default: output += input[i];
    }
  }
  return output;
}

bool validHostname(const String &hostname) {
  if (hostname.length() < 1 || hostname.length() > 32 ||
      hostname[0] == '-' || hostname[hostname.length() - 1] == '-') return false;
  for (size_t i = 0; i < hostname.length(); ++i) {
    char c = hostname[i];
    if (!isalnum((unsigned char)c) && c != '-') return false;
  }
  return true;
}

bool settingsAuthenticated() {
  if (server.authenticate("admin", settings.adminPassword.c_str())) return true;
  server.requestAuthentication(BASIC_AUTH, "AstroWeather settings");
  return false;
}

bool settingsCsrfValid() {
  return server.hasArg("csrf") && server.arg("csrf") == settingsCsrfToken;
}

void applySettingsToRuntime() {
  HUMIDITY_LIMIT = clampf(HUMIDITY_LIMIT, 1.0f, 100.0f);
  DEW_MARGIN_LIMIT_C = clampf(DEW_MARGIN_LIMIT_C, 0.0f, 20.0f);
  CLOUD_LIMIT_PERCENT = clampf(CLOUD_LIMIT_PERCENT, 0.0f, 100.0f);
  RAIN_THRESHOLD_RAW = constrain(RAIN_THRESHOLD_RAW, 0, 4095);
  settings.rainLatchMinutes =
      constrain((int)settings.rainLatchMinutes, 1, 120);
}

void loadSettings() {
  char csrf[17];
  snprintf(csrf, sizeof(csrf), "%08lx%08lx",
           (unsigned long)esp_random(), (unsigned long)esp_random());
  settingsCsrfToken = csrf;

  Preferences prefs;
  prefs.begin("astroweather", true);
  settings.hostname = prefs.getString("host", "astroweather");
  settings.wifiSsid = prefs.getString("wifi_ssid", "");
  settings.wifiPassword = prefs.getString("wifi_pass", "");
  settings.setupApPassword =
      prefs.getString("ap_pass", DEFAULT_SETUP_AP_PASSWORD);
  settings.adminPassword =
      prefs.getString("admin_pass", DEFAULT_ADMIN_PASSWORD);

  settings.telegramEnabled = prefs.getBool("tg_enable", false);
  settings.telegramToken = prefs.getString("tg_token", "");
  settings.telegramChatId = prefs.getString("tg_chat", "");
  settings.reportMinutes = prefs.getUShort("tg_mins", 60);
  settings.notifyUnsafe = prefs.getBool("tg_unsafe", true);
  settings.notifyRecovery = prefs.getBool("tg_safe", true);
  settings.notifyRain = prefs.getBool("tg_rain", true);
  settings.notifyStartup = prefs.getBool("tg_start", true);
  settings.utcOffsetMinutes = prefs.getInt("utc_offset", 330);

  CLEAR_DELTA_C = prefs.getFloat("clear_delta", CLEAR_DELTA_C);
  OVERCAST_DELTA_C = prefs.getFloat("cloud_delta", OVERCAST_DELTA_C);
  HUMIDITY_LIMIT = prefs.getFloat("humid_limit", HUMIDITY_LIMIT);
  DEW_MARGIN_LIMIT_C = prefs.getFloat("dew_limit", DEW_MARGIN_LIMIT_C);
  CLOUD_LIMIT_PERCENT = prefs.getFloat("cloud_limit", CLOUD_LIMIT_PERCENT);
  RAIN_THRESHOLD_RAW = prefs.getInt("rain_raw", RAIN_THRESHOLD_RAW);
  settings.rainLatchMinutes = prefs.getUShort("rain_latch", 10);
  SQM_CAL_OFFSET = prefs.getFloat("sqm_offset", SQM_CAL_OFFSET);
  prefs.end();

  if (settings.wifiSsid.length() == 0 &&
      !isPlaceholderCredential(String(WIFI_SSID))) {
    settings.wifiSsid = WIFI_SSID;
    settings.wifiPassword = WIFI_PASSWORD;
  }
  if (!validHostname(settings.hostname)) settings.hostname = "astroweather";
  if (settings.setupApPassword.length() < 8)
    settings.setupApPassword = DEFAULT_SETUP_AP_PASSWORD;
  if (settings.adminPassword.length() < 8)
    settings.adminPassword = DEFAULT_ADMIN_PASSWORD;
  if (settings.reportMinutes != 0 && settings.reportMinutes != 30 &&
      settings.reportMinutes != 60) settings.reportMinutes = 60;
  settings.utcOffsetMinutes = constrain(settings.utcOffsetMinutes, -720, 840);
  applySettingsToRuntime();
}

void saveSettingsToNvs() {
  Preferences prefs;
  prefs.begin("astroweather", false);
  prefs.putString("host", settings.hostname);
  prefs.putString("wifi_ssid", settings.wifiSsid);
  prefs.putString("wifi_pass", settings.wifiPassword);
  prefs.putString("ap_pass", settings.setupApPassword);
  prefs.putString("admin_pass", settings.adminPassword);

  prefs.putBool("tg_enable", settings.telegramEnabled);
  prefs.putString("tg_token", settings.telegramToken);
  prefs.putString("tg_chat", settings.telegramChatId);
  prefs.putUShort("tg_mins", settings.reportMinutes);
  prefs.putBool("tg_unsafe", settings.notifyUnsafe);
  prefs.putBool("tg_safe", settings.notifyRecovery);
  prefs.putBool("tg_rain", settings.notifyRain);
  prefs.putBool("tg_start", settings.notifyStartup);
  prefs.putInt("utc_offset", settings.utcOffsetMinutes);

  prefs.putFloat("clear_delta", CLEAR_DELTA_C);
  prefs.putFloat("cloud_delta", OVERCAST_DELTA_C);
  prefs.putFloat("humid_limit", HUMIDITY_LIMIT);
  prefs.putFloat("dew_limit", DEW_MARGIN_LIMIT_C);
  prefs.putFloat("cloud_limit", CLOUD_LIMIT_PERCENT);
  prefs.putInt("rain_raw", RAIN_THRESHOLD_RAW);
  prefs.putUShort("rain_latch", settings.rainLatchMinutes);
  prefs.putFloat("sqm_offset", SQM_CAL_OFFSET);
  prefs.end();
}

String checked(bool value) {
  return value ? " checked" : "";
}

String selected(int actual, int option) {
  return actual == option ? " selected" : "";
}

String settingsPageHtml(const String &notice = "") {
  String html;
  html.reserve(12500);
  html += F("<!doctype html><html><head><meta name=viewport "
            "content='width=device-width,initial-scale=1'><title>AstroWeather "
            "Settings</title><style>"
            "body{font-family:system-ui;background:#07111d;color:#e9f2ff;"
            "margin:0;padding:18px}.wrap{max-width:850px;margin:auto}"
            "fieldset{background:#102238;border:1px solid #244462;border-radius:"
            "14px;margin:14px 0;padding:16px}legend{font-weight:700}"
            "label{display:block;margin:12px 0 5px}.inline{display:flex;"
            "gap:14px;flex-wrap:wrap}.inline label{display:inline}"
            "input,select{box-sizing:border-box;width:100%;padding:10px;"
            "border-radius:8px;border:1px solid #52708e;background:#07111d;"
            "color:#fff}input[type=checkbox]{width:auto}button,.button{display:"
            "inline-block;background:#2f80ed;color:#fff;border:0;border-radius:"
            "9px;padding:11px 16px;text-decoration:none;margin:8px 8px 0 0}"
            ".secondary{background:#36516d}.warning{color:#ffd27a}.ok{color:"
            "#63e69b}.note{color:#a9bdd2;font-size:.9rem;line-height:1.45}"
            ".grid{display:grid;grid-template-columns:repeat(auto-fit,minmax("
            "220px,1fr));gap:12px}</style></head><body><div class=wrap>"
            "<h1>AstroWeather Settings</h1><p><a class='button secondary' "
            "href='/'>Back to dashboard</a></p>");
  if (notice.length()) {
    html += "<p class=ok>" + htmlEscape(notice) + "</p>";
  }

  html += F("<fieldset><legend>Current status</legend><div class=grid><div>");
  html += "Mode: " + String(WiFi.status() == WL_CONNECTED
      ? "Connected to Wi-Fi" : "Setup access point");
  html += "</div><div>Address: ";
  html += WiFi.status() == WL_CONNECTED
      ? WiFi.localIP().toString() : WiFi.softAPIP().toString();
  html += "</div><div>Signal: ";
  html += WiFi.status() == WL_CONNECTED ? String(WiFi.RSSI()) + " dBm" : "n/a";
  html += "</div><div>Telegram: ";
  html += settings.telegramEnabled &&
          settings.telegramToken.length() &&
          settings.telegramChatId.length() ? "configured" : "disabled";
  html += F("</div></div></fieldset><form method=post action='/settings/save'>");
  html += "<input type=hidden name=csrf value='" +
          htmlEscape(settingsCsrfToken) + "'>";
  html += F("<fieldset><legend>Network</legend><label>Device hostname</label>"
            "<input name=hostname maxlength=32 value='");
  html += htmlEscape(settings.hostname);
  html += F("'><label>Wi-Fi SSID</label><input name=wifi_ssid maxlength=64 "
            "value='");
  html += htmlEscape(settings.wifiSsid);
  html += F("'><label>Wi-Fi password</label><input type=password "
            "name=wifi_password maxlength=64 placeholder='Leave blank to keep "
            "the saved password'><label>Setup access-point password</label>"
            "<input type=password name=ap_password maxlength=63 "
            "placeholder='Leave blank to keep the saved password'>"
            "<p class=note>If Wi-Fi fails, connect to AstroWeather-Setup and "
            "open http://192.168.4.1/settings. Saving restarts the station.</p>"
            "</fieldset><fieldset><legend>Telegram</legend><div class=inline>"
            "<label><input type=checkbox name=tg_enable");
  html += checked(settings.telegramEnabled);
  html += F("> Enable Telegram</label><label><input type=checkbox "
            "name=tg_start");
  html += checked(settings.notifyStartup);
  html += F("> Startup message</label><label><input type=checkbox "
            "name=tg_unsafe");
  html += checked(settings.notifyUnsafe);
  html += F("> Unsafe alerts</label><label><input type=checkbox name=tg_safe");
  html += checked(settings.notifyRecovery);
  html += F("> Recovery alerts</label><label><input type=checkbox name=tg_rain");
  html += checked(settings.notifyRain);
  html += F("> Rain alerts</label></div><label>Bot token</label>"
            "<input type=password name=tg_token maxlength=128 "
            "placeholder='Leave blank to keep the saved token'><label>"
            "<input type=checkbox name=tg_clear> Delete saved bot token</label>"
            "<label>Chat or group ID</label><input name=tg_chat maxlength=64 "
            "value='");
  html += htmlEscape(settings.telegramChatId);
  html += F("'><label>Regular report interval</label><select name=tg_mins>"
            "<option value=0");
  html += selected(settings.reportMinutes, 0);
  html += F(">Off</option><option value=30");
  html += selected(settings.reportMinutes, 30);
  html += F(">Every 30 minutes</option><option value=60");
  html += selected(settings.reportMinutes, 60);
  html += F(">Every 1 hour</option></select><label>UTC offset in minutes "
            "(India = 330)</label><input type=number name=utc_offset min=-720 "
            "max=840 value='");
  html += String(settings.utcOffsetMinutes);
  html += F("'><p class=note>The token is never displayed by this page. "
            "Telegram needs working internet and the bot must already have "
            "access to the chosen private chat or group.</p></fieldset>"
            "<fieldset><legend>Weather thresholds</legend><div class=grid>"
            "<div><label>Humidity unsafe at (%)</label><input type=number "
            "step=0.1 min=1 max=100 name=humid_limit value='");
  html += String(HUMIDITY_LIMIT, 1);
  html += F("'></div><div><label>Minimum dew margin (C)</label><input "
            "type=number step=0.1 min=0 max=20 name=dew_limit value='");
  html += String(DEW_MARGIN_LIMIT_C, 1);
  html += F("'></div><div><label>Cloud unsafe at (%)</label><input "
            "type=number step=0.1 min=0 max=100 name=cloud_limit value='");
  html += String(CLOUD_LIMIT_PERCENT, 1);
  html += F("'></div><div><label>Rain raw threshold</label><input type=number "
            "min=0 max=4095 name=rain_raw value='");
  html += String(RAIN_THRESHOLD_RAW);
  html += F("'></div><div><label>Rain unsafe latch (minutes)</label><input "
            "type=number min=1 max=120 name=rain_latch value='");
  html += String(settings.rainLatchMinutes);
  html += F("'></div><div><label>Clear thermal delta (C)</label><input "
            "type=number step=0.1 min=-20 max=80 name=clear_delta value='");
  html += String(CLEAR_DELTA_C, 1);
  html += F("'></div><div><label>Overcast thermal delta (C)</label><input "
            "type=number step=0.1 min=-20 max=80 name=overcast_delta value='");
  html += String(OVERCAST_DELTA_C, 1);
  html += F("'></div><div><label>SQM calibration offset</label><input "
            "type=number step=0.0001 min=-100 max=100 name=sqm_offset ");
  if (isfinite(SQM_CAL_OFFSET))
    html += "value='" + String(SQM_CAL_OFFSET, 4) + "'";
  html += F(" placeholder='Blank keeps current value'></div></div>"
            "<p class=warning>Changing safety thresholds requires supervised "
            "testing. This remains an advisory device.</p></fieldset>"
            "<fieldset><legend>Settings security</legend><label>New settings "
            "password</label><input type=password name=admin_password "
            "minlength=8 maxlength=63 placeholder='Leave blank to keep it'>"
            "<p class=note>Username: admin. Use only on a trusted private LAN; "
            "do not expose this HTTP page through port forwarding.</p>"
            "</fieldset><button type=submit>Save and restart</button></form>"
            "<form method=post action='/settings/test-telegram'>");
  html += "<input type=hidden name=csrf value='" +
          htmlEscape(settingsCsrfToken) + "'>";
  html += F("<button class=secondary type=submit>Send Telegram test</button>"
            "</form></div></body></html>");
  return html;
}

void handleSettingsPage() {
  if (!settingsAuthenticated()) return;
  server.sendHeader("Cache-Control", "no-store");
  server.send(200, "text/html", settingsPageHtml());
}

bool requestFloatInRange(const String &name, float &target,
                         float minimum, float maximum) {
  if (!server.hasArg(name)) return false;
  float value = server.arg(name).toFloat();
  if (!isfinite(value) || value < minimum || value > maximum) return false;
  target = value;
  return true;
}

void handleSettingsSave() {
  if (!settingsAuthenticated()) return;
  if (!settingsCsrfValid()) {
    server.send(403, "text/plain", "Invalid or expired settings form.");
    return;
  }

  String hostname = server.arg("hostname");
  hostname.trim();
  if (!validHostname(hostname)) {
    server.send(400, "text/plain",
                "Hostname must be 1-32 letters, numbers or hyphens.");
    return;
  }
  String newAdminPassword = server.arg("admin_password");
  String newApPassword = server.arg("ap_password");
  if ((newAdminPassword.length() > 0 && newAdminPassword.length() < 8) ||
      (newApPassword.length() > 0 && newApPassword.length() < 8)) {
    server.send(400, "text/plain", "Passwords must contain at least 8 characters.");
    return;
  }

  float clearDelta = CLEAR_DELTA_C;
  float overcastDelta = OVERCAST_DELTA_C;
  float humidityLimit = HUMIDITY_LIMIT;
  float dewLimit = DEW_MARGIN_LIMIT_C;
  float cloudLimit = CLOUD_LIMIT_PERCENT;
  if (!requestFloatInRange("clear_delta", clearDelta, -20, 80) ||
      !requestFloatInRange("overcast_delta", overcastDelta, -20, 80) ||
      !requestFloatInRange("humid_limit", humidityLimit, 1, 100) ||
      !requestFloatInRange("dew_limit", dewLimit, 0, 20) ||
      !requestFloatInRange("cloud_limit", cloudLimit, 0, 100) ||
      clearDelta <= overcastDelta) {
    server.send(400, "text/plain",
                "Invalid thresholds. Clear delta must exceed overcast delta.");
    return;
  }

  int reportMinutes = server.arg("tg_mins").toInt();
  int utcOffset = server.arg("utc_offset").toInt();
  int rainRawValue = server.arg("rain_raw").toInt();
  int rainLatchValue = server.arg("rain_latch").toInt();
  if ((reportMinutes != 0 && reportMinutes != 30 && reportMinutes != 60) ||
      utcOffset < -720 || utcOffset > 840 ||
      rainRawValue < 0 || rainRawValue > 4095 ||
      rainLatchValue < 1 || rainLatchValue > 120) {
    server.send(400, "text/plain", "Invalid interval, timezone or rain setting.");
    return;
  }

  settings.hostname = hostname;
  settings.wifiSsid = server.arg("wifi_ssid");
  if (server.arg("wifi_password").length())
    settings.wifiPassword = server.arg("wifi_password");
  if (newApPassword.length()) settings.setupApPassword = newApPassword;
  if (newAdminPassword.length()) settings.adminPassword = newAdminPassword;

  settings.telegramEnabled = server.hasArg("tg_enable");
  settings.notifyStartup = server.hasArg("tg_start");
  settings.notifyUnsafe = server.hasArg("tg_unsafe");
  settings.notifyRecovery = server.hasArg("tg_safe");
  settings.notifyRain = server.hasArg("tg_rain");
  if (server.hasArg("tg_clear")) settings.telegramToken = "";
  else if (server.arg("tg_token").length())
    settings.telegramToken = server.arg("tg_token");
  settings.telegramChatId = server.arg("tg_chat");
  settings.reportMinutes = reportMinutes;
  settings.utcOffsetMinutes = utcOffset;
  settings.rainLatchMinutes = rainLatchValue;

  CLEAR_DELTA_C = clearDelta;
  OVERCAST_DELTA_C = overcastDelta;
  HUMIDITY_LIMIT = humidityLimit;
  DEW_MARGIN_LIMIT_C = dewLimit;
  CLOUD_LIMIT_PERCENT = cloudLimit;
  RAIN_THRESHOLD_RAW = rainRawValue;
  if (server.arg("sqm_offset").length()) {
    float offset = server.arg("sqm_offset").toFloat();
    if (!isfinite(offset) || offset < -100 || offset > 100) {
      server.send(400, "text/plain", "Invalid SQM calibration offset.");
      return;
    }
    SQM_CAL_OFFSET = offset;
  }

  saveSettingsToNvs();
  server.send(200, "text/html",
              "<!doctype html><meta name=viewport content='width=device-width'>"
              "<body style='font-family:system-ui'><h1>Settings saved</h1>"
              "<p>AstroWeather is restarting. Reconnect using the new network "
              "settings, then open the dashboard.</p></body>");
  settingsRestartAt = millis() + 1500UL;
}

void handleTelegramTest() {
  if (!settingsAuthenticated()) return;
  if (!settingsCsrfValid()) {
    server.send(403, "text/plain", "Invalid or expired settings form.");
    return;
  }
  bool sent = telegramSendTest();
  server.send(200, "text/html",
              settingsPageHtml(sent ? "Telegram test message sent."
                                    : "Telegram test failed. Check Wi-Fi, token and chat ID."));
}

void registerSettingsRoutes() {
  server.on("/settings", HTTP_GET, handleSettingsPage);
  server.on("/settings/save", HTTP_POST, handleSettingsSave);
  server.on("/settings/test-telegram", HTTP_POST, handleTelegramTest);
}

void settingsLoop() {
  if (settingsRestartAt &&
      (long)(millis() - settingsRestartAt) >= 0) ESP.restart();
}

void setupWiFi() {
  WiFi.setHostname(settings.hostname.c_str());
  WiFi.setAutoReconnect(true);
  WiFi.persistent(false);

  if (settings.wifiSsid.length()) {
    WiFi.mode(WIFI_STA);
    WiFi.begin(settings.wifiSsid.c_str(), settings.wifiPassword.c_str());
    unsigned long start = millis();
    while (WiFi.status() != WL_CONNECTED &&
           millis() - start < 20000UL) delay(250);
  }

  if (WiFi.status() != WL_CONNECTED) {
    WiFi.mode(WIFI_AP_STA);
    WiFi.softAP("AstroWeather-Setup", settings.setupApPassword.c_str());
  }
}
