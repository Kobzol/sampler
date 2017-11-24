#include "RunProgramDialog.h"

#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QFileDialog>
#include <QMessageBox>
#include <QtCore/QStringListModel>

RunProgramDialog::RunProgramDialog(const std::string& program, const std::string& cwd,
                                   const std::vector<std::string>& arguments,
                                   const std::vector<std::string>& environment)
{
    this->setWindowTitle("Run program");

    auto* layout = new QVBoxLayout();
    this->setLayout(layout);

    this->programEdit = this->createPathSelectRow(layout, "Program:", "Select program", program, false);
    this->workingDirectoryEdit = this->createPathSelectRow(layout, "Working directory:", "Select working directory",
                                                           cwd, true);
    this->argumentsList = this->createStringList(layout, "Arguments", arguments);
    this->environmentList = this->createStringList(layout, "Environment variables", environment);

    this->createRunButton(layout);
}

std::string RunProgramDialog::getProgram() const
{
    return this->programEdit->text().toStdString();
}

std::string RunProgramDialog::getWorkingDirectory() const
{
    return this->workingDirectoryEdit->text().toStdString();
}

std::vector<std::string> RunProgramDialog::getArguments() const
{
    return this->extractStrings(this->argumentsList);
}
std::vector<std::string> RunProgramDialog::getEnvironment() const
{
    return this->extractStrings(this->environmentList);
}

QLineEdit* RunProgramDialog::createPathSelectRow(QVBoxLayout* layout, const QString& label, const QString& browseLabel,
                                                 const std::string& initialValue, bool dir)
{
    auto* row = new QWidget();
    auto* rowLayout = new QHBoxLayout();
    rowLayout->setMargin(0);
    row->setLayout(rowLayout);

    auto* edit = new QLineEdit(initialValue.c_str());
    edit->setMinimumWidth(300);
    rowLayout->addWidget(edit);

    auto* button = new QPushButton("Browse");
    this->connect(button, &QPushButton::clicked, [this, dir, browseLabel, edit]() {
        auto filename = dir ?
                        QFileDialog::getExistingDirectory(this, browseLabel) :
                        QFileDialog::getOpenFileName(this, browseLabel);
        if (!filename.isEmpty())
        {
            edit->setText(filename);
        }
    });
    rowLayout->addWidget(button);

    layout->addWidget(new QLabel(label));
    layout->addWidget(row);

    return edit;
}

QPushButton* RunProgramDialog::createRunButton(QVBoxLayout* layout)
{
    auto* button = new QPushButton("Run program");
    button->setStyleSheet("margin-top: 30; padding: 5;");

    this->connect(button, &QPushButton::clicked, this, &RunProgramDialog::handleRunProgram);
    layout->addWidget(button);
    return button;
}

void RunProgramDialog::handleRunProgram()
{
    if (this->programEdit->text().isEmpty())
    {
        QMessageBox::warning(this, "Error", "You have to select a program",
                             QMessageBox::StandardButton::Ok);
    }
    else this->accept();
}

QListView* RunProgramDialog::createStringList(QVBoxLayout* layout, const QString& label,
                                              const std::vector<std::string>& initialValues)
{
    auto* model = new QStringListModel();
    QStringList list;
    for (auto& value: initialValues)
    {
        list.append(QString::fromStdString(value));
    }
    model->setStringList(list);

    auto* listview = new QListView();
    listview->setModel(model);
    listview->setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);
    listview->setEditTriggers(QAbstractItemView::EditTrigger::DoubleClicked);
    listview->setFixedHeight(80);

    auto* row = new QWidget();
    auto* rowLayout = new QHBoxLayout();
    row->setLayout(rowLayout);

    auto* addButton = new QPushButton("Add");
    connect(addButton, &QPushButton::clicked, [model, listview]() {
        model->insertRow(model->rowCount());
        auto index = model->index(model->rowCount() - 1);
        model->setData(index, "");
        listview->setCurrentIndex(index);
    });
    rowLayout->addWidget(addButton);

    auto* removeButton = new QPushButton("Remove");
    connect(removeButton, &QPushButton::clicked, [model, listview]() {
        auto index = listview->selectionModel()->currentIndex();

        if (model->rowCount() > 0 && index.isValid())
        {
            model->removeRow(index.row());
        }
    });
    rowLayout->addWidget(removeButton);

    layout->addWidget(new QLabel(label));
    layout->addWidget(listview);
    layout->addWidget(row);

    return listview;
}

std::vector<std::string> RunProgramDialog::extractStrings(QListView* listview) const
{
    auto* model = listview->model();
    std::vector<std::string> rows;

    for (int i = 0; i < model->rowCount(); i++)
    {
        rows.push_back(model->data(model->index(i, 0)).toString().toStdString());
    }

    return rows;
}
