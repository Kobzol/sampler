#pragma once

#include <QMainWindow>
#include <QSettings>

#include "../model/sampler/SamplerManager.h"
#include "../model/settings/SettingsManager.h"
#include "trace/topdown/TopdownTreeView.h"
#include "trace/TraceView.h"

class MainWindow: public QMainWindow
{
Q_OBJECT

public:
    explicit MainWindow(SamplerManager& samplerManager, SettingsManager& settingsManager);

private slots:
    void handleMenuSettings();
    void handleMenuExit();

private:
    void createMenu();
    void createContent();

    SamplerManager& samplerManager;
    SettingsManager& settingsManager;
    TraceView* traceView;
};
