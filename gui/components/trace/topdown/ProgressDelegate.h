#pragma once

#include <QStyledItemDelegate>
#include <QProgressBar>

class ProgressDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit ProgressDelegate(QAbstractItemModel* model);
    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const override;
    QSize sizeHint(const QStyleOptionViewItem &option,
                   const QModelIndex &index) const override;

private:
    QAbstractItemModel* model;
};
