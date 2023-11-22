#ifndef FLOWTOOLBOX_H
#define FLOWTOOLBOX_H

#include <utils/utils_global.h>
#include <utils/emoswidgets/overlaywidget.h>

#include <QFrame>
#include <QVBoxLayout>
#include <QIcon>

class QLabel;
class QToolButton;
namespace EmosWidgets
{
class DropFlowToolBox;
class FlowToolBoxPrivate;
class DropFlowToolBoxPrivate;
class FlowGroupWidgetPrivate;
class DropFlowGroupWidgetPrivate;

class EMSTUDIO_UTILS_EXPORT FlowGroupWidget : public QWidget
{
    Q_OBJECT

public:
    using dropWidgetFunction = std::function<QWidget*(const QList<QVariant>& dropValue)>;

    explicit FlowGroupWidget(const QString& title, const dropWidgetFunction &dropFunction = dropWidgetFunction(),
                             QLayout* groupLayout = nullptr, QWidget *parent = nullptr);
    ~FlowGroupWidget();

    void appendItem(QWidget* widget);
    void removeItem(QWidget* widget);
    void clearItem();
    void setCollapsed(bool collapse);
    void setEnableCollapsed(bool enable);

    QList<QWidget*> items() const;
    QString title() const;
    QLayout* groupLayout() const;

signals:
    void droppedEnd(QWidget* widget);
private slots:
    void valuesDropped(const QList<QVariant> &values, const QPoint &dropPos);

private:
    void supportDrop();
    dropWidgetFunction m_dropFunction;
    QScopedPointer<FlowGroupWidgetPrivate> d;
};

class EMSTUDIO_UTILS_EXPORT DropFlowGroupWidget : public FlowGroupWidget
{
    Q_OBJECT

public:
    explicit DropFlowGroupWidget(const QString& title, bool dropSupport = true, QLayout* groupLayout = new QVBoxLayout(), QWidget *parent = nullptr);
    ~DropFlowGroupWidget();

    void setAllowSameItem();//允许相同名字加入
    void setDeleteSupport();//支持右键删除
    void setDragDeleteSupport();//支持拖拽删除
    void setTipWidth(int width);
    void setIcon(const QIcon& icon);

    //先设置上面的特性再进行项目添加
    void appendItem(const QString& item, const QString& tip = "");
    void removeItem(const QString& item);
    void clearItem();

    QStringList itemNames() const;
signals:
    void itemBeforeDroped(const QString& item);
    void itemBeforeRemoved(const QString& item);
    void itemDroped(const QString& item);
    void itemRemoved(const QString& item);
    void itemAppended(const QString& item);

protected:
    bool eventFilter(QObject *object, QEvent * event);
private:
    friend DropFlowToolBox;
    QWidget* addDrop(const QList<QVariant>& dropValue);
    bool hasSameItem(const QString& item);
    void dragEnd(Qt::DropAction action);
    void slotDroppedEnd(QWidget* widget);
    QScopedPointer<DropFlowGroupWidgetPrivate> d;
};

class EMSTUDIO_UTILS_EXPORT FlowToolBox : public QFrame
{
    Q_OBJECT
public:
    FlowToolBox(QWidget* parent = nullptr);
    ~FlowToolBox();

    void appendGroup(QWidget* group);
    void clearGroup();
    void removeGroup(QWidget* group);
    QList<QWidget*> groups() const;

private:
    QScopedPointer<FlowToolBoxPrivate> d;
};

class EMSTUDIO_UTILS_EXPORT DropFlowToolBox : public FlowToolBox
{
    Q_OBJECT
public:
    DropFlowToolBox(QWidget* parent = nullptr);
    ~DropFlowToolBox();

private:
    QScopedPointer<DropFlowToolBoxPrivate> d;
};

}

#endif // FLATTABLE_H
