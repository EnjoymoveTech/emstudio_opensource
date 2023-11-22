#ifndef APILISTWIDGET_H
#define APILISTWIDGET_H

#include <QWidget>
#include <QBoxLayout>

class QListWidget;
class QDomDocument;
class QListWidgetItem;
namespace EmosTools{struct tSrvParsingService;}
namespace DescriptionEditor {
namespace Internal {

class ApiListWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ApiListWidget(QWidget *parent = nullptr);
    ~ApiListWidget();

    QList<EmosTools::tSrvParsingService> loadService(QDomDocument* doc);

public slots:
    void slotAddRow();
    void slotRemoveRow();
    void itemChanged(QListWidgetItem *item);
signals:
    void sigSelectChanged(const QString &service);
    void sigAddService(const QString &service);
    void sigRemoveService(const QString &service, const QString &selectService);
    void sigModifyService(const QString &service, const QString &newName);
private:
    void addIdl(const QString& serviceName);
    void addToorBar(QBoxLayout* layoutToolBar);
    void serviceChangeName(const QString& oldName, const QString& newName);
    QStringList getListValue();
    QListWidget* m_listWidget;
    QString m_curEditorName;
};
}}
#endif // APIWIDGET_H
