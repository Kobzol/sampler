#include "AttachDialog.h"

#include <QVBoxLayout>
#include <QStringListModel>
#include <QListView>
#include <QPushButton>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QtWidgets>

#include "../../model/process/ProcessLister.h"

AttachDialog::AttachDialog()
{
    this->setWindowTitle("Attach to a process");

    auto* layout = new QVBoxLayout();
    this->setLayout(layout);

    ProcessLister lister;
    this->processes = lister.getRunningProcesses();
    this->createProcessList(layout);

    auto* attachButton = new QPushButton("Attach");
    this->connect(attachButton, &QPushButton::clicked, this, &AttachDialog::attachToSelectedProcess);
    layout->addWidget(attachButton);
}

void AttachDialog::createProcessList(QVBoxLayout* layout)
{
    QStringList list;
    for (auto& process: this->processes)
    {
        list << QString::fromStdString(std::__cxx11::to_string(process.getPid()) + ": " + process.getName());
    }

    auto* model = new QStringListModel(this);
    model->setStringList(list);

    this->listview = new QListView(this);
    this->listview->setModel(model);
    this->listview->setSelectionMode(QAbstractItemView::SingleSelection);
    this->listview->setEditTriggers(QAbstractItemView::NoEditTriggers);
    this->connect(this->listview, &QListView::doubleClicked, this, &AttachDialog::attachToSelectedProcess);
    layout->addWidget(this->listview);
}

void AttachDialog::attachToSelectedProcess()
{
    auto indices = this->listview->selectionModel()->selectedIndexes();
    if (indices.isEmpty())
    {
        QMessageBox(QMessageBox::Icon::Warning, "Error", "You have to select a process",
                    QMessageBox::StandardButton::Ok, this).exec();
    }
    else
    {
        this->selectedProcess = this->processes[indices.first().row()];
        this->accept();
    }
}

Process AttachDialog::getSelectedProcess() const
{
    return this->selectedProcess;
}
