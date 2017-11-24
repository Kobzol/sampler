#include "MainWindow.h"
#include "command-bar/CommandBar.h"
#include "../utility/Utility.h"

#include <QMenuBar>
#include <QApplication>
#include <QStatusBar>

MainWindow::MainWindow(SamplerManager& samplerManager): samplerManager(samplerManager)
{
    this->setWindowTitle("Profiler");
    this->setMinimumSize(800, 600);
    this->resize(800, 600);
    this->statusBar()->setVisible(true);

    this->createMenu();
    this->createContent();
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

    this->traceView = new TraceView(this->samplerManager);

    layout->setMargin(5);
    layout->addWidget(new CommandBar(this->samplerManager));
    layout->addWidget(this->traceView);
}
