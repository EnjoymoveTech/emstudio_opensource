#ifndef SERVICEDETAILWIDGET_H
#define SERVICEDETAILWIDGET_H
#include <utils/emoswidgets/flattable.h>
namespace Idl {
namespace Internal {
class IdlEditor;

enum INTERFACETYPE{
    MethodType = 0,
    EventType = 1,
    FieldType = 2,
    PinType = 3
};

class ServiceDetailWidget : public EmosWidgets::FlatTable
{
    Q_OBJECT
public:
    ServiceDetailWidget(IdlEditor* IdlEditor, INTERFACETYPE kind, QWidget* parent = nullptr);
    void loadFromApi();
private:
    void restoreControl();
    IdlEditor*      m_editor;
    INTERFACETYPE   m_INTERFACETYPE;
};

}
}
#endif // SERVICEDETAILWIDGET_H
