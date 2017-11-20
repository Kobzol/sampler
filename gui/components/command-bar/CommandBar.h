#pragma once

#include <QWidget>
#include <QHBoxLayout>
#include <QPushButton>

#include "../../model/sampler/SamplerManager.h"
#include "../../model/util/event/subscription-manager.h"
#include "../../model/sampler/StartInfo.h"

class CommandBar : public QWidget
{
Q_OBJECT

public:
    explicit CommandBar(SamplerManager& samplerManager);

private slots:
    void showAttachDialog();
    void showRunProgramDialog();
    void handleDetach();
    void handleKill();
    void handleSamplerEvent(SamplingEvent event, TaskContext* task);

private:
    void setSamplerEvents(SamplerManager& samplerManager);
    void createButtons(QHBoxLayout* layout);
    QPushButton* createCommandButton(QHBoxLayout* layout, const QString& label,
                                     int stretch, void (CommandBar::* callback)());
    void startSampler(std::unique_ptr<StartInfo> startInfo);

    QPushButton* attachButton;
    QPushButton* runProgramButton;
    QPushButton* detachButton;
    QPushButton* killButton;

    std::string program = "/home/kobzol/projects/sampler/cmake-build-debug/profiled/profiled";
    std::string cwd = "";

    SubscriptionManager subManager;
    SamplerManager& samplerManager;
};
