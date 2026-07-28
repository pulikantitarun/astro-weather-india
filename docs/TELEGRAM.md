# Telegram weather reports

AstroWeather v1.3 can send outbound Telegram reports directly from the ESP32.
No additional computer, cloud subscription or electronic component is needed.

## Create the bot

1. Open the official `@BotFather` account in Telegram.
2. Send `/newbot` and follow its instructions.
3. Copy the generated bot token.
4. Open the new bot and send `/start`. A bot cannot initiate a private
   conversation until the user has contacted it.
5. For a group, add the bot to the group and send a message that it can receive.
6. Obtain the private chat or group `chat_id` from the bot's updates.

Treat the token like a password: anyone with it can control the bot. If it is
ever exposed, revoke it through BotFather.

## Configure AstroWeather

1. Open `http://astroweather.local/settings`.
2. Enable Telegram.
3. Paste the bot token and destination chat/group ID.
4. Choose reports every 30 minutes, every hour, or off.
5. Enable the desired event messages:
   - station online;
   - weather changed to unsafe;
   - conditions recovered to safe;
   - rain detected.
6. Keep the UTC offset at `330` for India Standard Time.
7. Save, allow the station to restart, reopen Settings and select
   **Send Telegram test**.

Example:

```text
REGULAR WEATHER REPORT
AstroWeather India - 2026-07-28 21:30

Temperature: 23.4 C
Humidity: 78.0 %
Dew point: 19.4 C
Dew margin: 4.0 C
Cloud cover: 16 %
Clarity: 84 / 100
SQM estimate: 20.65
Estimated Bortle: 4
Rain: dry
Status: ADVISORY SAFE
```

SQM and Bortle are labelled unavailable until a valid SQM calibration exists.
Rain triggers immediately and the safe recovery message is delayed until the
configured rain latch and all other unsafe conditions have cleared.

## Reliability and privacy

- Telegram uses certificate-validated HTTPS and NTP-corrected time.
- The token is never returned by the dashboard or JSON API.
- No inbound router port or Telegram command polling is required.
- If internet access fails, the dashboard, API, ASCOM and INDI remain local and
  operational.
- Event messages that occur entirely during an outage cannot be recovered
  because this inexpensive design does not include persistent message queuing.
- Telegram is an advisory notification channel, not a roof safety interlock.

Official references:

- [Telegram Bot API](https://core.telegram.org/bots/api)
- [Telegram BotFather and bot features](https://core.telegram.org/bots/features)
