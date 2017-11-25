#include "TopdownTreeView.h"
#include "../../../model/statistics/TopdownTreeCalculator.h"
#include "../../../utility/Utility.h"

#include <QVBoxLayout>
#include <QtConcurrent>
#include <sstream>

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

    layout->addWidget(this->treeView);
}

void TopdownTreeView::displayTask(TaskContext& task)
{
    QtConcurrent::run([this, &task]() {
        TopdownTreeCalculator calculator;
        auto* root = new TreeItem(this->headers);

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

        runOnUi(this, [this, root]() {
            this->model->replaceRoot(std::unique_ptr<TreeItem>(root));
        });
    });
}
