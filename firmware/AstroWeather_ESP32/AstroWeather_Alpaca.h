#pragma once

// SPDX-License-Identifier: AGPL-3.0-or-later
// Copyright (C) 2026 Tarun and contributors

#include <WiFiUdp.h>

// Minimal, standards-shaped ASCOM Alpaca server for the station's two logical
// devices. The existing WebServer remains on port 80; UDP discovery uses the
// standard Alpaca discovery port.
WiFiUDP alpacaDiscovery;
bool alpacaConnected = true;
uint32_t alpacaServerTransactionID = 0;

const uint16_t ALPACA_DISCOVERY_PORT = 32227;
const int ALPACA_NOT_IMPLEMENTED = 1024;
const int ALPACA_INVALID_VALUE = 1025;
const int ALPACA_VALUE_NOT_SET = 1026;
const int ALPACA_NOT_CONNECTED = 1031;
const int ALPACA_ACTION_NOT_IMPLEMENTED = 1036;

String alpacaArgCI(const String &wanted) {
  for (uint8_t i = 0; i < server.args(); ++i) {
    if (server.argName(i).equalsIgnoreCase(wanted)) return server.arg(i);
  }
  return "";
}

String alpacaJsonString(const String &value) {
  String out = "\"";
  for (size_t i = 0; i < value.length(); ++i) {
    char c = value[i];
    if (c == '"' || c == '\\') out += '\\';
    if (c == '\r') out += "\\r";
    else if (c == '\n') out += "\\n";
    else out += c;
  }
  out += "\"";
  return out;
}

uint32_t alpacaClientTransactionID() {
  String value = alpacaArgCI("ClientTransactionID");
  return value.length() ? (uint32_t)value.toInt() : 0;
}

void alpacaSendRawValue(const String &value, int errorNumber = 0,
                        const String &errorMessage = "") {
  String body = "{\"Value\":" + value;
  body += ",\"ClientTransactionID\":" + String(alpacaClientTransactionID());
  body += ",\"ServerTransactionID\":" + String(++alpacaServerTransactionID);
  body += ",\"ErrorNumber\":" + String(errorNumber);
  body += ",\"ErrorMessage\":" + alpacaJsonString(errorMessage) + "}";
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, "application/json", body);
}

void alpacaSendString(const String &value) {
  alpacaSendRawValue(alpacaJsonString(value));
}

void alpacaSendError(int number, const String &message) {
  alpacaSendRawValue("null", number, message);
}

String alpacaMacSuffix() {
  uint64_t mac = ESP.getEfuseMac();
  char text[13];
  snprintf(text, sizeof(text), "%04X%08X",
           (uint16_t)(mac >> 32), (uint32_t)mac);
  return String(text);
}

bool alpacaRequireConnected() {
  if (alpacaConnected) return true;
  alpacaSendError(ALPACA_NOT_CONNECTED, "Device is not connected");
  return false;
}

bool alpacaValue(float value, int digits = 3) {
  if (!alpacaRequireConnected()) return false;
  if (!isfinite(value)) {
    alpacaSendError(ALPACA_VALUE_NOT_SET, "No valid sensor value is available");
    return false;
  }
  alpacaSendRawValue(String(value, digits));
  return true;
}

void alpacaHandleManagement(const String &path) {
  if (path == "/management/apiversions") {
    alpacaSendRawValue("[1]");
    return;
  }
  if (path == "/management/v1/description") {
    String value = "{\"ServerName\":\"AstroWeather India\",";
    value += "\"Manufacturer\":\"AstroWeather India community project\",";
    value += "\"ManufacturerVersion\":\"1.2.0\",";
    value += "\"Location\":\"Local ESP32 weather station\"}";
    alpacaSendRawValue(value);
    return;
  }
  if (path == "/management/v1/configureddevices") {
    String mac = alpacaMacSuffix();
    String value = "[{\"DeviceName\":\"AstroWeather Observing Conditions\",";
    value += "\"DeviceType\":\"ObservingConditions\",\"DeviceNumber\":0,";
    value += "\"UniqueID\":\"ASTROWEATHER-OC-" + mac + "\"},";
    value += "{\"DeviceName\":\"AstroWeather Safety Monitor\",";
    value += "\"DeviceType\":\"SafetyMonitor\",\"DeviceNumber\":0,";
    value += "\"UniqueID\":\"ASTROWEATHER-SAFE-" + mac + "\"}]";
    alpacaSendRawValue(value);
    return;
  }
  server.send(404, "text/plain", "Unknown Alpaca management endpoint");
}

bool alpacaHandleCommon(const String &deviceType, const String &member) {
  const bool isSafety = deviceType == "safetymonitor";
  if (member == "connected") {
    if (server.method() == HTTP_PUT) {
      String requested = alpacaArgCI("Connected");
      if (!requested.length()) {
        alpacaSendError(ALPACA_INVALID_VALUE, "Connected is required");
        return true;
      }
      alpacaConnected = requested.equalsIgnoreCase("true") || requested == "1";
      alpacaSendRawValue("null");
    } else {
      alpacaSendRawValue(alpacaConnected ? "true" : "false");
    }
    return true;
  }
  if (member == "connect") {
    alpacaConnected = true;
    alpacaSendRawValue("null");
    return true;
  }
  if (member == "disconnect") {
    alpacaConnected = false;
    alpacaSendRawValue("null");
    return true;
  }
  if (member == "connecting") {
    alpacaSendRawValue("false");
    return true;
  }
  if (member == "name") {
    alpacaSendString(isSafety ? "AstroWeather Safety Monitor"
                              : "AstroWeather Observing Conditions");
    return true;
  }
  if (member == "description") {
    alpacaSendString(isSafety
      ? "Advisory safety state from rain, humidity, dew and cloud limits"
      : "Temperature, humidity, dew, clouds, sky temperature and brightness");
    return true;
  }
  if (member == "driverinfo") {
    alpacaSendString("AstroWeather India ESP32 Alpaca server; AGPL-3.0-or-later");
    return true;
  }
  if (member == "driverversion") {
    alpacaSendString("1.2");
    return true;
  }
  if (member == "interfaceversion") {
    // Interface V1 maximises compatibility. Platform 7 connect/disconnect
    // endpoints are also accepted above as a convenience.
    alpacaSendRawValue("1");
    return true;
  }
  if (member == "supportedactions") {
    alpacaSendRawValue("[]");
    return true;
  }
  if (member == "action" || member == "commandblind" ||
      member == "commandbool" || member == "commandstring") {
    alpacaSendError(ALPACA_ACTION_NOT_IMPLEMENTED,
                    "No custom command actions are implemented");
    return true;
  }
  return false;
}

bool alpacaImplementedSensorName(const String &name) {
  return name == "" || name == "temperature" || name == "humidity" ||
         name == "dewpoint" || name == "cloudcover" ||
         name == "skytemperature" || name == "skybrightness" ||
         name == "skyquality";
}

void alpacaHandleObserving(const String &member) {
  if (alpacaHandleCommon("observingconditions", member)) return;
  if (member == "averageperiod") {
    if (server.method() == HTTP_PUT) {
      float requested = alpacaArgCI("AveragePeriod").toFloat();
      if (fabs(requested) > 0.00001f) {
        alpacaSendError(ALPACA_INVALID_VALUE,
                        "Only instantaneous readings (0.0 hours) are supported");
      } else {
        alpacaSendRawValue("null");
      }
    } else {
      alpacaSendRawValue("0.0");
    }
    return;
  }
  if (member == "temperature") { alpacaValue(airC); return; }
  if (member == "humidity") { alpacaValue(humidity); return; }
  if (member == "dewpoint") { alpacaValue(dewC); return; }
  if (member == "cloudcover") { alpacaValue(cloudPercent); return; }
  if (member == "skytemperature") { alpacaValue(skyC); return; }
  if (member == "skybrightness") { alpacaValue(lux, 6); return; }
  if (member == "skyquality") {
    if (!isfinite(SQM_CAL_OFFSET)) {
      alpacaSendError(ALPACA_VALUE_NOT_SET,
                      "Calibrate SQM_CAL_OFFSET before using SkyQuality");
    } else {
      alpacaValue(sqmEstimate);
    }
    return;
  }
  if (member == "refresh") {
    if (!alpacaRequireConnected()) return;
    readSensors();
    readRain();
    lastRead = millis();
    alpacaSendRawValue("null");
    return;
  }
  if (member == "timesincelastupdate") {
    String sensor = alpacaArgCI("SensorName");
    sensor.toLowerCase();
    if (!alpacaImplementedSensorName(sensor)) {
      alpacaSendError(ALPACA_NOT_IMPLEMENTED,
                      "Requested sensor is not implemented");
      return;
    }
    alpacaSendRawValue(String((millis() - lastRead) / 1000.0f, 3));
    return;
  }
  if (member == "sensordescription") {
    String sensor = alpacaArgCI("SensorName");
    sensor.toLowerCase();
    if (!alpacaImplementedSensorName(sensor) || sensor == "") {
      alpacaSendError(ALPACA_INVALID_VALUE, "Unknown or empty SensorName");
      return;
    }
    if (sensor == "temperature" || sensor == "humidity" || sensor == "dewpoint")
      alpacaSendString("SHT31 ambient sensor and calculated dew point");
    else if (sensor == "cloudcover" || sensor == "skytemperature")
      alpacaSendString("MLX90614 zenith IR sensor with local cloud calibration");
    else
      alpacaSendString("TSL2591 zenith light sensor; SkyQuality requires calibration");
    return;
  }
  if (member == "pressure" || member == "rainrate" ||
      member == "starfwhm" || member == "winddirection" ||
      member == "windgust" || member == "windspeed") {
    alpacaSendError(ALPACA_NOT_IMPLEMENTED,
                    "This measurement is not implemented by the installed sensors");
    return;
  }
  alpacaSendError(ALPACA_NOT_IMPLEMENTED, "Unknown ObservingConditions member");
}

void alpacaHandleSafety(const String &member) {
  if (alpacaHandleCommon("safetymonitor", member)) return;
  if (member == "issafe") {
    if (!alpacaRequireConnected()) return;
    alpacaSendRawValue(advisorySafe() ? "true" : "false");
    return;
  }
  alpacaSendError(ALPACA_NOT_IMPLEMENTED, "Unknown SafetyMonitor member");
}

void alpacaHandleHttp() {
  String path = server.uri();
  path.toLowerCase();
  if (path.startsWith("/management/")) {
    alpacaHandleManagement(path);
    return;
  }

  const String prefix = "/api/v1/";
  if (!path.startsWith(prefix)) {
    server.send(404, "text/plain", "Not found");
    return;
  }
  String rest = path.substring(prefix.length());
  int firstSlash = rest.indexOf('/');
  int secondSlash = rest.indexOf('/', firstSlash + 1);
  if (firstSlash < 0 || secondSlash < 0) {
    server.send(404, "text/plain", "Invalid Alpaca endpoint");
    return;
  }
  String deviceType = rest.substring(0, firstSlash);
  int deviceNumber = rest.substring(firstSlash + 1, secondSlash).toInt();
  String member = rest.substring(secondSlash + 1);
  if (deviceNumber != 0) {
    alpacaSendError(ALPACA_INVALID_VALUE, "Only device number 0 exists");
  } else if (deviceType == "observingconditions") {
    alpacaHandleObserving(member);
  } else if (deviceType == "safetymonitor") {
    alpacaHandleSafety(member);
  } else {
    alpacaSendError(ALPACA_INVALID_VALUE, "Unknown device type");
  }
}

void alpacaBegin() {
  alpacaDiscovery.begin(ALPACA_DISCOVERY_PORT);
  server.onNotFound(alpacaHandleHttp);
}

void alpacaLoop() {
  int packetSize = alpacaDiscovery.parsePacket();
  if (!packetSize) return;
  char packet[32] = {0};
  int count = alpacaDiscovery.read(packet, sizeof(packet) - 1);
  if (count <= 0) return;
  String request(packet);
  request.trim();
  request.toLowerCase();
  if (request == "alpacadiscovery1") {
    const char response[] = "{\"AlpacaPort\":80}";
    alpacaDiscovery.beginPacket(alpacaDiscovery.remoteIP(),
                                alpacaDiscovery.remotePort());
    alpacaDiscovery.write((const uint8_t *)response, strlen(response));
    alpacaDiscovery.endPacket();
  }
}
