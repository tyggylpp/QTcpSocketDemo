#include "delegate.h"

Delegate::Delegate(QObject *parent):QStyledItemDelegate(parent)
{

}

Delegate::~Delegate()
{

}

void Delegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyleOptionViewItem viewOption(option);
        initStyleOption(&viewOption,index);
        if(option.state.testFlag(QStyle::State_HasFocus))
        {
             viewOption.state^QStyle::State_HasFocus;
        }

        QStyledItemDelegate::paint(painter,viewOption,index);
        if(index.column()==CHECKBOX_COLUMN)//the first column
        {
         bool data=index.model()->data(index,Qt::UserRole).toBool();
         QStyleOptionButton checkBoxStyle;
         checkBoxStyle.state=data?QStyle::State_On:QStyle::State_Off;
         checkBoxStyle.state|=QStyle::State_Enabled;
         checkBoxStyle.iconSize=QSize(20,20);
         checkBoxStyle.rect=option.rect;
         QCheckBox checkbox;
         checkBoxStyle.iconSize=QSize(20,20);
         checkBoxStyle.rect=option.rect;
         QApplication::style()->drawPrimitive(QStyle::PE_IndicatorCheckBox,&checkBoxStyle,painter,&checkbox);
        }else if(index.column()==BUTTONDOWN_COLUMN)
        {

            QStyleOptionFocusRect rectStyle;
            //QStyleOptionButton buttonStyle;
            if(rectStyle.state.testFlag(QStyle::State_FocusAtBorder))
            {
                rectStyle.state^QStyle::State_FocusAtBorder;
            }
            rectStyle.rect=option.rect;

            QPushButton button;
            button.setFixedSize(50,20);
           //QApplication::style()->drawPrimitive(,&rectStyle,painter,&button);
        }
}

bool Delegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    QRect decorationRect =option.rect;
      QMouseEvent *mouseevent=static_cast<QMouseEvent *>(event);
      if(event->type()==QEvent::MouseButtonPress&&decorationRect.contains(mouseevent->pos()))
      {
         if(index.column()==0)
         {
             bool data=model->data(index,Qt::UserRole).toBool();
             model->setData(index,!data,Qt::UserRole);
         }
      }
      return QStyledItemDelegate::editorEvent(event,model,option,index);
}
