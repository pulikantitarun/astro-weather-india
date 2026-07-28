# AstroWeather INDI driver

This driver reads the station's local `/api/weather` JSON endpoint and exposes
standard INDI Weather parameters plus rain and estimated Bortle details.

## Build and install

On a Debian/Ubuntu-based INDI computer:

```bash
sudo apt update
sudo apt install build-essential cmake libindi-dev libcurl4-openssl-dev nlohmann-json3-dev
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
sudo cmake --install build
```

Restart the INDI server, select **AstroWeather India** under Weather, then set
the endpoint to `http://astroweather.local/api/weather`.

The driver is intended for normal INDI installations including KStars/Ekos,
Astroberry and StellarMate. Closed appliances that do not permit third-party
INDI driver installation, including standard ASIAIR firmware, are not
guaranteed to support it.

`WEATHER_SKY_QUALITY` is only updated after `SQM_CAL_OFFSET` is calibrated in
the ESP32 firmware. The binary rain plate is exposed as a dry/wet safety
parameter; it is not misreported as a measured rain rate.
