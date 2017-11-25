#pragma once

#include <QWidget>
#include <QLabel>

#include "../../model/sampler/SamplerManager.h"
#include "topdown/TopdownGroupView.h"
#include "export/ExportView.h"

class TraceView: public QWidget
{
    Q_OBJECT
public:
    explicit TraceView(SamplerManager& manager);

private:
    void handleSamplerEvent(SamplingEvent event, TaskContext* task);
    size_t countSamples(Trace& trace);
    std::string createTraceLabel(Trace& trace);

    SamplerManager& manager;

    QLabel* label;
    QTabWidget* content;
    TopdownGroupView* topdownGroup;
    ExportView* exportView;
};
