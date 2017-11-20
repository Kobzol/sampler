#pragma once

#include <QDialog>
#include <QLineEdit>
#include <QtWidgets/QVBoxLayout>

class RunProgramDialog: public QDialog
{
    Q_OBJECT

public:
    RunProgramDialog(const std::string& program, const std::string& cwd);

    std::string getProgram() const;
    std::string getWorkingDirectory() const;
    std::string getArguments() const;

private slots:
    void handleRunProgram();

private:
    QLineEdit* createPathSelectRow(QVBoxLayout* layout, const QString& label, const QString& browseLabel,
                                   const std::string& initialValue, bool dir);
    QPushButton* createRunButton(QVBoxLayout* layout);

    QLineEdit* programEdit;
    QLineEdit* workingDirectoryEdit;
    QLineEdit* argumentsEdit;
};
