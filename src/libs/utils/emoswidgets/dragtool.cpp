#include "dragtool.h"

#include <QIcon>
#include <QPainter>
#include <QCursor>
#include <QDrag>
#include <QMouseEvent>
#include <QMimeData>
#include <QApplication>
#include <utils/dropsupport.h>

namespace EmosWidgets {
class DragToolPrivate {
public:
    QIcon icon = QIcon();
    QSize iconSize = QSize(16, 16);
    QString title;
    QString tip = "";
    QList<QVariant> dropValue;
    int tipWidth = 50;
    bool disableFrame = false;
    bool framePainted = false;
    QPoint m_dragStartPostion;
    QColor m_color = QColor();
};

DragTool::DragTool(const QString &title, QWidget *parent)
    : QWidget{parent},
      d(new DragToolPrivate)
{
    setMinimumHeight(20);
    setMaximumHeight(60);
    d->title = title;
    QMargins margins = contentsMargins();
    if (margins.top() < 3)
        margins.setTop(3);
    if (margins.bottom() < 3)
        margins.setBottom(3);
    if (margins.left() < 3)
        margins.setLeft(3);
    if (margins.right() < 3)
        margins.setRight(3);
    setContentsMargins(margins);
}

DragTool::~DragTool()
{
    delete d;
}

void DragTool::setTipWidth(int width)
{
    d->tipWidth = width;
    update();
}

void DragTool::setColor(const QColor &color)
{
    d->m_color = color;
    update();
}

QString DragTool::getTitle() const
{
    return d->title;
}

QString DragTool::getTip() const
{
    return d->tip;
}

QIcon DragTool::getIcon() const
{
    return d->icon;
}

QList<QVariant> DragTool::getDropValues() const
{
    return d->dropValue;
}

int DragTool::getTipWidth() const
{
    return d->tipWidth;
}

QColor DragTool::getColor() const
{
    return d->m_color;
}

void DragTool::setTitle(const QString& title)
{
    d->title = title;
    update();
}

void DragTool::setTip(const QString& tip)
{
    d->tip = tip;
    update();
}

void DragTool::setIcon(const QIcon &icon)
{
    d->icon = icon;
    update();
}

void DragTool::setDropValues(const QList<QVariant>& dropValue)
{
    d->dropValue = dropValue;
    update();
}

void DragTool::addDropValue(const QVariant& dropValue)
{
    d->dropValue << dropValue;
    update();
}

QSize DragTool::sizeHint() const
{
    //haojie.fang modify size
    int width = 0;
    int height = 0;
    if (d->iconSize.width() > width)
        width = d->iconSize.width();
    if (d->iconSize.height() > height)
        height = d->iconSize.height();
    QRect textRect = fontMetrics().boundingRect(QRect(), Qt::AlignLeft | Qt::TextSingleLine, d->title);
    width += textRect.width();
    if (textRect.height() > height)
        height = textRect.height();
    QMargins margins = contentsMargins();
    width += margins.left() + margins.right() + 1;
    height += margins.top() + margins.bottom() + 1;
    return QSize(width, height);
}

void DragTool::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    if(d->icon.isNull())
        d->icon = QIcon(":/utils/images/arrow.png");

    QMargins margins = contentsMargins();
    QPixmap pixmap = d->icon.pixmap(d->iconSize, isEnabled() ? QIcon::Normal : QIcon::Disabled, QIcon::Off);
    QPainter painter(this);

    //draw bakcground
    if(d->m_color.isValid())
    {
        painter.save();
        QRect rect(0, 0, width(), height());
        painter.setBrush(d->m_color);
        painter.setPen(Qt::NoPen);
        painter.drawRect(rect);
        painter.restore();
    }

    painter.drawPixmap(margins.left(),
                       (height() - static_cast<int>(pixmap.height() / pixmap.devicePixelRatio())) / 2,
                       pixmap);

    QRect textRect = painter.boundingRect(QRect(), Qt::AlignLeft | Qt::TextSingleLine, d->title);
    QRect boundingRect(static_cast<int>(margins.left() + pixmap.width() / pixmap.devicePixelRatio() + 2), (height() - textRect.height()) / 2, textRect.width(), textRect.height());
    painter.drawText(boundingRect, Qt::AlignHCenter | Qt::TextSingleLine, d->title);

    //
    //绘制提示信息,提示信息为空则不绘制
    QString tip = d->tip;
    if (!tip.isEmpty()) {
        //如果是数字则将超过999的数字显示成 999+
        //如果显示的提示信息长度过长则将多余显示成省略号 .
//        if (tip.toInt() > 0) {
//            tip = tip.toInt() > 999 ? "999+" : tip;
//        } else if (tip.length() > 9) {
//            tip = tip.left(2) + " .";
//        }

        //计算绘制区域,半径取高度的四分之一
        int radius = height() / 4;
        //QRect tipTextRect = painter.boundingRect(QRect(), Qt::AlignLeft | Qt::TextSingleLine, tip);
        QRect tipRect(static_cast<int>(width() - d->tipWidth - 5), 3 , d->tipWidth, height() - 6);

        //绘制提示文字的背景
        painter.setPen(Qt::NoPen);
        painter.setBrush(Qt::lightGray);
        painter.drawRoundedRect(tipRect, radius, radius);

        //设置字体大小
//        QFont font;
//        font.setPixelSize(painter.font().pixelSize() - 4);
//        painter.setFont(font);

        //绘制提示文字
        painter.setPen(Qt::SolidLine);
        painter.setBrush(Qt::NoBrush);
        painter.drawText(tipRect, Qt::AlignCenter, tip);
    }

    // draw a weak frame if mouse is inside widget
    if (!d->disableFrame && rect().contains(QWidget::mapFromGlobal(QCursor::pos()))) {
        QRect rect(0, 0, width() - 1, height() - 1);
        QPen pen = painter.pen();
        pen.setStyle(Qt::DashLine);
        pen.setColor(Qt::blue);
        painter.setPen(pen);
        painter.drawRect(rect);
        d->framePainted = true;
    } else {
        d->framePainted = false;
    }
}

void DragTool::enterEvent(QEvent *event)
{
    Q_UNUSED(event)
    update();
}

void DragTool::leaveEvent(QEvent *event)
{
    Q_UNUSED(event)
    update();
}

void DragTool::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        d->m_dragStartPostion = event->pos();
    }
    QWidget::mousePressEvent(event);
}

void DragTool::mouseMoveEvent(QMouseEvent *event)
{
    Q_UNUSED(event)
    const bool containsMouse = rect().contains(QWidget::mapFromGlobal(QCursor::pos()));
    if ((!containsMouse) || (containsMouse))
        update();

    if(!(event->buttons() & Qt::LeftButton))
           return;
    if((event->pos() - d->m_dragStartPostion).manhattanLength() < QApplication::startDragDistance())
        return;

    if(d->dropValue.isEmpty())
        return;

    auto drag = new QDrag(this);
    auto data = new Utils::DropMimeData;
    for(const auto& v : d->dropValue)
        data->addValue(v);
    drag->setMimeData(data);
    if(!d->icon.isNull())
    {
        QPixmap pixmap = d->icon.pixmap(QSize(48,48), QIcon::Normal, QIcon::Off);
        QPainter painter(&pixmap);
        painter.setCompositionMode(QPainter::CompositionMode_DestinationIn);
        painter.fillRect(pixmap.rect(), QColor(0, 0, 0, 96));
        drag->setPixmap(pixmap);
        drag->setHotSpot(QPoint(drag->pixmap().width() / 2, drag->pixmap().height() / 2));
    }

    update();
    Qt::DropAction action = drag->exec(Qt::MoveAction | Qt::CopyAction, Qt::MoveAction);
    update();
    emit dragEnd(action);
}

}
