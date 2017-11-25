#include "ExportView.h"
#include <QVBoxLayout>
#include <QMessageBox>
#include <fstream>
#include <sstream>

#include <export/flamechart-exporter.h>
#include <unix/utility/utility.h>

static std::vector<std::string> exportTrace(Trace* trace)
{
    FlameChartExporter exporter;
    size_t timestamp = getTimestamp();
    std::vector<std::string> files;

    for (size_t i = 0; i < trace->getTaskCount(); i++)
    {
        TaskContext& task = *trace->getTaskAt(i);
        auto lines = exporter.createCondensedFrames(task);

        std::string file = std::to_string(timestamp) + "-" + std::to_string(task.getTask().getPid()) + ".trace";
        files.push_back(file);
        std::ofstream fs(file);
        for (auto& line : lines)
        {
            fs << line << std::endl;
        }
    }

    return files;
}

ExportView::ExportView()
{
    auto* layout = new QVBoxLayout();
    this->setLayout(layout);

    this->exportFlamechart = new QPushButton("Export flamechart");
    this->connect(this->exportFlamechart, &QPushButton::clicked, [this]() {
       this->handleExportFlamechart();
    });
    this->exportFlamechart->setEnabled(false);
    this->exportFlamechart->setStatusTip("You can find the flamechart generator at "
                                                 "https://github.com/brendangregg/FlameGraph");
    this->exportFlamechart->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    layout->addWidget(this->exportFlamechart, 0, Qt::AlignmentFlag::AlignTop);
}

void ExportView::displayTrace(Trace* trace)
{
    this->trace = trace;

    this->exportFlamechart->setEnabled(trace != nullptr);
}

void ExportView::handleExportFlamechart()
{
    if (this->trace != nullptr)
    {
        std::vector<std::string> files = exportTrace(this->trace);

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

        QMessageBox::information(this, "Export successful",
                                 QString::fromStdString(summary.str()));
    }
}
