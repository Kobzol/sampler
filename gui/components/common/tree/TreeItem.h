#pragma once

#include <QVariant>
#include <QList>

#include <vector>
#include <memory>

class TreeItem
{
public:
    explicit TreeItem(const std::vector<std::string>& columns, TreeItem* parentItem = nullptr);

    TreeItem(const TreeItem& TreeItem) = delete;
    TreeItem operator=(const TreeItem& TreeItem) = delete;
    TreeItem(const TreeItem&& TreeItem) = delete;

    void addChild(TreeItem* child);
    TreeItem* getChild(int row);

    int childCount() const;
    int columnCount() const;

    std::string data(int column) const;
    int row() const;

    TreeItem* getParent();

private:
    int getChildRow(const TreeItem* item) const;

    std::vector<std::string> columns;
    TreeItem* parent;

    std::vector<std::unique_ptr<TreeItem>> children;
};
