#include "dragbutton.h"
#include <utils/dropsupport.h>

#include <QApplication>
#include <QDrag>
#include <QMimeData>
#include <QMouseEvent>
#include <QPixmap>
#include <QPainter>

namespace EmosWidgets {

DragButton::DragButton(QWidget *parent)
    : QToolButton{parent}
{

}

void DragButton::setDropValue(const QVariant &value)
{
    m_dropValue = value;
}

void DragButton::enterEvent(QEvent *event)
{
    Q_UNUSED(event)
    update();
}

void DragButton::leaveEvent(QEvent *event)
{
    Q_UNUSED(event)
    update();
}

void DragButton::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        m_dragStartPostion = event->pos();
    }
    QToolButton::mousePressEvent(event);
}

void DragButton::mouseMoveEvent(QMouseEvent *event)
{
    Q_UNUSED(event)
    const bool containsMouse = rect().contains(QWidget::mapFromGlobal(QCursor::pos()));
    if ((!containsMouse) || (containsMouse))
        update();

    if(!(event->buttons() & Qt::LeftButton))
           return;
    if((event->pos() - m_dragStartPostion).manhattanLength() < QApplication::startDragDistance())
        return;

    if(m_dropValue.isNull())
        return;

    auto drag = new QDrag(this);
    auto data = new Utils::DropMimeData;
    data->addValue(m_dropValue);
    drag->setMimeData(data);
    if(!this->icon().isNull())
    {
        QPixmap pixmap = this->icon().pixmap(QSize(48,48), QIcon::Normal, QIcon::Off);
        QPainter painter(&pixmap);
        painter.setCompositionMode(QPainter::CompositionMode_DestinationIn);
        painter.fillRect(pixmap.rect(), QColor(0, 0, 0, 96));
        drag->setPixmap(pixmap);
        drag->setHotSpot(QPoint(drag->pixmap().width() / 2, drag->pixmap().height() / 2));
    }

    update();
    Qt::DropAction action = drag->exec(Qt::MoveAction | Qt::CopyAction, Qt::MoveAction);
    Q_UNUSED(action)
    update();
    data->deleteLater();
    drag->deleteLater();
    setDown(false);
}
}
