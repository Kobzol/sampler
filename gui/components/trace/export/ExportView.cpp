#include "ExportView.h"
#include "../../../utility/Utility.h"
#include <export/flamechart-exporter.h>
#include <export/callgrind-exporter.h>

#include <QMessageBox>
#include <QtConcurrent>
#include <fstream>
#include <sstream>

static std::vector<std::string> exportTrace(Trace* trace, std::unique_ptr<Exporter> exporter,
                                            const std::string& suffix)
{
    auto timestamp = static_cast<size_t>(QDateTime::currentMSecsSinceEpoch());
    std::vector<std::string> files;

    for (size_t i = 0; i < trace->getTaskCount(); i++)
    {
        TaskContext& task = *trace->getTaskAt(i);

        std::string file = std::to_string(timestamp) + "-" + std::to_string(task.getTask().getPid()) + "." + suffix;
        files.push_back(file);
        std::ofstream fs(file);
        exporter->exportTask(task, fs);
    }

    return files;
}

ExportView::ExportView()
{
    auto* layout = new QVBoxLayout();
    this->setLayout(layout);

    this->createExportButton(layout, "Export flamechart", "You can find the flamechart visualisator at "
            "https://github.com/brendangregg/FlameGraph", &ExportView::handleExportFlamechart);
    this->createExportButton(layout, "Export callgrind", "You can find the callgrind visualisator at "
            "https://github.com/jrfonseca/gprof2dot", &ExportView::handleExportCallgrind);
    layout->addStretch(1);
}

void ExportView::displayTrace(Trace* trace)
{
    this->trace = trace;

    for (auto button : this->exportButtons)
    {
        button->setEnabled(trace != nullptr);
    }
}

void ExportView::handleExportFlamechart()
{
    this->exportWithExporter(std::make_unique<FlameChartExporter>(), "flame");
}
void ExportView::handleExportCallgrind()
{
    this->exportWithExporter(std::make_unique<CallgrindExporter>(), "cg");
}

void ExportView::exportWithExporter(std::unique_ptr<Exporter> exporter, const std::string& suffix)
{
    if (this->trace != nullptr)
    {
        auto* exporterPtr = exporter.release();
        QtConcurrent::run([this, exporterPtr, suffix]() {
            std::vector<std::string> files = exportTrace(this->trace, std::unique_ptr<Exporter>(exporterPtr), suffix);

            std::stringstream summary;
            summary << "Generated files:" << std::endl;

            for (size_t i = 0; i < files.size(); i++)
            {
                summary << files[i];
                if (i != files.size() - 1)
                {
                    summary << std::endl;
                }
            }

            auto str = summary.str();
            runOnUi(this, [this, str]() {
                QMessageBox::information(this, "Export successful",
                                         QString::fromStdString(str));
            });
        });
    }
}

QPushButton* ExportView::createExportButton(QVBoxLayout* layout, const QString& label, const QString& statusTip,
                                    void (ExportView::* callback)())
{
    auto* button = new QPushButton(label);
    this->connect(button, &QPushButton::clicked, this, callback);
    button->setEnabled(false);
    button->setStatusTip(statusTip);
    button->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    layout->addWidget(button, 0, Qt::AlignmentFlag::AlignTop);

    this->exportButtons.push_back(button);

    return button;
}
