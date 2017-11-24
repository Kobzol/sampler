#include "TopdownTreeView.h"
#include "../../common/tree/TreeModel.h"
#include "../../../model/statistics/TopdownTreeCalculator.h"

#include <QVBoxLayout>

TopdownTreeView::TopdownTreeView()
{
    auto* layout = new QVBoxLayout();
    this->setLayout(layout);

    this->model = new TreeModel(std::make_unique<TreeItem>(this->headers));

    this->sortmodel = new QSortFilterProxyModel();
    this->sortmodel->setSourceModel(this->model);

    this->treeView= new QTreeView();
    this->treeView->setModel(this->sortmodel);
    this->treeView->setSortingEnabled(true);
    this->treeView->setUniformRowHeights(true);

    layout->addWidget(this->treeView);
}

void TopdownTreeView::displayTask(TaskContext& task)
{
    TopdownTreeCalculator calculator;
    auto root = std::make_unique<TreeItem>(this->headers);
    calculator.createTopdownTree(task, *root);

    this->model->replaceRoot(std::move(root));
    this->treeView->setModel(this->sortmodel);
}
