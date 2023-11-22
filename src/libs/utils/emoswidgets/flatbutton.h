#ifndef FLATBUTTON_H
#define FLATBUTTON_H

#include <utils/utils_global.h>

#include <QPushButton>

namespace EmosWidgets
{

class FlatButtonPrivate;
class EMSTUDIO_UTILS_EXPORT FlatButton : public QPushButton
{
public:
    FlatButton(QWidget* parent = nullptr);
    FlatButton(const QString& text, QWidget* parent = nullptr);
    ~FlatButton();

    void setTransparent(bool trans);

    void setTextColor(const QColor &color);
    QColor textColor() const;

    void setBackgroundColor(const QColor &color);
    QColor backgroundColor() const;

    void setHoverTextColor(const QColor &color);
    QColor hoverTextColor() const;

    void setHoverBackgroundColor(const QColor &color);
    QColor hoverBackgroundColor() const;

    void setPressTextColor(const QColor &color);
    QColor pressTextColor() const;

    void setPressBackgroundColor(const QColor &color);
    QColor pressBackgroundColor() const;

    void setTextAlignment(Qt::Alignment alignment);
    Qt::Alignment textAlignment() const;

private:
    QScopedPointer<FlatButtonPrivate> d;

    void setButtonStyle();
};

}
#endif // FLATBUTTON_H
