#include "RunProgramDialog.h"

#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QFileDialog>
#include <QMessageBox>

RunProgramDialog::RunProgramDialog(const std::string& program, const std::string& cwd)
{
    this->setWindowTitle("Run program");

    auto* layout = new QVBoxLayout();
    this->setLayout(layout);

    this->programEdit = this->createPathSelectRow(layout, "Program:", "Select program", program, false);
    this->workingDirectoryEdit = this->createPathSelectRow(layout, "Working directory:", "Select working directory",
                                                           cwd, true);
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

std::string RunProgramDialog::getArguments() const
{
    return this->argumentsEdit->text().toStdString();
}

QLineEdit* RunProgramDialog::createPathSelectRow(QVBoxLayout* layout, const QString& label, const QString& browseLabel,
                                                 const std::string& initialValue, bool dir)
{
    auto* row = new QWidget();
    auto* rowLayout = new QHBoxLayout();
    row->setLayout(rowLayout);
    layout->addWidget(row);

    rowLayout->addWidget(new QLabel(label));
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

    return edit;
}

QPushButton* RunProgramDialog::createRunButton(QVBoxLayout* layout)
{
    auto* button = new QPushButton("Run program");
    this->connect(button, &QPushButton::clicked, this, &RunProgramDialog::handleRunProgram);
    layout->addWidget(button);
    return button;
}

void RunProgramDialog::handleRunProgram()
{
    if (this->programEdit->text().isEmpty())
    {
        QMessageBox(QMessageBox::Icon::Warning, "Error", "You have to select a program",
                   QMessageBox::StandardButton::Ok, this).exec();
    }
    else this->accept();
}
