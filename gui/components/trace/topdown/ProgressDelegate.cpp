#include "ProgressDelegate.h"
#include <QPainter>

ProgressDelegate::ProgressDelegate(QAbstractItemModel* model): model(model)
{

}

void ProgressDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    auto value = this->model->data(index, Qt::ItemDataRole::DisplayRole).toString().toUInt();

    QProgressBar progressBar;
    progressBar.setFixedWidth(50);
    progressBar.setValue(value);
    progressBar.setMaximum(100);

    auto* device = painter->device();

    progressBar.setGeometry(option.rect);
    QPoint mappedorigin = painter->deviceTransform().map(QPoint(option.rect.x(), option.rect.y()));
    painter->end();
    progressBar.render(painter->device(), mappedorigin,
                              QRegion(0, 0, option.rect.width(), option.rect.height()),
                              QWidget::RenderFlag::DrawChildren);
    painter->begin(device);
}

QSize ProgressDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    return option.rect.size();
}
