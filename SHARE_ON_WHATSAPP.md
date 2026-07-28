# WhatsApp sharing text

## Short group message

🌦️ **AstroWeather India — DIY weather station for astrophotography**

I have designed and open-sourced an affordable ESP32 weather station for
attended astrophotography. It measures temperature, humidity, dew point,
dew margin, infrared cloud cover, relative sky brightness and rain, and provides
a live Wi-Fi dashboard with an imaging OK/UNSAFE advisory.

✅ Almost completely 3D printable
✅ Support-free parts for Creality K1C and K2 printers
✅ ESP32 web dashboard + JSON API
✅ Locally calibrated cloud and rain thresholds
✅ All STL, OpenSCAD, firmware, wiring and assembly files included
✅ AGPL-3.0 open-source licence

💰 Approximate complete cost: **₹2,900 plus shipping**
Electronics and assembly supplies: ₹2,617.70
PETG and mounting straps: approximately ₹250

GitHub: https://github.com/pulikantitarun/astro-weather-india

Version 1.2 also works as an ASCOM Alpaca ObservingConditions + SafetyMonitor
device directly from the ESP32, and includes a native INDI Weather driver for
KStars/Ekos-compatible systems.

⚠️ This is an attended-imaging advisory device, not a certified unattended-roof
safety controller.

## Detailed group message

🔭🌦️ **AstroWeather India — affordable open-source astrophotography weather
station**

I wanted a practical weather monitor that Indian astrophotographers could build
without expensive imported observatory hardware, so I designed this modular
ESP32 station.

It provides:

• air temperature and humidity
• calculated dew point and dew margin
• infrared zenith temperature
• locally calibrated cloud percentage
• thermal clarity score
• relative zenith lux, full-spectrum and IR counts
• reference-calibrated estimated SQM and estimated Bortle class
• corrosion-reduced rain detection with a 10-minute unsafe latch
• live phone/PC dashboard over Wi-Fi
• JSON API for future Home Assistant, Node-RED or observatory integration
• automatic imaging OK/UNSAFE advisory

The printable package has physically labelled A-L parts, a radiation shield,
twin-sensor sky head, sloped rain mount, electronics box, fit-test pieces and
support-free K1C/K2 print orientations.

**Approximate July 2026 India cost**

• ESP32, MLX90614, rain sensor, wiring, solder, flux and power: ₹1,550
• SHT31: ₹312.70
• TSL2591: ₹755
• PETG and straps: about ₹250
• Complete estimate: **₹2,867.70 before shipping**

All STL files, editable OpenSCAD, ESP32 firmware, BOM, wiring, calibration
guide, validation report and assembly visuals are available here:

https://github.com/pulikantitarun/astro-weather-india

Version 1.2 adds direct ASCOM Alpaca discovery, ObservingConditions and
SafetyMonitor support, plus a buildable native INDI Weather driver for
KStars/Ekos, Astroberry and StellarMate-style installations.

Licensed under **AGPL-3.0-or-later** so improvements remain open.

Important: this is intended for supervised imaging. It is not a certified
weather instrument and should not be the sole safety sensor for an unattended
observatory roof.
