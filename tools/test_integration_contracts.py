#!/usr/bin/env python3
"""Fast repository checks for the ASCOM/INDI integration contract."""

from pathlib import Path
import sys
import xml.etree.ElementTree as ET

ROOT = Path(__file__).resolve().parents[1]
alpaca = (ROOT / "firmware/AstroWeather_ESP32/AstroWeather_Alpaca.h").read_text(
    encoding="utf-8"
)
indi = (ROOT / "integrations/indi-astroweather/astroweather.cpp").read_text(
    encoding="utf-8"
)

required_alpaca = [
    "alpacadiscovery1",
    "/management/apiversions",
    "/management/v1/configureddevices",
    "observingconditions",
    "safetymonitor",
    "skyquality",
    "ALPACA_VALUE_NOT_SET",
    "ALPACA_NOT_IMPLEMENTED",
]
required_indi = [
    "WEATHER_TEMPERATURE",
    "WEATHER_HUMIDITY",
    "WEATHER_DEW_POINT",
    "WEATHER_CLOUD_COVER",
    "WEATHER_SKY_QUALITY",
    "WEATHER_SKY_TEMPERATURE",
    "WEATHER_RAIN_DETECTED",
    "WEATHER_BORTLE_ESTIMATE",
    "WEATHER_FORECAST",
]

missing = [item for item in required_alpaca if item not in alpaca]
missing += [item for item in required_indi if item not in indi]
if missing:
    print("Missing integration contract markers:", ", ".join(missing))
    sys.exit(1)

xml_path = ROOT / "integrations/indi-astroweather/indi_astroweather.xml"
root = ET.parse(xml_path).getroot()
driver = root.find(".//driver")
if driver is None or driver.text != "indi_astroweather":
    print("Invalid INDI driver registry XML")
    sys.exit(1)

print("ASCOM Alpaca and INDI integration contract checks passed.")
