#pragma once

#include <QSettings>

class SettingsManager
{
public:
    explicit SettingsManager(QSettings& settings);

    SettingsManager(const SettingsManager& other) = delete;
    SettingsManager operator=(const SettingsManager& other) = delete;
    SettingsManager(const SettingsManager&& other) = delete;

    uint32_t getSamplingRate() const;
    void setSamplingRate(uint32_t value);

private:
    QSettings& settings;
};
