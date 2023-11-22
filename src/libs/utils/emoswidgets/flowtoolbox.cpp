#include "flowtoolbox.h"

#include "widgetsstyle.h"
#include "widgetstheme.h"

#include <utils/utilsicons.h>
#include <utils/flowlayout.h>
#include <utils/dropsupport.h>
#include <utils/emoswidgets/dragtool.h>

#include <QEvent>
#include <QLabel>
#include <QToolBar>
#include <QToolButton>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QMouseEvent>

namespace EmosWidgets
{

class FlowGroupWidgetPrivate
{
public:
    QToolBar* m_toolbar = nullptr;
    QLabel *m_title = nullptr;
    QToolButton *m_closeButton = nullptr;
    QWidget *m_content = nullptr;
    QList<QWidget *> m_items;
    Utils::DropSupport* m_dropUtils = nullptr;
};

class DropFlowGroupWidgetPrivate
{
public:
    int m_tipWidth = 70;
    bool m_deleteSupport = false;
    bool m_dragDeleteSupport = false;
    bool m_allowSameItem = false;
    QIcon m_icon = QIcon();
};

class FlowToolBoxPrivate
{
public:
    QList<QWidget*> m_widgets;
    QVBoxLayout* m_shapeGrouplayout;
};

class DropFlowToolBoxPrivate
{
public:

};

FlowGroupWidget::FlowGroupWidget(const QString& title, const dropWidgetFunction &dropFunction, QLayout* groupLayout, QWidget *parent):
    QWidget(parent),
    m_dropFunction(dropFunction),
    d(new FlowGroupWidgetPrivate)
{
    d->m_title = new QLabel(title);
    d->m_title->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);

    d->m_closeButton = new QToolButton;
    d->m_closeButton->setIcon(Utils::Icons::COLLAPSE_TOOLBAR.icon());

    d->m_toolbar = new QToolBar;
    d->m_toolbar->addWidget(d->m_title);
    d->m_toolbar->addWidget(d->m_closeButton);

    d->m_content = new QWidget;
    d->m_content->setLayout(groupLayout ? groupLayout : new Utils::FlowLayout());

    setLayout(new QVBoxLayout);
    layout()->setContentsMargins(0, 0, 0, 0);
    layout()->setSpacing(0);
    layout()->addWidget(d->m_toolbar);
    layout()->addWidget(d->m_content);

    connect(d->m_closeButton, &QToolButton::clicked, this, [this]() {
        d->m_content->setVisible(!d->m_content->isVisible());
        d->m_closeButton->setIcon(d->m_content->isVisible()
                               ? Utils::Icons::COLLAPSE_TOOLBAR.icon()
                               : Utils::Icons::EXPAND_TOOLBAR.icon());
    });

    if(m_dropFunction)
        supportDrop();
}

FlowGroupWidget::~FlowGroupWidget()
{

}

void FlowGroupWidget::appendItem(QWidget *widget)
{
    d->m_content->layout()->addWidget(widget);
    d->m_items << widget;
}

void FlowGroupWidget::removeItem(QWidget *widget)
{
    d->m_items.removeOne(widget);
    d->m_content->layout()->removeWidget(widget);
    widget->disconnect();
    widget->setParent(NULL);
    widget->deleteLater();
}

void FlowGroupWidget::clearItem()
{
    QLayoutItem *child;
    while ((child = d->m_content->layout()->takeAt(0)) != 0)
    {
        if(child->widget())
        {
            child->widget()->disconnect();
            child->widget()->setParent(NULL);
            delete child->widget();
        }
        delete child;
    }
    d->m_items.clear();
}

void FlowGroupWidget::setCollapsed(bool collapse)
{
    d->m_content->setVisible(!collapse);
    d->m_closeButton->setIcon(!collapse
                           ? Utils::Icons::COLLAPSE_TOOLBAR.icon()
                           : Utils::Icons::EXPAND_TOOLBAR.icon());
}

void FlowGroupWidget::setEnableCollapsed(bool enable)
{
    d->m_toolbar->actions().at(1)->setVisible(enable);
}

QList<QWidget *> FlowGroupWidget::items() const
{
    return d->m_items;
}

QString FlowGroupWidget::title() const
{
    return d->m_title->text();
}

QLayout *FlowGroupWidget::groupLayout() const
{
    return d->m_content->layout();
}

void FlowGroupWidget::valuesDropped(const QList<QVariant> &values, const QPoint &dropPos)
{
    if(!m_dropFunction)
        return;

    QWidget* w = m_dropFunction(values);
    if(!w)
        return;

    appendItem(w);

    emit droppedEnd(w);
}

void FlowGroupWidget::supportDrop()
{
    d->m_dropUtils = new Utils::DropSupport(
                this,
                [](QDropEvent *event, Utils::DropSupport *dropSupport)
            -> bool {
            return dropSupport->isValueDrop(event);
    });

    connect(d->m_dropUtils, &Utils::DropSupport::valuesDropped, this, &FlowGroupWidget::valuesDropped);
}

//////////////////////////////////////////

DropFlowGroupWidget::DropFlowGroupWidget(const QString &title, bool dropSupport, QLayout *groupLayout, QWidget *parent) :
    FlowGroupWidget(title, dropSupport?std::bind(&DropFlowGroupWidget::addDrop, this, std::placeholders::_1):dropWidgetFunction() , groupLayout, parent),
    d(new DropFlowGroupWidgetPrivate)
{
    connect(this, &DropFlowGroupWidget::droppedEnd, this, &DropFlowGroupWidget::slotDroppedEnd);
}

DropFlowGroupWidget::~DropFlowGroupWidget()
{

}

void DropFlowGroupWidget::setAllowSameItem()
{
    d->m_allowSameItem = true;
}

void DropFlowGroupWidget::setDeleteSupport()
{
    d->m_deleteSupport = true;
}

void DropFlowGroupWidget::setDragDeleteSupport()
{
    d->m_dragDeleteSupport = true;
}

void DropFlowGroupWidget::setTipWidth(int width)
{
    d->m_tipWidth = width;
}

void DropFlowGroupWidget::setIcon(const QIcon &icon)
{
    d->m_icon = icon;
}

void DropFlowGroupWidget::appendItem(const QString& item, const QString& tip)
{
    if(!d->m_allowSameItem && hasSameItem(item))
        return;

    DragTool* tool = new DragTool(item, this);
    tool->setMaximumHeight(30);
    tool->setMinimumHeight(25);
    tool->setTitle(item);
    tool->setTip(tip);
    if(!d->m_icon.isNull())
        tool->setIcon(d->m_icon);
    tool->addDropValue(item);
    tool->addDropValue(tip);
    tool->setTipWidth(d->m_tipWidth);
    tool->installEventFilter(this);

    if(d->m_dragDeleteSupport)
        connect(tool, &DragTool::dragEnd, this, &DropFlowGroupWidget::dragEnd);

    FlowGroupWidget::appendItem(tool);

    emit itemAppended(item);
}

void DropFlowGroupWidget::removeItem(const QString &item)
{
    for(QWidget* w : items())
    {
        if(dynamic_cast<DragTool*>(w))
        {
            if(item == dynamic_cast<DragTool*>(w)->getTitle())
            {
                emit itemBeforeRemoved(item);
                FlowGroupWidget::removeItem(w);
                emit itemRemoved(item);
                return;
            }
        }
    }
}

void DropFlowGroupWidget::clearItem()
{
    FlowGroupWidget::clearItem();
}

QStringList DropFlowGroupWidget::itemNames() const
{
    QStringList names;
    for(QWidget* w : items())
    {
        if(dynamic_cast<DragTool*>(w))
        {
            names << dynamic_cast<DragTool*>(w)->getTitle();
        }
    }
    return names;
}

bool DropFlowGroupWidget::eventFilter(QObject *object, QEvent *event)
{
/*    if(event->type() == QEvent::KeyPress)
    {
        if(d->m_deleteSupport && ((QKeyEvent*)event)->key() == Qt::Key_Delete)
        {
            if(dynamic_cast<DragTool*>(object))
            {
                emit itemRemoved(dynamic_cast<DragTool*>(object)->getTitle());
                FlowGroupWidget::removeItem(dynamic_cast<DragTool*>(object));
            }
        }
    }
    else */if(event->type() == QEvent::MouseButtonPress)
    {
        if(d->m_deleteSupport && ((QMouseEvent*)event)->buttons() & Qt::RightButton)
        {
            if(dynamic_cast<DragTool*>(object))
            {
                QString title = dynamic_cast<DragTool*>(object)->getTitle();
                emit itemBeforeRemoved(title);
                FlowGroupWidget::removeItem(dynamic_cast<DragTool*>(object));
                emit itemRemoved(title);
            }
        }
    }

    return QWidget::eventFilter(object, event);
}

QWidget *DropFlowGroupWidget::addDrop(const QList<QVariant> &dropValue)
{
    if(!d->m_allowSameItem && hasSameItem(dropValue.first().toString()))
        return nullptr;

    emit itemBeforeDroped(dropValue.first().toString());

    DragTool* tool = new DragTool(dropValue.first().toString(), this);
    tool->setMaximumHeight(30);
    tool->setMinimumHeight(25);
    tool->setTitle(dropValue.first().toString());
    tool->setTip(dropValue[1].toString());
    if(!d->m_icon.isNull())
        tool->setIcon(d->m_icon);
    tool->setDropValues(dropValue);
    tool->setTipWidth(d->m_tipWidth);
    tool->installEventFilter(this);

    if(d->m_dragDeleteSupport)
        connect(tool, &DragTool::dragEnd, this, &DropFlowGroupWidget::dragEnd);

    return tool;
}

bool DropFlowGroupWidget::hasSameItem(const QString &item)
{
    for(QWidget* w : items())
    {
        if(dynamic_cast<DragTool*>(w))
        {
            if(item == dynamic_cast<DragTool*>(w)->getTitle())
            {
                return true;
            }
        }
    }
    return false;
}

void DropFlowGroupWidget::dragEnd(Qt::DropAction action)
{
    DragTool* tool = qobject_cast<DragTool*>(sender());
    if(tool)
    {
        QString title = tool->getTitle();
        emit itemBeforeRemoved(title);
        FlowGroupWidget::removeItem(tool);
        emit itemRemoved(title);
    }
}

void DropFlowGroupWidget::slotDroppedEnd(QWidget *widget)
{
    if(dynamic_cast<DragTool*>(widget))
    {
        emit itemDroped(dynamic_cast<DragTool*>(widget)->getTitle());
    }
}

//////////////////////////////////////////

FlowToolBox::FlowToolBox(QWidget* parent) :
    QFrame(parent),
    d(new FlowToolBoxPrivate)
{
    setFrameShape(QFrame::NoFrame);
    setFrameShadow(QFrame::Plain);
    setLineWidth(1);
    setMidLineWidth(0);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);

    QScrollArea* area = new QScrollArea(this);
    QWidget* widget = new QWidget(this);
    QVBoxLayout* widgetLayout = new QVBoxLayout(widget);
    widgetLayout->setSpacing(0);
    widgetLayout->setContentsMargins(0, 0, 0, 0);
    area->setFrameShape(QFrame::NoFrame);
    area->setFrameShadow(QFrame::Plain);
    area->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    area->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    area->setSizeAdjustPolicy(QScrollArea::AdjustIgnored);
    area->setWidgetResizable(true);
    area->setWidget(widget);

    d->m_shapeGrouplayout = new QVBoxLayout();
    d->m_shapeGrouplayout->setSpacing(0);
    widgetLayout->addLayout(d->m_shapeGrouplayout);

    QSpacerItem *spacerItem = new QSpacerItem(20, 40, QSizePolicy::Maximum, QSizePolicy::Expanding);
    widgetLayout->addItem(spacerItem);

    layout->addWidget(area);
}

FlowToolBox::~FlowToolBox()
{

}

void FlowToolBox::appendGroup(QWidget *group)
{
    d->m_widgets << group;
    d->m_shapeGrouplayout->addWidget(group);
    update();
}

void FlowToolBox::clearGroup()
{
    QLayoutItem *child;
    while ((child = d->m_shapeGrouplayout->takeAt(0)) != 0)
    {
        if(child->widget())
        {
            child->widget()->disconnect();
            child->widget()->setParent(NULL);
            delete child->widget();
        }
        delete child;
    }
    d->m_widgets.clear();
}

void FlowToolBox::removeGroup(QWidget *group)
{
    d->m_widgets.removeOne(group);
    d->m_shapeGrouplayout->layout()->removeWidget(group);
    group->disconnect();
    group->setParent(NULL);
    group->deleteLater();
}

QList<QWidget *> FlowToolBox::groups() const
{
    return d->m_widgets;
}

DropFlowToolBox::DropFlowToolBox(QWidget* parent) :
    FlowToolBox(parent),
    d(new DropFlowToolBoxPrivate)
{
    auto dropUtils = new Utils::DropSupport(
                this,
                [](QDropEvent *event, Utils::DropSupport *dropSupport)
            -> bool {
            return dropSupport->isValueDrop(event);
    });

    connect(dropUtils, &Utils::DropSupport::valuesDropped, [&](const QList<QVariant> &values, const QPoint &dropPos){
        if(groups().size() == 0)
            return;

        DropFlowGroupWidget* group = dynamic_cast<DropFlowGroupWidget*>(groups().last());
        if(group)
        {
            QWidget* w = group->addDrop(values);
            if(!w)
                return;

            (qobject_cast<FlowGroupWidget*>(group))->appendItem(w);
            emit group->droppedEnd(w);
        }
    });
}

DropFlowToolBox::~DropFlowToolBox()
{

}

}
