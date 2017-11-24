#pragma once

#include <QWidget>
#include <trace.h>
#include <QTabWidget>

class TopdownGroupView: public QTabWidget
{
    Q_OBJECT
public:
    void displayTrace(Trace& trace);

private:
    void removeAllTabs();

    void createTab(TaskContext* task);
};
