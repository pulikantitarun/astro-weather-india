/*
  AstroWeather India v1.3
  ESP32 + SHT31 + MLX90614 + TSL2591

  SPDX-License-Identifier: AGPL-3.0-or-later
  Copyright (C) 2026 Tarun and contributors

  Advisory weather data only. Its low-cost rain plate is consumable and it
  must never be the sole input to an unattended roof-closing system.
*/

#include <WiFi.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <Wire.h>
#include <Adafruit_SHT31.h>
#include <Adafruit_MLX90614.h>
#include <Adafruit_TSL2591.h>
#include "secrets.h"

WebServer server(80);
Adafruit_SHT31 sht31 = Adafruit_SHT31();
Adafruit_MLX90614 mlx = Adafruit_MLX90614();
Adafruit_TSL2591 tsl = Adafruit_TSL2591(2591);

// Tune these after at least 3 clear and 3 overcast local nights.
float CLEAR_DELTA_C = 18.0;      // ambient - sky under locally clear conditions
float OVERCAST_DELTA_C = 7.0;    // ambient - sky under locally overcast conditions
float HUMIDITY_LIMIT = 90.0;
float DEW_MARGIN_LIMIT_C = 2.0;
float CLOUD_LIMIT_PERCENT = 55.0;
int RAIN_THRESHOLD_RAW = 700;    // Calibrate between dry and droplet readings.

// Leave as NAN until calibrated against a reference SQM. With the TSL2591 fixed
// at maximum gain and 600 ms integration:
// offset = reference_SQM + 2.5 * log10(reference_visible_counts)
float SQM_CAL_OFFSET = NAN;
const uint16_t TSL_SATURATION_COUNTS = 65000;

// Cheapest rain input: bare exposed-trace plate, no LM393 board.
// Each GPIO connects to one plate electrode through 100k; each GPIO also has
// 470k to GND. Alternating polarity and a very low duty cycle reduce corrosion.
const int RAIN_A_PIN = 32;
const int RAIN_B_PIN = 33;

bool shtOK = false, mlxOK = false, tslOK = false;
float airC = NAN, humidity = NAN, dewC = NAN, skyC = NAN;
float cloudDeltaC = NAN, cloudPercent = NAN, clarityScore = NAN;
float lux = NAN, sqmEstimate = NAN;
uint16_t tslFull = 0, tslIR = 0, tslVisible = 0;
int bortleEstimate = 0;
bool tslLightValid = false;
unsigned long lastRead = 0;
unsigned long lastRainRead = 0, rainLatchUntil = 0;
int rainRaw = 0;
bool rainDetected = false;

const char INDEX_HTML[] PROGMEM = R"rawliteral(
<!doctype html><html><head><meta name="viewport" content="width=device-width,initial-scale=1">
<title>AstroWeather</title><style>
body{font-family:system-ui;background:#07111d;color:#e9f2ff;margin:0;padding:18px}
.wrap{max-width:850px;margin:auto}.top{display:flex;justify-content:space-between;align-items:center}
.grid{display:grid;grid-template-columns:repeat(auto-fit,minmax(160px,1fr));gap:12px}
.card{background:#102238;border:1px solid #244462;border-radius:14px;padding:15px}
.v{font-size:1.7rem;font-weight:700;margin-top:5px}.ok{color:#63e69b}.bad{color:#ff8178}
.note{color:#9bb0c8;font-size:.9rem;line-height:1.4}h1{font-size:1.5rem}
a{color:#79b8ff}.settings{background:#18324e;border-radius:8px;padding:7px 10px;text-decoration:none}
</style></head><body><div class="wrap">
<div class="top"><h1>AstroWeather India</h1><div><span id="safe">Reading…</span> &nbsp; <a class=settings href="/settings">Settings</a></div></div>
<div class="grid" id="grid"></div>
<p class="note">Advisory only: “Clarity” is the calibrated IR cloud score. SQM and Bortle are estimates from the
TSL2591 and remain unavailable until reference calibration. Never use this alone to control an unattended roof.</p>
</div><script>
const f=(x,n=1)=>x===null?'—':Number(x).toFixed(n);
async function tick(){let d=await (await fetch('/api/weather')).json();
let sqm=d.sqm_estimate===null?(d.sqm_calibrated?'OUT OF RANGE':'CALIBRATE'):f(d.sqm_estimate,2)+' mag/arcsec²';
let bortle=d.bortle_estimate===null?'—':'Bortle '+d.bortle_estimate;
let a=[['Air',f(d.air_c)+' °C'],['Humidity',f(d.humidity)+' %'],
['Dew point',f(d.dew_c)+' °C'],['Dew margin',f(d.dew_margin_c)+' °C'],
['Sky IR',f(d.sky_c)+' °C'],['Cloud delta',f(d.cloud_delta_c)+' °C'],
['Cloud estimate',f(d.cloud_percent,0)+' %'],['Clarity',f(d.clarity_score,0)+' / 100'],
['SQM estimate',sqm],['Estimated Bortle',bortle],
['Zenith light',f(d.lux,6)+' lux'],['Rain plate',d.rain_detected?'WET':'dry'],
['Wi-Fi',d.wifi_connected?(d.wifi_rssi_dbm+' dBm'):'setup AP'],
['Telegram',d.telegram_enabled?(d.telegram_report_minutes?d.telegram_report_minutes+' min':'alerts only'):'off']];
document.querySelector('#grid').innerHTML=a.map(x=>`<div class=card>${x[0]}<div class=v>${x[1]}</div></div>`).join('');
let s=document.querySelector('#safe');s.textContent=d.advisory_safe?'ADVISORY: OK':'ADVISORY: UNSAFE';
s.className=d.advisory_safe?'ok':'bad'}tick();setInterval(tick,10000);
</script></body></html>)rawliteral";

float dewPoint(float t, float rh) {
  if (!isfinite(t) || !isfinite(rh) || rh <= 0) return NAN;
  const float a = 17.62, b = 243.12;
  float g = log(rh / 100.0) + (a * t) / (b + t);
  return (b * g) / (a - g);
}

float clampf(float v, float lo, float hi) {
  return v < lo ? lo : (v > hi ? hi : v);
}

#include "AstroWeather_Config.h"

int sqmToEstimatedBortle(float sqm) {
  // A convenient heuristic, not a measurement of John Bortle's visual scale.
  if (!isfinite(sqm)) return 0;
  if (sqm >= 21.99) return 1;
  if (sqm >= 21.89) return 2;
  if (sqm >= 21.69) return 3;
  if (sqm >= 20.49) return 4;
  if (sqm >= 19.50) return 5;
  if (sqm >= 18.94) return 6;
  if (sqm >= 18.38) return 7;
  if (sqm >= 17.80) return 8;
  return 9;
}

int rainOneWay(int drivePin, int sensePin) {
  pinMode(drivePin, OUTPUT);
  digitalWrite(drivePin, HIGH);
  pinMode(sensePin, INPUT);
  delay(4);
  int v = analogRead(sensePin);
  digitalWrite(drivePin, LOW);
  pinMode(drivePin, INPUT);
  pinMode(sensePin, INPUT);
  delay(2);
  return v;
}

void readRain() {
  // Swap polarity so deposited metal is not continuously driven one way.
  int ab = rainOneWay(RAIN_A_PIN, RAIN_B_PIN);
  int ba = rainOneWay(RAIN_B_PIN, RAIN_A_PIN);
  rainRaw = (ab + ba) / 2;
  bool wetNow = rainRaw >= RAIN_THRESHOLD_RAW;
  if (wetNow)
    rainLatchUntil = millis() +
      (unsigned long)settings.rainLatchMinutes * 60000UL;
  rainDetected = wetNow || ((long)(rainLatchUntil - millis()) > 0);
}

void readSensors() {
  if (shtOK) {
    float t = sht31.readTemperature();
    float h = sht31.readHumidity();
    if (isfinite(t)) airC = t;
    if (isfinite(h)) humidity = h;
  }
  if (mlxOK) {
    float s = mlx.readObjectTempC();
    if (isfinite(s)) skyC = s;
  }
  if (tslOK) {
    uint32_t lum = tsl.getFullLuminosity();
    tslIR = lum >> 16;
    tslFull = lum & 0xFFFF;
    tslVisible = tslFull > tslIR ? tslFull - tslIR : 0;
    tslLightValid = tslVisible > 0 &&
                    tslFull < TSL_SATURATION_COUNTS &&
                    tslIR < TSL_SATURATION_COUNTS;
    if (tslLightValid) {
      float measuredLux = tsl.calculateLux(tslFull, tslIR);
      lux = isfinite(measuredLux) && measuredLux >= 0 ? measuredLux : NAN;
      sqmEstimate = isfinite(SQM_CAL_OFFSET)
        ? SQM_CAL_OFFSET - 2.5 * log10((float)tslVisible)
        : NAN;
      bortleEstimate = sqmToEstimatedBortle(sqmEstimate);
    } else {
      lux = NAN;
      sqmEstimate = NAN;
      bortleEstimate = 0;
    }
  }

  dewC = dewPoint(airC, humidity);
  cloudDeltaC = airC - skyC;
  if (isfinite(cloudDeltaC) && CLEAR_DELTA_C > OVERCAST_DELTA_C) {
    cloudPercent = 100.0 * (CLEAR_DELTA_C - cloudDeltaC) /
                   (CLEAR_DELTA_C - OVERCAST_DELTA_C);
    cloudPercent = clampf(cloudPercent, 0, 100);
    clarityScore = 100.0 - cloudPercent;
  }
}

bool advisorySafe() {
  if (!shtOK || !mlxOK || !isfinite(humidity) || !isfinite(dewC) ||
      !isfinite(cloudPercent)) return false;
  return humidity < HUMIDITY_LIMIT &&
         (airC - dewC) > DEW_MARGIN_LIMIT_C &&
         cloudPercent < CLOUD_LIMIT_PERCENT &&
         !rainDetected;
}

String numOrNull(float v, int digits=2) {
  return isfinite(v) ? String(v, digits) : "null";
}

#include "AstroWeather_Telegram.h"

void apiWeather() {
  String j = "{";
  j += "\"air_c\":" + numOrNull(airC) + ",";
  j += "\"humidity\":" + numOrNull(humidity) + ",";
  j += "\"dew_c\":" + numOrNull(dewC) + ",";
  j += "\"dew_margin_c\":" + numOrNull(airC-dewC) + ",";
  j += "\"sky_c\":" + numOrNull(skyC) + ",";
  j += "\"cloud_delta_c\":" + numOrNull(cloudDeltaC) + ",";
  j += "\"cloud_percent\":" + numOrNull(cloudPercent) + ",";
  j += "\"clarity_score\":" + numOrNull(clarityScore) + ",";
  j += "\"lux\":" + numOrNull(lux,6) + ",";
  j += "\"tsl_full\":" + String(tslFull) + ",";
  j += "\"tsl_ir\":" + String(tslIR) + ",";
  j += "\"tsl_visible\":" + String(tslVisible) + ",";
  j += "\"tsl_light_valid\":" + String(tslLightValid ? "true":"false") + ",";
  j += "\"sqm_estimate\":" + numOrNull(sqmEstimate) + ",";
  j += "\"sqm_calibrated\":" + String(isfinite(SQM_CAL_OFFSET) ? "true":"false") + ",";
  j += "\"bortle_estimate\":";
  j += bortleEstimate > 0 ? String(bortleEstimate) : "null";
  j += ",";
  j += "\"rain_raw\":" + String(rainRaw) + ",";
  j += "\"sht_ok\":" + String(shtOK ? "true":"false") + ",";
  j += "\"mlx_ok\":" + String(mlxOK ? "true":"false") + ",";
  j += "\"tsl_ok\":" + String(tslOK ? "true":"false") + ",";
  j += "\"advisory_safe\":" + String(advisorySafe() ? "true":"false") + ",";
  j += "\"rain_detected\":" + String(rainDetected ? "true":"false") + ",";
  j += "\"wifi_connected\":" +
       String(WiFi.status() == WL_CONNECTED ? "true":"false") + ",";
  j += "\"wifi_rssi_dbm\":";
  j += WiFi.status() == WL_CONNECTED ? String(WiFi.RSSI()) : "null";
  j += ",";
  j += "\"telegram_enabled\":" +
       String(settings.telegramEnabled ? "true":"false") + ",";
  j += "\"telegram_report_minutes\":" + String(settings.reportMinutes) + ",";
  j += "\"firmware_version\":\"1.3.0\",";
  j += "\"uptime_s\":" + String(millis()/1000);
  j += "}";
  server.send(200, "application/json", j);
}

#include "AstroWeather_Alpaca.h"

void setup() {
  Serial.begin(115200);
  loadSettings();
  Wire.begin(21, 22);
  analogReadResolution(12);
  pinMode(RAIN_A_PIN, INPUT);
  pinMode(RAIN_B_PIN, INPUT);
  shtOK = sht31.begin(0x44);
  mlxOK = mlx.begin();
  tslOK = tsl.begin();
  if (tslOK) {
    tsl.setGain(TSL2591_GAIN_MAX);
    tsl.setTiming(TSL2591_INTEGRATIONTIME_600MS);
  }
  setupWiFi();
  MDNS.begin(settings.hostname.c_str());
  server.on("/", [](){ server.send_P(200, "text/html", INDEX_HTML); });
  server.on("/api/weather", apiWeather);
  server.on("/health", [](){ server.send(200, "text/plain", "ok"); });
  registerSettingsRoutes();
  alpacaBegin();
  server.begin();
  readSensors();
  readRain();
  lastRead = millis();
  telegramBegin();
  Serial.print("Open http://");
  Serial.println(WiFi.status() == WL_CONNECTED
      ? WiFi.localIP() : WiFi.softAPIP());
}

void loop() {
  server.handleClient();
  alpacaLoop();
  settingsLoop();
  if (millis() - lastRead >= 10000) {
    lastRead = millis();
    readSensors();
  }
  if (millis() - lastRainRead >= 2000) {
    lastRainRead = millis();
    readRain();
  }
  telegramLoop();
  delay(2);
}
