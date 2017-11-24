#pragma once

#include <QDialog>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QListView>

class RunProgramDialog: public QDialog
{
    Q_OBJECT

public:
    RunProgramDialog(const std::string& program, const std::string& cwd,
                     const std::vector<std::string>& arguments, const std::vector<std::string>& environment);

    std::string getProgram() const;
    std::string getWorkingDirectory() const;
    std::vector<std::string> getArguments() const;
    std::vector<std::string> getEnvironment() const;

private slots:
    void handleRunProgram();

private:
    QLineEdit* createPathSelectRow(QVBoxLayout* layout, const QString& label, const QString& browseLabel,
                                   const std::string& initialValue, bool dir);
    QPushButton* createRunButton(QVBoxLayout* layout);
    QListView* createStringList(QVBoxLayout* layout, const QString& label,
                                const std::vector<std::string>& initialValues);
    std::vector<std::string> extractStrings(QListView* listview) const;

    QLineEdit* programEdit;
    QLineEdit* workingDirectoryEdit;
    QListView* argumentsList;
    QListView* environmentList;
};
