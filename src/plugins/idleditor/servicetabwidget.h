#ifndef SERVICETABWIDGET_H
#define SERVICETABWIDGET_H
#include <QTabWidget>
namespace Idl {
namespace Internal {
class IdlEditor;
class ServiceDetailWidget;
class ServiceTabWidget : public QTabWidget
{
    Q_OBJECT
public:
    ServiceTabWidget(IdlEditor *editor,QWidget *parent = nullptr);
    ~ServiceTabWidget();
    void loadFromDoc();
public slots:
    void onWidgetChange();
private:
    IdlEditor          * m_editor = nullptr;
    ServiceDetailWidget* m_methodWidget = nullptr;
    ServiceDetailWidget* m_eventWidget = nullptr;
    ServiceDetailWidget* m_fieldWidget = nullptr;
    ServiceDetailWidget* m_pinWidget = nullptr;
};

}
}
#endif // SERVICETABWIDGET_H
