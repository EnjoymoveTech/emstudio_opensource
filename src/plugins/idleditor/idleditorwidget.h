#ifndef IDLEDITORWIDGET_H
#define IDLEDITORWIDGET_H
#include <utils/fancymainwindow.h>
#include <QDomDocument>
using namespace Utils;

namespace Idl {
namespace Internal {
class IdlEditor;
class PropertyWidget;
class ThreadWidget;
class ServiceWidget;

class IdlEditorWidget : public Utils::FancyMainWindow
{
    Q_OBJECT
public:
    IdlEditorWidget(IdlEditor* editor,QWidget *parent = nullptr);
    ~IdlEditorWidget();

    void exportToDoc();
    void loadFromDoc();

    PropertyWidget* getPropertyWidget();
    ThreadWidget*   getThreadWidget();
    ServiceWidget*  getServiceWidget();
public slots:
    void onappendEmptyRow();
    void onremoveOneRow();
private:
    IdlEditor*      m_editor = nullptr;
    PropertyWidget* m_propertyWidget = nullptr;
    ThreadWidget*   m_threadWidget = nullptr;
    ServiceWidget*  m_serviceWidget = nullptr;
    QTabWidget*     m_tabwidget = nullptr;
};

}
} // namespace Idl

#endif
