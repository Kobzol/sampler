#pragma once

#include <QAbstractItemModel>
#include <QStringList>
#include <QModelIndex>

#include "TreeItem.h"

class TreeModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    explicit TreeModel(std::unique_ptr<TreeItem> root, QObject* parent = nullptr);

    QVariant data(const QModelIndex &index, int role) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    TreeItem* getRoot();
    void replaceRoot(std::unique_ptr<TreeItem> root);

private:
    std::unique_ptr<TreeItem> root;
};
