#include "overlaywidget.h"

#include <QEvent>
#include <QPainter>

namespace EmosWidgets
{

OverlayWidget::OverlayWidget(QWidget *parent)
{
    setAttribute(Qt::WA_TransparentForMouseEvents);
    if (parent)
        attachToWidget(parent);
}

void OverlayWidget::setPaintFunction(const OverlayWidget::PaintFunction &paint)
{
    m_paint = paint;
}

bool OverlayWidget::eventFilter(QObject *obj, QEvent *ev)
{
    if (obj == parent() && ev->type() == QEvent::Resize)
        resizeToParent();
    return QWidget::eventFilter(obj, ev);
}

void OverlayWidget::paintEvent(QPaintEvent *ev)
{
    if (m_paint) {
        QPainter p(this);
        m_paint(this, p, ev);
    }
}

void OverlayWidget::attachToWidget(QWidget *parent)
{
    if (parentWidget())
        parentWidget()->removeEventFilter(this);
    setParent(parent);
    if (parent) {
        parent->installEventFilter(this);
        resizeToParent();
        raise();
    }
}

void OverlayWidget::resizeToParent()
{
    if(!parentWidget())
            return;
    setGeometry(QRect(QPoint(0, 0), parentWidget()->size()));
}

}
