#include "TreeModel.h"

TreeModel::TreeModel(std::unique_ptr<TreeItem> root, QObject* parent)
        : QAbstractItemModel(parent), root(std::move(root))
{

}

QModelIndex TreeModel::index(int row, int column, const QModelIndex &parent)
const
{
    if (!hasIndex(row, column, parent))
    {
        return QModelIndex();
    }

    TreeItem* parentItem;
    if (!parent.isValid())
    {
        parentItem = this->root.get();
    }
    else parentItem = static_cast<TreeItem*>(parent.internalPointer());

    TreeItem* childItem = parentItem->getChild(row);
    if (childItem)
    {
        return this->createIndex(row, column, childItem);
    }
    else return QModelIndex();
}

QModelIndex TreeModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
    {
        return QModelIndex();
    }

    auto* childItem = static_cast<TreeItem*>(index.internalPointer());
    TreeItem* parentItem = childItem->getParent();

    if (parentItem == this->root.get())
    {
        return QModelIndex();
    }

    return this->createIndex(parentItem->row(), 0, parentItem);
}

int TreeModel::rowCount(const QModelIndex &parent) const
{
    TreeItem* parentItem;
    if (parent.column() > 0)
    {
        return 0;
    }

    if (!parent.isValid())
    {
        parentItem = this->root.get();
    }
    else parentItem = static_cast<TreeItem*>(parent.internalPointer());

    return parentItem->childCount();
}

int TreeModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
    {
        return static_cast<TreeItem*>(parent.internalPointer())->columnCount();
    }
    else return this->root->columnCount();
}

QVariant TreeModel::data(const QModelIndex &index, int role) const
{
    if (index.isValid() && (role == Qt::ItemDataRole::ToolTipRole ||
                            role == Qt::ItemDataRole::DisplayRole))
    {
        return QString::fromStdString(static_cast<TreeItem*>(index.internalPointer())->data(index.column()));
    }

    return QVariant();
}

Qt::ItemFlags TreeModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
    {
        return Qt::NoItemFlags;
    }

    return QAbstractItemModel::flags(index);
}

QVariant TreeModel::headerData(int section, Qt::Orientation orientation,
                               int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
    {
        return QString::fromStdString(this->root->data(section));
    }

    return QVariant();
}

TreeItem* TreeModel::getRoot()
{
    return this->root.get();
}

void TreeModel::replaceRoot(std::unique_ptr<TreeItem> root)
{
    this->beginResetModel();
    this->root = std::move(root);
    this->endResetModel();
}
