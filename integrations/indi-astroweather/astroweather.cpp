/*
 * AstroWeather India INDI weather driver
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */

#include "astroweather.h"

#ifdef _USE_SYSTEM_JSONLIB
#include <nlohmann/json.hpp>
#else
#include <indijson.hpp>
#endif

#include <curl/curl.h>

#include <cmath>
#include <cstring>
#include <memory>
#include <stdexcept>
#include <string>

using json = nlohmann::json;

std::unique_ptr<AstroWeather> astroWeather(new AstroWeather());

static size_t writeCallback(void *contents, size_t size, size_t count, void *target)
{
    static_cast<std::string *>(target)->append(
        static_cast<char *>(contents), size * count);
    return size * count;
}

static bool readFiniteNumber(const json &report, const char *key, double &value)
{
    if (!report.contains(key) || !report[key].is_number())
        return false;
    value = report[key].get<double>();
    return std::isfinite(value);
}

AstroWeather::AstroWeather()
{
    setVersion(1, 2);
    setWeatherConnection(CONNECTION_NONE);
    curl_global_init(CURL_GLOBAL_DEFAULT);
}

AstroWeather::~AstroWeather()
{
    curl_global_cleanup();
}

const char *AstroWeather::getDefaultName()
{
    return "AstroWeather India";
}

bool AstroWeather::initProperties()
{
    INDI::Weather::initProperties();

    char savedURL[512] = {0};
    IUGetConfigText(getDeviceName(), "STATION_URL", "URL",
                    savedURL, sizeof(savedURL));
    const char *defaultURL = savedURL[0]
        ? savedURL : "http://astroweather.local/api/weather";
    stationURLTP[0].fill("URL", "JSON endpoint", defaultURL);
    stationURLTP.fill(getDeviceName(), "STATION_URL", "AstroWeather station",
                      OPTIONS_TAB, IP_RW, 60, IPS_IDLE);

    // Safe ranges mirror the firmware's default advisory thresholds. Users can
    // adjust them in the INDI Weather Parameters tab if their site differs.
    addParameter("WEATHER_FORECAST", "Advisory (0 safe, 3 unsafe)", 0, 0, 0);
    addParameter("WEATHER_TEMPERATURE", "Air temperature (C)", -50, 60, 10);
    addParameter("WEATHER_HUMIDITY", "Humidity (%)", 0, 90, 10);
    addParameter("WEATHER_DEW_POINT", "Dew point (C)", -80, 60, 10);
    addParameter("WEATHER_CLOUD_COVER", "Cloud cover (%)", 0, 55, 10);
    addParameter("WEATHER_SKY_QUALITY", "Estimated SQM (mag/arcsec^2)", 0, 30, 10);
    addParameter("WEATHER_SKY_TEMPERATURE", "Sky temperature (C)", -100, 80, 10);
    addParameter("WEATHER_RAIN_DETECTED", "Rain plate (0 dry, 1 wet)", 0, 0, 0);
    addParameter("WEATHER_BORTLE_ESTIMATE", "Estimated Bortle class", 1, 9, 10);

    setCriticalParameter("WEATHER_FORECAST");
    setCriticalParameter("WEATHER_RAIN_DETECTED");
    addDebugControl();
    return true;
}

void AstroWeather::ISGetProperties(const char *dev)
{
    INDI::Weather::ISGetProperties(dev);
    defineProperty(stationURLTP);
}

bool AstroWeather::ISNewText(const char *dev, const char *name, char *texts[],
                             char *names[], int n)
{
    if (dev && !std::strcmp(dev, getDeviceName()) &&
        stationURLTP.isNameMatch(name))
    {
        stationURLTP.update(texts, names, n);
        stationURLTP.setState(IPS_OK);
        stationURLTP.apply();
        saveConfig(true, stationURLTP.getName());
        return true;
    }
    return INDI::Weather::ISNewText(dev, name, texts, names, n);
}

bool AstroWeather::Connect()
{
    const std::string url = stationURLTP[0].getText();
    if (url.rfind("http://", 0) != 0 && url.rfind("https://", 0) != 0)
    {
        LOG_ERROR("Station URL must begin with http:// or https://");
        return false;
    }
    return true;
}

bool AstroWeather::Disconnect()
{
    return true;
}

IPState AstroWeather::updateWeather()
{
    CURL *curl = curl_easy_init();
    if (!curl)
    {
        LOG_ERROR("Unable to initialise HTTP client");
        return IPS_ALERT;
    }

    std::string response;
    char errorBuffer[CURL_ERROR_SIZE] = {0};
    curl_easy_setopt(curl, CURLOPT_URL, stationURLTP[0].getText());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errorBuffer);
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 5L);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 8L);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

    const CURLcode result = curl_easy_perform(curl);
    long status = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &status);
    curl_easy_cleanup(curl);

    if (result != CURLE_OK || status < 200 || status >= 300)
    {
        LOGF_ERROR("Station request failed: HTTP %ld, %s", status,
                   errorBuffer[0] ? errorBuffer : curl_easy_strerror(result));
        return IPS_ALERT;
    }

    try
    {
        const json report = json::parse(response);
        double value = 0;

        if (!readFiniteNumber(report, "air_c", value))
            throw std::runtime_error("air_c is missing");
        setParameterValue("WEATHER_TEMPERATURE", value);

        if (!readFiniteNumber(report, "humidity", value))
            throw std::runtime_error("humidity is missing");
        setParameterValue("WEATHER_HUMIDITY", value);

        if (!readFiniteNumber(report, "dew_c", value))
            throw std::runtime_error("dew_c is missing");
        setParameterValue("WEATHER_DEW_POINT", value);

        if (!readFiniteNumber(report, "cloud_percent", value))
            throw std::runtime_error("cloud_percent is missing");
        setParameterValue("WEATHER_CLOUD_COVER", value);

        if (!readFiniteNumber(report, "sky_c", value))
            throw std::runtime_error("sky_c is missing");
        setParameterValue("WEATHER_SKY_TEMPERATURE", value);

        // SQM and Bortle remain untouched until the station reports calibrated
        // finite values. This prevents a zero/default from masquerading as data.
        if (readFiniteNumber(report, "sqm_estimate", value))
            setParameterValue("WEATHER_SKY_QUALITY", value);
        if (readFiniteNumber(report, "bortle_estimate", value))
            setParameterValue("WEATHER_BORTLE_ESTIMATE", value);

        if (!report.contains("rain_detected") ||
            !report["rain_detected"].is_boolean() ||
            !report.contains("advisory_safe") ||
            !report["advisory_safe"].is_boolean())
            throw std::runtime_error("safety fields are missing");

        const bool raining = report["rain_detected"].get<bool>();
        const bool safe = report["advisory_safe"].get<bool>();
        setParameterValue("WEATHER_RAIN_DETECTED", raining ? 1.0 : 0.0);
        setParameterValue("WEATHER_FORECAST", safe ? 0.0 : 3.0);
        return safe ? IPS_OK : IPS_ALERT;
    }
    catch (const std::exception &error)
    {
        LOGF_ERROR("Invalid station JSON: %s", error.what());
        return IPS_ALERT;
    }
}

bool AstroWeather::saveConfigItems(FILE *fp)
{
    INDI::Weather::saveConfigItems(fp);
    stationURLTP.save(fp);
    return true;
}
