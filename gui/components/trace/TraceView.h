#pragma once

#include <QWidget>
#include <QLabel>

#include "../../model/sampler/SamplerManager.h"
#include "topdown/TopdownGroupView.h"

class TraceView: public QWidget
{
    Q_OBJECT
public:
    explicit TraceView(SamplerManager& manager);

private:
    void handleSamplerEvent(SamplingEvent event, TaskContext* task);
    size_t countSamples(Trace& trace);

    SamplerManager& manager;

    QLabel* label;
    TopdownGroupView* topdownGroup;
};
