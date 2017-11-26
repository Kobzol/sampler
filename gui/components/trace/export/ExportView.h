#pragma once

#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>

#include <trace.h>
#include <export/exporter.h>

class ExportView: public QWidget
{
    Q_OBJECT
public:
    ExportView();

    void displayTrace(Trace* trace);

private slots:
    void handleExportFlamechart();
    void handleExportCallgrind();

private:
    QPushButton* createExportButton(QVBoxLayout* layout, const QString& label,
                            const QString& statusTip, void (ExportView::* callback)());
    void exportWithExporter(std::unique_ptr<Exporter> exporter, const std::string& suffix);

    Trace* trace = nullptr;

    std::vector<QPushButton*> exportButtons;
};
