#include "CommandBar.h"

#include <QCoreApplication>
#include <QMessageBox>

#include "AttachDialog.h"
#include "RunProgramDialog.h"
#include "../../utility/Utility.h"

CommandBar::CommandBar(SamplerManager& samplerManager): samplerManager(samplerManager)
{
    auto* layout = new QHBoxLayout();
    this->setLayout(layout);

    this->createButtons(layout);
    this->setSamplerEvents(samplerManager);
}
void CommandBar::createButtons(QHBoxLayout* layout)
{
    this->attachButton = this->createCommandButton(layout, "Attach to process", 0, &CommandBar::showAttachDialog);
    this->attachButton->setStatusTip("Attach to a running process");

    this->runProgramButton = this->createCommandButton(layout, "Run program", 0, &CommandBar::showRunProgramDialog);
    this->runProgramButton->setStatusTip("Run and profile a program executable");

    this->detachButton = this->createCommandButton(layout, "Detach", 0, &CommandBar::handleDetach);
    this->detachButton->setStatusTip("Detach from a running program");
    this->detachButton->setEnabled(false);

    this->killButton = this->createCommandButton(layout, "Kill process", 1, &CommandBar::handleKill);
    this->killButton->setStatusTip("Kill profiled process");
    this->killButton->setEnabled(false);
}
void CommandBar::setSamplerEvents(SamplerManager& samplerManager)
{
    this->subManager += samplerManager.onSamplerEvent().subscribe([this](SamplingEvent event, TaskContext* task) {
        runOnUi(this, [this, event, task]() {
            this->handleSamplerEvent(event, task);
        });
    });
    this->subManager += samplerManager.onSamplerError().subscribe([this](const std::exception& error) {
        std::string msg(error.what());
        runOnUi(this, [this, msg]() {
            QMessageBox(QMessageBox::Icon::Warning, "Profiling error", msg.c_str(),
                        QMessageBox::StandardButton::Ok, this).exec();
        });
    });
}

QPushButton* CommandBar::createCommandButton(QHBoxLayout* layout, const QString& label,
                                             int stretch, void (CommandBar::* callback)())
{
    auto* button = new QPushButton(label);
    this->connect(button, &QPushButton::clicked, this, callback);
    layout->addWidget(button, stretch, Qt::AlignmentFlag::AlignLeft);
    return button;
}

void CommandBar::showAttachDialog()
{
    AttachDialog dialog;
    if (dialog.exec())
    {
        auto process = dialog.getSelectedProcess();
        this->startSampler(std::make_unique<PidStartInfo>(process.getPid()));
    }
}
void CommandBar::showRunProgramDialog()
{
    RunProgramDialog dialog(this->program, this->cwd);
    if (dialog.exec())
    {
        this->program = dialog.getProgram();
        this->cwd = dialog.getWorkingDirectory();
        this->startSampler(std::make_unique<ProgramStartInfo>(this->program, this->cwd));
    }
}

void CommandBar::handleDetach()
{
    this->samplerManager.stopSampler();
}

void CommandBar::handleSamplerEvent(SamplingEvent event, TaskContext* task)
{
    if (event == SamplingEvent::Start)
    {
        this->attachButton->setEnabled(false);
        this->runProgramButton->setEnabled(false);
        this->detachButton->setEnabled(true);
        this->killButton->setEnabled(true);
    }
    else if (event == SamplingEvent::Exit)
    {
        this->attachButton->setEnabled(true);
        this->runProgramButton->setEnabled(true);
        this->detachButton->setEnabled(false);
        this->killButton->setEnabled(false);
    }
}

void CommandBar::startSampler(std::unique_ptr<StartInfo> startInfo)
{
    this->samplerManager.startSampler(200, std::move(startInfo));
}

void CommandBar::handleKill()
{
    this->samplerManager.killProcess();
}
