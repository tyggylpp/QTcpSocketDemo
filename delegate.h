#ifndef DELEGATE_H
#define DELEGATE_H

#include <QObject>
#include<QStyledItemDelegate>
#include<QCheckBox>
#include<QApplication>
#include<QMouseEvent>
#include<QPushButton>
#ifndef CHECKBOX_COLUMN
#define CHECKBOX_COLUMN 0
#endif
#ifndef BUTTONDOWN_COLUMN
#define BUTTONDOWN_COLUMN 4
#endif
class Delegate : public QStyledItemDelegate
{
public:
    Delegate(QObject *parent);
    ~Delegate();
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index);
};

#endif // DELEGATE_H
