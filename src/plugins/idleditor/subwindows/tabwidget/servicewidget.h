#ifndef SERVICEWIDGET_H
#define SERVICEWIDGET_H
#include <QWidget>
#include <QDomDocument>
#include <QStringList>
#include <QStyledItemDelegate>
#include <utils/styledbar.h>
#include <utils/emoswidgets/flowtoolbox.h>
#include <coreplugin/editormanager/ieditor.h>
#include <coreplugin/idocument.h>

class QListWidgetItem;
class QListWidget;

namespace Idl{
namespace Internal{
class IdlEditor;

enum SERVICETYPE{
    SERVICELIST_GROUP,
    SERVICE_GROUP,
    QUOTESERVICE_GROUP
};

class ServiceWidget : public QWidget
{
    Q_OBJECT

public:
    ServiceWidget(IdlEditor* editor, QWidget *parent = nullptr);
    ~ServiceWidget();

    void loadFromDoc();
    void exportToDoc();
    QStringList getServices();
    QStringList getQuetoServices();
signals:
    void CurrentWidgetChange(ServiceWidget*);
public slots:
    void onCurrenWidgetChange(Core::IEditor* editor);
private slots:
    void onModify();

private:
    void onServiceChange();
    void updateDataForSelectService(SERVICETYPE type);
    void additemforGroup(QString name,SERVICETYPE type);
    void exportDocForSelectService(SERVICETYPE type);
private:
    EmosWidgets::DropFlowGroupWidget*   m_serviceListGroup = nullptr;
    EmosWidgets::DropFlowGroupWidget*   m_serviceGroup = nullptr;
    EmosWidgets::DropFlowGroupWidget*   m_quetoServiceGroup = nullptr;
    IdlEditor*                          m_editor = nullptr;

};

}}
#endif // SERVICEWIDGET_H
