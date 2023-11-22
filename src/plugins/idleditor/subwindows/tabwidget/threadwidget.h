#ifndef THREADWIDGET_H
#define THREADWIDGET_H
#include <utils/emoswidgets/flattable.h>
#include <QDomDocument>
#include <QTableView>
#include <QComboBox>
#include <QStandardItemModel>
namespace Idl{
namespace Internal {
class IdlEditor;
class ServiceWidget;

enum INTERFACETYPE{
    ThreadType = 0,
    EventType = 1,
    FieldType = 2,
    DataType = 3,
    TimeType = 4
};

class ThreadWidget : public EmosWidgets::FlatTable
{
    Q_OBJECT
public:
    ThreadWidget(IdlEditor* IdlEditor, QWidget *parent = nullptr);
    ~ThreadWidget();

    void loadFromDoc();
    void exportToDoc();
signals:
    void threadChange();
public slots:
    void appendRow(QString name = "", QString type = "Normal", QString data = "");
    void removeOneRow();
public slots:
    void onWidgetChange(ServiceWidget* w);

private slots:
    void onModify();
    void onthreadTypeChanged(QString type);
private:
    void restoreControl();
    void updateDataList(const QStringList &serviceList);
    void ExportToTable();
    void addCellWidget(INTERFACETYPE kind, int rowCount, QString content);
    bool checkNameRepeat(const QString name);
    void updateCombobox();
    IdlEditor*  m_editor = nullptr;
    QStringList m_threadTypeList;
    QStringList m_eventTypeList;
    QStringList m_fieldTypeList;
    QStringList m_pinTypeList;
    QStringList m_threadNameList;
    bool m_changeFlag = false;
};

}}

#endif // THREADWIDGET_H
