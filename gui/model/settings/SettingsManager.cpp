#include "SettingsManager.h"

SettingsManager::SettingsManager(QSettings& settings): settings(settings)
{

}

uint32_t SettingsManager::getSamplingRate() const
{
    uint32_t samplingRate = 10;
    return this->settings.value("samplingRate", samplingRate).toUInt();
}

void SettingsManager::setSamplingRate(uint32_t value)
{
    this->settings.setValue("samplingRate", value);
}
