#!/usr/bin/env python3
"""Validate security and feature markers for the web configuration portal."""

from pathlib import Path
import re
import sys

ROOT = Path(__file__).resolve().parents[1]
config = (ROOT / "firmware/AstroWeather_ESP32/AstroWeather_Config.h").read_text(
    encoding="utf-8"
)
telegram = (
    ROOT / "firmware/AstroWeather_ESP32/AstroWeather_Telegram.h"
).read_text(encoding="utf-8")
firmware = (
    ROOT / "firmware/AstroWeather_ESP32/AstroWeather_ESP32.ino"
).read_text(encoding="utf-8")

required = [
    "/settings",
    "/settings/save",
    "/settings/test-telegram",
    "server.authenticate",
    "settingsCsrfValid",
    "Preferences",
    "wifi_ssid",
    "tg_token",
    "rain_latch",
    "telegram_report_minutes",
    "firmware_version",
]
missing = [marker for marker in required if marker not in config + firmware]

telegram_required = [
    "TELEGRAM_CERTIFICATE_ROOT",
    "sendMessage",
    "configTime",
    "RAIN DETECTED",
    "WEATHER UNSAFE",
    "CONDITIONS RECOVERED",
    "REGULAR WEATHER REPORT",
]
missing += [marker for marker in telegram_required if marker not in telegram]

if "setInsecure" in telegram:
    print("Telegram TLS must not disable certificate validation.")
    sys.exit(1)

# ESP32 Preferences/NVS keys are limited to 15 characters.
keys = re.findall(r'prefs\.(?:get|put)\w+\("([^"]+)"', config)
if len(set(keys)) < 15:
    print("NVS key validation did not inspect the expected settings.")
    sys.exit(1)
too_long = sorted({key for key in keys if len(key) > 15})
if too_long:
    print("NVS keys exceed 15 characters:", ", ".join(too_long))
    sys.exit(1)

# Secret values must not be rendered into the HTML form.
for unsafe in (
    "htmlEscape(settings.telegramToken)",
    "htmlEscape(settings.wifiPassword)",
    "htmlEscape(settings.adminPassword)",
):
    if unsafe in config:
        print("Configuration page exposes a saved secret:", unsafe)
        sys.exit(1)

if missing:
    print("Missing configuration contract markers:", ", ".join(missing))
    sys.exit(1)

print("Web configuration and Telegram security contract checks passed.")
