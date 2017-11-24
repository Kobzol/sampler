#include "TraceView.h"
#include "../../utility/Utility.h"

#include <QVBoxLayout>

TraceView::TraceView(SamplerManager& manager): manager(manager)
{
    auto* layout = new QVBoxLayout();
    this->setLayout(layout);

    this->label = new QLabel("No trace generated yet");
    layout->addWidget(this->label);

    this->topdownGroup = new TopdownGroupView();
    layout->addWidget(this->topdownGroup);

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
    }
    else if (event == SamplingEvent::Exit)
    {
        Trace& trace = *this->manager.getSampler()->getTrace();
        std::string text = "Trace completed, duration " + std::to_string(trace.getDuration()) +
                           " ms, " + std::to_string(this->countSamples(trace)) + " samples";
        this->label->setText(QString::fromStdString(text));
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
