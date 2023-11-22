#include "iconlinktext.h"

#include "widgetsstyle.h"
#include "widgetstheme.h"

#include <utils/theme/theme.h>

#include <QDesktopServices>
#include <QLabel>
#include <QMouseEvent>
#include <QPainter>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QPixmap>
#include <QIcon>

namespace EmosWidgets
{

class IconLinkTextPrivate
{
public:
    QIcon m_iconSource;
    QString m_title;
    QString m_openUrl;

    QLabel *m_icon;
    QLabel *m_label;
    const int lrPadding = 34;

    QFont sizedFont(int size, const QWidget *widget, bool underline = false)
    {
        QFont f = widget->font();
        f.setPixelSize(size);
        f.setUnderline(underline);
        return f;
    }
    QColor themeColor(Utils::Theme::Color role)
    {
        return Utils::creatorTheme()->color(role);
    }
};

IconLinkText::IconLinkText(const QIcon &iconSource, const QString &title, const QString &openUrl, QWidget *parent)
    : QWidget(parent),
      d(new IconLinkTextPrivate)
{
    d->m_iconSource = iconSource;
    d->m_title = title;
    d->m_openUrl = openUrl;

    setAutoFillBackground(true);
    setMinimumHeight(30);
    setToolTip(d->m_openUrl);

    d->m_icon = new QLabel;
    d->m_icon->setPixmap(iconSource.pixmap(QSize(16,16)));

    d->m_label = new QLabel(title);
    d->m_label->setFont(d->sizedFont(11, d->m_label, false));

    auto layout = new QHBoxLayout;
    layout->setContentsMargins(d->lrPadding, 0, d->lrPadding, 0);
    layout->addWidget(d->m_icon);
    layout->addSpacing(2);
    layout->addWidget(d->m_label);
    layout->addStretch(1);
    setLayout(layout);
}

IconLinkText::~IconLinkText()
{
    delete d;
}

void IconLinkText::enterEvent(QEvent *)
{
    QPalette pal;
    pal.setColor(QPalette::Window, d->themeColor(Utils::Theme::Welcome_HoverColor));
    setPalette(pal);
    d->m_label->setFont(d->sizedFont(11, d->m_label, true));
    update();
}

void IconLinkText::leaveEvent(QEvent *)
{
    QPalette pal;
    pal.setColor(QPalette::Window, d->themeColor(Utils::Theme::Welcome_BackgroundColor));
    setPalette(pal);
    d->m_label->setFont(d->sizedFont(11, d->m_label, false));
    update();
}

void IconLinkText::mousePressEvent(QMouseEvent *)
{
    QDesktopServices::openUrl(d->m_openUrl);
}

}
