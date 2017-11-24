#pragma once

#include <QMainWindow>
#include "../model/sampler/SamplerManager.h"
#include "trace/topdown/TopdownTreeView.h"
#include "trace/TraceView.h"

class MainWindow: public QMainWindow
{
Q_OBJECT

public:
    explicit MainWindow(SamplerManager& samplerManager);

private slots:
    void handleMenuExit();

private:
    void createMenu();
    void createContent();

    SamplerManager& samplerManager;
    TraceView* traceView;
};
