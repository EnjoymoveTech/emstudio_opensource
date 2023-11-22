#ifndef DRAGBUTTON_H
#define DRAGBUTTON_H

#include <utils/utils_global.h>
#include <QToolButton>
#include <QVariant>

namespace EmosWidgets {

class EMSTUDIO_UTILS_EXPORT DragButton : public QToolButton
{
    Q_OBJECT
public:
    explicit DragButton(QWidget *parent = nullptr);

    void setDropValue(const QVariant& value);
protected:
    void enterEvent(QEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
signals:

private:
    QPoint m_dragStartPostion;
    QVariant m_dropValue;
};
}
#endif // DRAGBUTTON_H
