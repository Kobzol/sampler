#include "TopdownGroupView.h"
#include "TopdownTreeView.h"

void TopdownGroupView::displayTrace(Trace& trace)
{
    this->removeAllTabs();

    for (int i = 0; i < trace.getTaskCount(); i++)
    {
        auto* task = trace.getTaskAt(i);
        this->createTab(task);
    }
}

void TopdownGroupView::removeAllTabs()
{
    int count = this->count();
    for (int i = 0; i < count; i++)
    {
        this->removeTab(0);
    }
}

void TopdownGroupView::createTab(TaskContext* task)
{
    auto name = task->getTask().getName() + " (" + std::to_string(task->getTask().getPid()) + ")";
    auto view = new TopdownTreeView();
    view->displayTask(*task);
    this->addTab(view, QString::fromStdString(name));
}
