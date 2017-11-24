#include "MainWindow.h"
#include "command-bar/CommandBar.h"
#include "../utility/Utility.h"
#include "trace/topdown/TopdownTreeView.h"

#include <QMenuBar>
#include <QApplication>
#include <QStatusBar>
#include <export/flamechart-exporter.h>
#include <fstream>

MainWindow::MainWindow(SamplerManager& samplerManager): samplerManager(samplerManager)
{
    this->setWindowTitle("Profiler");
    this->setMinimumSize(800, 600);
    this->resize(800, 600);
    this->statusBar()->setVisible(true);

    this->createMenu();
    this->createContent();

    samplerManager.onSamplerEvent().subscribe([this](SamplingEvent event, TaskContext* task) {
        if (event == SamplingEvent::Exit)
        {
            auto& ctx = *this->samplerManager.getSampler()->getTaskAt(0);
            this->topdownTree->displayTask(ctx);
            FlameChartExporter exporter;
            auto lines = exporter.createCondensedFrames(ctx);
            std::ofstream fs("samples.txt");
            for (auto& line: lines)
            {
                fs << line << std::endl;
            }
            fs.close();
        }
    });
}

void MainWindow::handleMenuExit()
{
    QApplication::quit();
}

void MainWindow::createMenu()
{
    // File
    auto* fileMenu = this->menuBar()->addMenu("File");

    auto* exitAction = new QAction("&Exit", this);
    exitAction->setShortcuts(QKeySequence::Quit);
    exitAction->setStatusTip("Exit profiler");
    this->connect(exitAction, &QAction::triggered, this, &MainWindow::handleMenuExit);
    fileMenu->addAction(exitAction);
}

void MainWindow::createContent()
{
    auto widget = new QWidget();
    this->setCentralWidget(widget);

    auto* layout = new QVBoxLayout();
    widget->setLayout(layout);

    this->topdownTree = new TopdownTreeView();

    layout->setMargin(5);
    layout->addWidget(new CommandBar(this->samplerManager));
    layout->addWidget(this->topdownTree);
}
