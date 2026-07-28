# INDI / KStars / Ekos setup

The source for the native driver is in
`integrations/indi-astroweather`. It polls the ESP32 JSON endpoint and exposes
INDI Weather values to KStars/Ekos and other standard INDI clients.

## Install

Run these commands on the Linux computer that hosts INDI:

```bash
sudo apt update
sudo apt install build-essential cmake libindi-dev libcurl4-openssl-dev
cd integrations/indi-astroweather
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
sudo cmake --install build
```

Restart the INDI service. Add **AstroWeather India** as a Weather device, open
its Options tab and set:

```text
http://astroweather.local/api/weather
```

If mDNS is not available, reserve an IP address for the ESP32 in the router
and use `http://192.168.x.x/api/weather`.

## Values

The driver supplies:

- temperature, humidity and dew point;
- cloud cover and sky temperature;
- calibrated estimated sky quality in mag/arcsec²;
- estimated Bortle class;
- dry/wet rain state;
- `WEATHER_FORECAST = 0` for advisory safe or `3` for unsafe.

The rain state and forecast are critical safety parameters. SQM/Bortle are
only updated when calibrated finite values are present in the ESP32 response.

This works with INDI installations that permit third-party driver binaries,
including typical KStars/Ekos, Astroberry and StellarMate systems. Standard
ASIAIR firmware does not provide a supported route for installing arbitrary
third-party INDI drivers, so ASIAIR compatibility is not claimed.

As with the dashboard and Alpaca interface, the INDI status is advisory only
and must not be the sole unattended roof-closing input.
