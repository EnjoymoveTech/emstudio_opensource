#ifndef PROPERTYEDITORWIDGET_H
#define PROPERTYEDITORWIDGET_H
#include <QObject>
#include <QLineEdit>
#include <QLabel>
#include <QDomDocument>
#include <utils/emoswidgets/flattable.h>

namespace Idl {
namespace Internal {
class IdlEditor;

class OidEditWidget : public QWidget
{
    Q_OBJECT
public:
    OidEditWidget(QWidget* parent = nullptr);
    ~OidEditWidget();

    void setOid1(const QString& str);
    void setOid2(const QString& str);
    QString getOid() const;
    QString getOid1() const;
    QString getOid2() const;
signals:
    void modify();
private:
    QLineEdit*  m_edit;
    QLabel*     m_label;
};

class PropertyEditorWidget : public EmosWidgets::FlatTable
{
    Q_OBJECT
public:
    PropertyEditorWidget(IdlEditor* IdlEditor,QWidget* parent = nullptr);
    ~PropertyEditorWidget();

    void loadFromDoc();
    void exportToDoc();
public slots:
    void onIdlNameChange(const QString& from, const QString& to);
private slots:
    void onModify();
private:
    IdlEditor*         m_editor;
    QString            m_idlname;
    OidEditWidget*     m_oidEditWidget;
    QTableWidgetItem*  m_priority;
};
}}
#endif // PROPERTYEDITORWIDGET_H
