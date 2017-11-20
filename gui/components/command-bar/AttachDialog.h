#pragma once

#include <QDialog>
#include <QListView>
#include <QtWidgets/QVBoxLayout>
#include <QSortFilterProxyModel>

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
    void createProcessList(QVBoxLayout* layout);

    std::vector<Process> processes;
    QListView* listview;
    Process selectedProcess;
    QSortFilterProxyModel* filterModel;

    void createSearchBox(QVBoxLayout* layout);
};
