#include "TopdownTreeView.h"
#include "../../../model/statistics/TopdownTreeCalculator.h"

#include <QVBoxLayout>
#include <sstream>

TopdownTreeView::TopdownTreeView(QWidget* parent): QWidget(parent)
{
    auto* layout = new QVBoxLayout();
    this->setLayout(layout);

    this->model = new TreeModel(std::make_unique<TreeItem>(this->headers));

    this->sortmodel = new QSortFilterProxyModel();
    this->sortmodel->setSourceModel(this->model);

    this->treeView = new QTreeView();
    this->treeView->setModel(this->sortmodel);
    this->treeView->setSortingEnabled(true);
    this->treeView->setUniformRowHeights(true);
    this->treeView->setColumnWidth(0, 400);

    layout->addWidget(this->treeView);
}

void TopdownTreeView::displayTask(TaskContext& task)
{
    TopdownTreeCalculator calculator;
    auto root = std::make_unique<TreeItem>(this->headers);

    size_t totalSamples = task.getCollector().getSamples().size();
    calculator.createTopdownTree(task, *root, [totalSamples](const TopdownTreeCalculator::CallRecord& record) {
        std::stringstream ss;
        ss << std::fixed;
        ss.precision(2);
        ss << ((double) record.samples / totalSamples) * 100.0 << " %";

        return std::vector<std::string> {
                record.function,
                std::to_string(record.samples),
                ss.str(),
                record.location
        };
    });

    this->model->replaceRoot(std::move(root));
    this->treeView->setModel(this->sortmodel);
}
