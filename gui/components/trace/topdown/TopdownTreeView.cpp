#include "TopdownTreeView.h"
#include "../../../model/statistics/TopdownTreeCalculator.h"
#include "../../../utility/Utility.h"
#include "ProgressDelegate.h"

#include <QVBoxLayout>
#include <QtConcurrent>

TopdownTreeView::TopdownTreeView(QWidget* parent): QWidget(parent)
{
    auto* layout = new QVBoxLayout();
    this->setLayout(layout);

    this->model = new TreeModel(std::make_unique<TreeItem>(this->headers));

    this->treeView = new QTreeView();
    this->treeView->setModel(this->model);
    this->treeView->setSortingEnabled(false);
    this->treeView->setUniformRowHeights(true);
    this->treeView->setColumnWidth(0, 400);
    this->treeView->setItemDelegateForColumn(2, new ProgressDelegate(this->model));

    layout->addWidget(this->treeView);
}

void TopdownTreeView::displayTask(TaskContext& task)
{
    QtConcurrent::run([this, &task]() {
        TopdownTreeCalculator calculator;
        auto* root = new TreeItem(this->headers);

        size_t totalSamples = task.getCollector().getSamples().size();
        calculator.createTopdownTree(task, *root, [totalSamples](const TopdownTreeCalculator::CallRecord& record) {
            double percent = ((double) record.getSamples() / totalSamples) * 100.0;
            auto name = record.getFunction().empty() ? "(unknown)" : record.getFunction();
            auto location = record.getLocation().empty() ? "(unknown)" : record.getLocation();
            auto module = record.getModule().empty() ? "(unknown)" : record.getModule();

            return std::vector<std::string> {
                    name,
                    std::to_string(record.getSamples()),
                    std::to_string((int) percent),
                    location,
                    module
            };
        });

        runOnUi(this, [this, root]() {
            this->model->replaceRoot(std::unique_ptr<TreeItem>(root));
        });
    });
}
