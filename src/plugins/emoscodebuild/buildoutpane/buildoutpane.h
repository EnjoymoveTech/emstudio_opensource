#ifndef BUILDOUTPANE_H
#define BUILDOUTPANE_H

#include <coreplugin/dialogs/ioptionspage.h>
#include <coreplugin/ioutputpane.h>
#include <coreplugin/outputwindow.h>
#include <coreplugin/messageoutputwindow.h>

namespace EmosCodeBuild {
namespace Internal {


class BuildoutPane : public Core::Internal::MessageOutputWindow
{
    Q_OBJECT
public:
    explicit BuildoutPane(QObject *parent = nullptr);
    //~BuildoutPane();
    QString displayName() const;
    int priorityInStatusBar() const;
};
}
}

#endif // BUILDOUTPANE_H
