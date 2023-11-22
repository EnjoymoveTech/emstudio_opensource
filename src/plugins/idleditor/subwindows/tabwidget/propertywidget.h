#ifndef PROPERTYWIDGET_H
#define PROPERTYWIDGET_H
#include <QComboBox>
#include <QDomDocument>
#include <utils/emoswidgets/messagetips.h>
#include <utils/emoswidgets/flattable.h>

namespace Idl{
namespace Internal{
class IdlEditor;

class PropertyWidget : public EmosWidgets::FlatTable
{
    Q_OBJECT
public:
    PropertyWidget(IdlEditor* IdlEditor, QWidget *parent = nullptr);
    ~PropertyWidget();

    void loadFromDoc();
    void exportToDoc();
signals:
    void propertyChange();
public slots:
    void appendRow(QString name = "", QString type = "");
    void removeOneRow();
private slots:
    void onModify();
private:
    void restoreControl();
    bool checkNameRepeat(const QString name);
    IdlEditor*  m_editor = nullptr;
    QStringList m_propertyTypeList;
    QStringList m_propertyNameList;
};

}
}
#endif // PROPERTYWIDGET_H
