#pragma once

#include <QWidget>
#include <QTableWidget>
#include <QSortFilterProxyModel>
#include <QTreeView>

#include <taskcontext.h>
#include "../../common/tree/TreeModel.h"

class TopdownTreeView: public QWidget
{
    Q_OBJECT
public:
    explicit TopdownTreeView(QWidget* parent = nullptr);

    void displayTask(TaskContext& task);

private:
    TreeModel* model;
    QTreeView* treeView;

    std::vector<std::string> headers {"Function", "Cumul. samples #", "Cumul. %", "Location" };
};
