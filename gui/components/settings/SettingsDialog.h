#pragma once

#include <QDialog>
#include <QLineEdit>
#include <QVBoxLayout>
#include "../../model/settings/SettingsManager.h"

class SettingsDialog: public QDialog
{
    Q_OBJECT
public:
    explicit SettingsDialog(SettingsManager& settingsManager);

    virtual void accept() override;

private:
    bool validate(QLineEdit* input);
    QLineEdit* addInput(QVBoxLayout* layout, const std::string& label,
                        const std::string& value, QValidator* validator);

    SettingsManager& settingsManager;

    QLineEdit* samplingRate;

    void showError(QLineEdit* input, const std::string& message);
};
