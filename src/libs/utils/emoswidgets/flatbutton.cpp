#include "flatbutton.h"

#include "widgetsstyle.h"
#include "widgetstheme.h"


namespace EmosWidgets
{

class FlatButtonPrivate
{
public:
    enum {
        IconPadding = 12
    };

    int cornerRadius = 3;
    int padding = 8;
    int textSize = 15;
    bool transparent = false;
    Qt::Alignment textAlignment = Qt::AlignHCenter;
    QColor backgroundColor;
    QColor textColor;
    QColor hoverBackgroundColor;
    QColor hoverTextColor;
    QColor pressBackgroundColor;
    QColor pressTextColor;
    QColor disableBackgroundColor;
    QColor disableTextColor;
};

FlatButton::FlatButton(QWidget* parent) :
    QPushButton(parent),
    d(new FlatButtonPrivate)
{
    QColor primary1 = WidgetsStyle::instance().themeColor(DefC_Primary1);
    QColor primary2 = WidgetsStyle::instance().themeColor(DefC_Primary2);
    QColor primary3 = WidgetsStyle::instance().themeColor(DefC_Primary3);
    QColor primary4 = WidgetsStyle::instance().themeColor(DefC_Primary4);
    QColor accent1 = WidgetsStyle::instance().themeColor(DefC_Accent1);
    QColor accent2 = WidgetsStyle::instance().themeColor(DefC_Accent2);
    QColor accent3 = WidgetsStyle::instance().themeColor(DefC_Accent3);
    QColor textcolor = WidgetsStyle::instance().themeColor(DefC_Text);
    QColor alternateText = WidgetsStyle::instance().themeColor(DefC_AlternateText);
    QColor canvas = WidgetsStyle::instance().themeColor(DefC_Canvas);
    QColor border = WidgetsStyle::instance().themeColor(DefC_Border);
    QColor disabled = WidgetsStyle::instance().themeColor(DefC_Disabled);
    QColor disabled2 = WidgetsStyle::instance().themeColor(DefC_Disabled2);
    QColor disabled3 = WidgetsStyle::instance().themeColor(DefC_Disabled3);

    d->backgroundColor = primary1;
    d->textColor = textcolor;

    accent1.setAlpha(180);
    d->hoverBackgroundColor = accent1;
    d->hoverTextColor = alternateText;

    accent1.setAlpha(255);
    d->pressBackgroundColor = accent1;
    d->pressTextColor = alternateText;

    d->disableBackgroundColor = disabled;
    textcolor.setAlpha(50);
    d->disableTextColor = textcolor;

    QFont font("Roboto");
    font.setPixelSize(d->textSize);
    setFont(font);

    setButtonStyle();
}

FlatButton::FlatButton(const QString& text, QWidget* parent) :
    QPushButton(text, parent),
    d(new FlatButtonPrivate)
{
    QColor primary1 = WidgetsStyle::instance().themeColor(DefC_Primary1);
    QColor primary2 = WidgetsStyle::instance().themeColor(DefC_Primary2);
    QColor primary3 = WidgetsStyle::instance().themeColor(DefC_Primary3);
    QColor primary4 = WidgetsStyle::instance().themeColor(DefC_Primary4);
    QColor accent1 = WidgetsStyle::instance().themeColor(DefC_Accent1);
    QColor accent2 = WidgetsStyle::instance().themeColor(DefC_Accent2);
    QColor accent3 = WidgetsStyle::instance().themeColor(DefC_Accent3);
    QColor textcolor = WidgetsStyle::instance().themeColor(DefC_Text);
    QColor alternateText = WidgetsStyle::instance().themeColor(DefC_AlternateText);
    QColor canvas = WidgetsStyle::instance().themeColor(DefC_Canvas);
    QColor border = WidgetsStyle::instance().themeColor(DefC_Border);
    QColor disabled = WidgetsStyle::instance().themeColor(DefC_Disabled);
    QColor disabled2 = WidgetsStyle::instance().themeColor(DefC_Disabled2);
    QColor disabled3 = WidgetsStyle::instance().themeColor(DefC_Disabled3);

    d->backgroundColor = primary1;
    d->textColor = text;

    accent1.setAlpha(180);
    d->hoverBackgroundColor = accent1;
    d->hoverTextColor = alternateText;

    accent1.setAlpha(255);
    d->pressBackgroundColor = accent1;
    d->pressTextColor = alternateText;

    d->disableBackgroundColor = disabled;
    textcolor.setAlpha(50);
    d->disableTextColor = textcolor;

    QFont font("Roboto");
    font.setPixelSize(d->textSize);
    setFont(font);

    setButtonStyle();
}

FlatButton::~FlatButton()
{

}

void FlatButton::setTransparent(bool trans)
{
    d->transparent = trans;
    setButtonStyle();
    update();
}

void FlatButton::setTextColor(const QColor &color)
{
    d->textColor = color;
    setButtonStyle();
    update();
}

QColor FlatButton::textColor() const
{
    return d->textColor;
}

void FlatButton::setBackgroundColor(const QColor &color)
{
    d->backgroundColor = color;
    setButtonStyle();
    update();
}

QColor FlatButton::backgroundColor() const
{
    return d->backgroundColor;
}

void FlatButton::setHoverTextColor(const QColor &color)
{
    d->hoverTextColor = color;
    setButtonStyle();
    update();
}

QColor FlatButton::hoverTextColor() const
{
    return d->hoverTextColor;
}

void FlatButton::setHoverBackgroundColor(const QColor &color)
{
    d->hoverBackgroundColor = color;
    setButtonStyle();
    update();
}

QColor FlatButton::hoverBackgroundColor() const
{
    return d->hoverBackgroundColor;
}

void FlatButton::setPressTextColor(const QColor &color)
{
    d->pressTextColor = color;
    setButtonStyle();
    update();
}

QColor FlatButton::pressTextColor() const
{
    return d->pressTextColor;
}

void FlatButton::setPressBackgroundColor(const QColor &color)
{
    d->pressBackgroundColor = color;
    setButtonStyle();
    update();
}

QColor FlatButton::pressBackgroundColor() const
{
    return d->pressBackgroundColor;
}

void FlatButton::setTextAlignment(Qt::Alignment alignment)
{
    d->textAlignment = alignment;
    setButtonStyle();
    update();
}

Qt::Alignment FlatButton::textAlignment() const
{
    return d->textAlignment;
}

void FlatButton::setButtonStyle()
{
    QStringList list;

    QString textAlign = "center";
    switch(textAlignment())
    {
    case Qt::AlignLeft:
        textAlign = "left";
        break;
    case Qt::AlignRight:
        textAlign = "right";
        break;
    default:
        break;
    }

    list.append(QString(R"(
                        QPushButton
                        {
                            border-style:none;
                            padding:%1px;
                            border-radius:%2px;
                            color:%3;
                            background-color:%4;
                            text-align:%5;
                        }
                        )").arg(d->padding).arg(d->cornerRadius).arg(WidgetsTheme::converARGB16Hex(textColor())).arg(d->transparent? "#00000000" : WidgetsTheme::converARGB16Hex(backgroundColor())).arg(textAlign));
    list.append(QString(R"(
                        QPushButton:hover
                        {
                            color:%1;
                            background-color:%2;
                        }
                        )").arg(WidgetsTheme::converARGB16Hex(hoverTextColor())).arg(WidgetsTheme::converARGB16Hex(hoverBackgroundColor())));
    list.append(QString(R"(
                        QPushButton:pressed,QPushButton:checked
                        {
                            color:%1;
                            background-color:%2;
                        }
                        )").arg(WidgetsTheme::converARGB16Hex(pressTextColor())).arg(WidgetsTheme::converARGB16Hex(pressBackgroundColor())));
    list.append(QString(R"(
                        QPushButton:disabled
                        {
                            color:%1;
                            background-color:%2;
                        }
                        )").arg(WidgetsTheme::converARGB16Hex(d->disableTextColor)).arg(WidgetsTheme::converARGB16Hex(d->disableBackgroundColor)));

    QString qss = list.join("");
    this->setStyleSheet(qss);
}


}
