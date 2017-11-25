#include "TraceView.h"
#include "../../utility/Utility.h"

#include <QVBoxLayout>

TraceView::TraceView(SamplerManager& manager): manager(manager)
{
    auto* layout = new QVBoxLayout();
    this->setLayout(layout);

    this->label = new QLabel("No trace generated yet");
    layout->addWidget(this->label);

    this->content = new QTabWidget();
    layout->addWidget(this->content);

    this->topdownGroup = new TopdownGroupView();
    this->content->addTab(this->topdownGroup, "Top-down");
    this->content->setTabToolTip(this->content->count() - 1, "Top down view of hot functions");

    this->exportView = new ExportView();
    this->content->addTab(this->exportView, "Export");
    this->content->setTabToolTip(this->content->count() - 1, "Export trace for external visualisation");

    manager.onSamplerEvent().subscribe([this](SamplingEvent event, TaskContext* context) {
        runOnUi(this, [this, event, context]() {
            this->handleSamplerEvent(event, context);
        });
    });
}

void TraceView::handleSamplerEvent(SamplingEvent event, TaskContext* task)
{
    if (event == SamplingEvent::Start)
    {
        this->label->setText("Trace in progress...");
        this->exportView->displayTrace(nullptr);
    }
    else if (event == SamplingEvent::Exit)
    {
        Trace& trace = *this->manager.getSampler()->getTrace();
        this->label->setText(QString::fromStdString(this->createTraceLabel(trace)));
        this->exportView->displayTrace(&trace);
        this->topdownGroup->displayTrace(trace);
    }
}

size_t TraceView::countSamples(Trace& trace)
{
    size_t count = 0;
    for (int t = 0; t < trace.getTaskCount(); t++)
    {
        count += trace.getTaskAt(static_cast<size_t>(t))->getCollector().getSamples().size();
    }
    return count;
}

std::string TraceView::createTraceLabel(Trace& trace)
{
    return "Trace completed, duration " + std::to_string(trace.getDuration()) +
                       " ms, " + std::to_string(this->countSamples(trace)) + " samples";
}
