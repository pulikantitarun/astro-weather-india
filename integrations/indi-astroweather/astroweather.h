#pragma once

// SPDX-License-Identifier: AGPL-3.0-or-later
// Copyright (C) 2026 Tarun and contributors

#include "indiweather.h"
#include "indipropertytext.h"

class AstroWeather : public INDI::Weather
{
    public:
        AstroWeather();
        ~AstroWeather() override;

        const char *getDefaultName() override;
        bool initProperties() override;
        void ISGetProperties(const char *dev) override;
        bool ISNewText(const char *dev, const char *name, char *texts[],
                       char *names[], int n) override;
        bool Connect() override;
        bool Disconnect() override;

    protected:
        IPState updateWeather() override;
        bool saveConfigItems(FILE *fp) override;

    private:
        INDI::PropertyText stationURLTP {1};
};
