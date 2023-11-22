#ifndef ICONLINKTEXT_H
#define ICONLINKTEXT_H

#include <utils/utils_global.h>

#include <QWidget>

namespace EmosWidgets
{

class IconLinkTextPrivate;
class EMSTUDIO_UTILS_EXPORT IconLinkText : public QWidget
{
public:
    IconLinkText(const QIcon &iconSource, const QString &title, const QString &openUrl, QWidget *parent = nullptr);
    ~IconLinkText();

protected:
    void enterEvent(QEvent *) override;
    void leaveEvent(QEvent *) override;
    void mousePressEvent(QMouseEvent *) override;

private:
    IconLinkTextPrivate* d = nullptr;
};

}

#endif // FLATTABLE_H
