#pragma once

#include <QDialog>
#include <QListView>
#include <QtWidgets/QVBoxLayout>

#include "../../model/process/Process.h"

class AttachDialog: public QDialog
{
    Q_OBJECT

public:
    AttachDialog();

    Process getSelectedProcess() const;

private slots:
    void attachToSelectedProcess();

private:
    std::vector<Process> processes;
    QListView* listview;
    Process selectedProcess;

    void createProcessList(QVBoxLayout* layout);
};
