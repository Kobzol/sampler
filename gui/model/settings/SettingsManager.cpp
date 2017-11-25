#include "SettingsManager.h"

SettingsManager::SettingsManager(QSettings& settings): settings(settings)
{

}

int SettingsManager::getSamplingRate() const
{
    int samplingRate = 10;
    return this->settings.value("samplingRate", samplingRate).toInt();
}

void SettingsManager::setSamplingRate(int value)
{
    this->settings.setValue("samplingRate", value);
}
