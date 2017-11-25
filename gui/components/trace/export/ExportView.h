#pragma once

#include <QWidget>
#include <trace.h>
#include <QtWidgets/QPushButton>

class ExportView: public QWidget
{
    Q_OBJECT
public:
    ExportView();

    void displayTrace(Trace* trace);

private slots:
    void handleExportFlamechart();

private:
    Trace* trace = nullptr;

    QPushButton* exportFlamechart;
};
