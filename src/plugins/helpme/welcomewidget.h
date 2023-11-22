#ifndef WELCOMEWIDGET_H
#define WELCOMEWIDGET_H

#include <QWidget>
#include "bugreportwidget.h"

namespace Helpme {
namespace Internal {

class SideBar;
class DocWidget;
class WelcomeWidget : public QWidget
{
    Q_OBJECT
public:
    explicit WelcomeWidget(QWidget *parent = nullptr);
    ~WelcomeWidget();

    void inits();
signals:

private:
    SideBar *m_sideBar;
    DocWidget* m_docWidget;
};
}}
#endif // WELCOMEWIDGET_H
