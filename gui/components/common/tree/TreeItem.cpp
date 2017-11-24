#include "TreeItem.h"

#include <cassert>

TreeItem::TreeItem(const std::vector<std::string>& columns, TreeItem* parent)
        : columns(columns), parent(parent)
{

}

void TreeItem::addChild(TreeItem* item)
{
    item->parent = this;
    this->children.push_back(std::unique_ptr<TreeItem>(item));
}

TreeItem* TreeItem::getChild(int row)
{
    return this->children[row].get();
}

int TreeItem::childCount() const
{
    return static_cast<int>(this->children.size());
}

int TreeItem::row() const
{
    if (this->parent)
    {
        return this->parent->getChildRow(this);
    }

    return 0;
}

int TreeItem::columnCount() const
{
    return static_cast<int>(this->columns.size());
}

std::string TreeItem::data(int column) const
{
    return this->columns[column];
}

TreeItem* TreeItem::getParent()
{
    return this->parent;
}

int TreeItem::getChildRow(const TreeItem* item) const
{
    for (int i = 0; i < this->children.size(); i++)
    {
        if (this->children[i].get() == item)
        {
            return i;
        }
    }

    assert(false);
    return -1;
}
