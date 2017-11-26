#include "SettingsDialog.h"
#include <QIntValidator>
#include <QDialogButtonBox>
#include <QtWidgets>

SettingsDialog::SettingsDialog(SettingsManager& settingsManager): settingsManager(settingsManager)
{
    this->setWindowTitle("Settings");

    auto* layout = new QVBoxLayout();
    this->setLayout(layout);
    this->resize(600, 400);

    this->samplingRate = this->addInput(layout, "Sampling rate (10-100 ms):",
                                        std::to_string(settingsManager.getSamplingRate()),
                                        new QIntValidator(10, 100, this));
    this->samplingRate->setFixedWidth(100);

    auto* buttons = new QDialogButtonBox(this);
    auto* ok = buttons->addButton(QDialogButtonBox::StandardButton::Ok);
    ok->setText("Save");
    this->connect(ok, &QPushButton::clicked, this, &QDialog::accept);

    auto* cancel = buttons->addButton(QDialogButtonBox::StandardButton::Cancel);
    this->connect(cancel, &QPushButton::clicked, this, &QDialog::close);

    layout->addWidget(buttons);
}

void SettingsDialog::accept()
{
    if (this->validate(this->samplingRate))
    {
        this->settingsManager.setSamplingRate(this->samplingRate->text().toUInt());
    }
    else
    {
        this->showError(this->samplingRate, "Invalid sampling rate");
        return;
    }

    QDialog::accept();
}

bool SettingsDialog::validate(QLineEdit* input)
{
    int pos = 0;
    QString value = input->text();
    return input->validator()->validate(value, pos) == QValidator::Acceptable;
}

QLineEdit* SettingsDialog::addInput(QVBoxLayout* layout, const std::string& label,
                                     const std::string& value, QValidator* validator)
{
    auto* input = new QLineEdit(QString::fromStdString(value));
    input->setValidator(validator);

    auto* row = new QWidget();
    auto* rowLayout = new QHBoxLayout();
    row->setLayout(rowLayout);

    rowLayout->addWidget(new QLabel(QString::fromStdString(label)), 0, Qt::AlignmentFlag::AlignLeft);
    rowLayout->addWidget(input, 1, Qt::AlignmentFlag::AlignLeft);

    layout->addWidget(row, 0, Qt::AlignmentFlag::AlignTop);

    return input;
}

void SettingsDialog::showError(QLineEdit* input, const std::string& message)
{
    QMessageBox::warning(this, "Wrong value entered", QString::fromStdString(message));
    input->setFocus(Qt::FocusReason::OtherFocusReason);
}
