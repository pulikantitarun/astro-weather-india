# Web configuration portal

Firmware v1.3 stores operating settings in the ESP32's non-volatile memory.
Wi-Fi, Telegram and weather limits can therefore be changed from a phone or
computer without editing source code or reflashing.

## First setup

1. Flash the firmware.
2. If it cannot join a configured Wi-Fi network, connect your phone or computer
   to:
   - network: `AstroWeather-Setup`
   - default password: `astroclear`
3. Open `http://192.168.4.1/settings`.
4. Sign in with:
   - username: `admin`
   - default password: `astroadmin`
5. Immediately set a new settings password and setup-access-point password.
6. Enter the normal Wi-Fi SSID and password.
7. Configure the required weather and Telegram options.
8. Select **Save and restart**.

After a successful connection, use:

```text
http://astroweather.local/
http://astroweather.local/settings
```

If the hostname was changed, replace `astroweather` with the new hostname.

## Configurable values

### Network

- local hostname;
- Wi-Fi SSID and password;
- fallback setup-access-point password;
- settings-page password.

### Telegram

- enable/disable;
- bot token and chat/group ID;
- reports off, every 30 minutes or every hour;
- startup, unsafe, recovery and rain alerts;
- local UTC offset, defaulting to India Standard Time (`330` minutes).

### Weather and calibration

- humidity unsafe limit;
- minimum dew margin;
- cloud unsafe percentage;
- rain raw threshold;
- rain unsafe latch from 1 to 120 minutes;
- clear and overcast thermal calibration deltas;
- SQM calibration offset.

Passwords and the Telegram token are write-only: the page never displays their
stored values. A blank secret field keeps the current value. The Telegram form
also has an explicit option to delete the saved token.

## Persistence and security

Settings survive normal restarts and firmware updates because they are stored
in ESP32 NVS. Erasing the ESP32 flash also erases them.

The configuration page uses HTTP Basic authentication, but local HTTP is not
encrypted. Keep AstroWeather on a trusted private network, change both default
passwords and never expose port 80 through router port forwarding. Wi-Fi
passwords and Telegram tokens must be available to the firmware and are not
encrypted in ordinary ESP32 NVS; physical access to the device should therefore
be treated as access to those credentials.

The public dashboard, JSON API, ASCOM Alpaca and INDI service remain available
without settings credentials so astronomy clients continue to work.
