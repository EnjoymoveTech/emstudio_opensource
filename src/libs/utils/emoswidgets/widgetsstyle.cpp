#include "widgetsstyle.h"
#include <QFontDatabase>
#include "widgetstheme.h"

namespace EmosWidgets
{

class WidgetsStylePrivate
{
    Q_DISABLE_COPY(WidgetsStylePrivate)
    Q_DECLARE_PUBLIC(WidgetsStyle)

public:
    WidgetsStylePrivate(WidgetsStyle *q);
    ~WidgetsStylePrivate();

    void init();

    WidgetsStyle *const q_ptr;
    WidgetsTheme *theme;
};

WidgetsStylePrivate::WidgetsStylePrivate(WidgetsStyle *q)
    : q_ptr(q)
{
}

WidgetsStylePrivate::~WidgetsStylePrivate()
{
}

void WidgetsStylePrivate::init()
{
    Q_Q(WidgetsStyle);

    QFontDatabase::addApplicationFont(":/fonts/roboto_regular");
    QFontDatabase::addApplicationFont(":/fonts/roboto_medium");
    QFontDatabase::addApplicationFont(":/fonts/roboto_bold");

    q->setTheme(new WidgetsTheme);
}

WidgetsStyle::WidgetsStyle()
    : QCommonStyle(),
      d_ptr(new WidgetsStylePrivate(this))
{
    d_func()->init();
}

WidgetsStyle::~WidgetsStyle()
{

}

WidgetsStyle &WidgetsStyle::instance()
{
    static WidgetsStyle m_instance;
    return m_instance;
}

void WidgetsStyle::setTheme(WidgetsTheme *theme)
{
    Q_D(WidgetsStyle);

    d->theme = theme;
    theme->setParent(this);
}

QColor WidgetsStyle::themeColor(const QString &key) const
{
    Q_D(const WidgetsStyle);

    Q_ASSERT(d->theme);

    return d->theme->getColor(key);
}

QColor WidgetsStyle::themeColor(MaterialColor color) const
{
    Q_D(const WidgetsStyle);

    Q_ASSERT(d->theme);

    return d->theme->getColor(color);
}
}
