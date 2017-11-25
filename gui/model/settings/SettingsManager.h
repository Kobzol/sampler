#pragma once

#include <QSettings>

class SettingsManager
{
public:
    explicit SettingsManager(QSettings& settings);

    SettingsManager(const SettingsManager& other) = delete;
    SettingsManager operator=(const SettingsManager& other) = delete;
    SettingsManager(const SettingsManager&& other) = delete;

    int getSamplingRate() const;
    void setSamplingRate(int value);

private:
    QSettings& settings;
};
