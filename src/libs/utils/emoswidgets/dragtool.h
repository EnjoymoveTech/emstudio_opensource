#ifndef DRAGTOOL_H
#define DRAGTOOL_H

#include <QIcon>
#include <QWidget>
#include <utils/utils_global.h>

namespace EmosWidgets {
class DragToolPrivate;
class EMSTUDIO_UTILS_EXPORT DragTool : public QWidget
{
    Q_OBJECT
public:
    explicit DragTool(const QString &title, QWidget *parent = nullptr);
    ~DragTool();

    void setTitle(const QString& title);
    void setTip(const QString& tip);
    void setIcon(const QIcon& icon);
    void addDropValue(const QVariant& dropValue);
    void setDropValues(const QList<QVariant>& dropValue);
    void setTipWidth(int width);
    void setColor(const QColor& color);

    QString getTitle() const;
    QString getTip() const;
    QIcon getIcon() const;
    QList<QVariant> getDropValues() const;
    int getTipWidth() const;
    QColor getColor() const;
signals:
    void dragEnd(Qt::DropAction action);

protected:
    QSize sizeHint() const override;
    void paintEvent(QPaintEvent *event) override;
    void enterEvent(QEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
private:
    DragToolPrivate *d;
};

}

#endif // DRAGTOOL_H
