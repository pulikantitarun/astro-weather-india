# Technical references

- [Adafruit TSL2591 gain, integration time, raw channels and lux calculation](https://learn.adafruit.com/adafruit-tsl2591/wiring-and-test)
- [Unihedron SQM-LR operator manual and mag/arcsec² response format](https://unihedron.com/projects/darksky/cd/SQM-LR/SQM-LR_Users_manual.pdf)
- [Introducing the Bortle Dark-Sky Scale, John E. Bortle](https://www.skyandtelescope.org/wp-content/uploads/BortleDarkSkyScale.pdf)
- [ASCOM Master ObservingConditions interface](https://ascom-standards.org/newdocs/observingconditions.html)
- [ASCOM Alpaca API reference](https://ascom-standards.org/AlpacaDeveloper/ASCOMAlpacaAPIReference.html)
- [INDI WeatherInterface documentation](https://docs.indilib.org/interfaces/weather-interface.html)
- [Telegram Bot API](https://core.telegram.org/bots/api)
- [Telegram BotFather and bot features](https://core.telegram.org/bots/features)

The TSL2591 does not natively report SQM. AstroWeather applies the astronomical
magnitude relationship to fixed-gain visible counts after calibration against a
reference SQM. The reported Bortle value is a heuristic label and is not a
replacement for the visual assessment described by Bortle.

The network integrations intentionally use the published ASCOM Alpaca and INDI
Weather interfaces. Unsupported physical measurements such as pressure, wind,
seeing and rain rate are not inferred.
