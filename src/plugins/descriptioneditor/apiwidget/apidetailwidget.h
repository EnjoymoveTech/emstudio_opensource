#ifndef APIDETAILWIDGET_H
#define APIDETAILWIDGET_H

#include <QTabWidget>

class QDomDocument;
class QDomElement;
class ApiMethodWidget;
class ApiEventWidget;
class ApiFieldWidget;
class ApiPinWidget;
class QTreeView;
class InspectorModel;
class ApiPropertyWidget;
namespace Core {class MiniSplitter;}
namespace EmosWidgets {class FlatTable;}
namespace EmosTools{struct tSrvParsingService;}
namespace DescriptionEditor {
namespace Internal {

enum tabSelect {
    Method,
    Event,
    Field,
    Pin,
};

class ApiDetailWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ApiDetailWidget(QDomDocument* doc, QWidget *parent = nullptr);
    ~ApiDetailWidget();

    QString serviceName() const;
    void setServiceName(const QString& name);

    QString serviceId() const;
    void setServiceId(const QString& id);

    QString serviceVersion() const;
    void setServiceVersion(const QString& ver);

    void widgetData(QDomDocument *doc, QDomElement* element);
    void loadWidget(EmosTools::tSrvParsingService srv);

    tabSelect currentTab();
    void setTab(tabSelect selection);

    QHash<QString, EmosWidgets::FlatTable*> getTableHash();

    void updateCombo();
signals:
    void modify();
public slots:
    void slotAddRow();
    void slotRemoveRow();
    void slotTreeReload();
private:
    QDomDocument* m_doc;
    Core::MiniSplitter *detailSplitter = nullptr;
    Core::MiniSplitter *rightSplitter = nullptr;
    QTreeView* treeWidget = nullptr;
    InspectorModel* apiInfo = nullptr;
    ApiPropertyWidget* apiProperty = nullptr;

    QString m_ServiceName;
    QString m_ServiceId;
    QString m_ServiceVersion;

    ApiMethodWidget* m_methodWidget = nullptr;
    ApiEventWidget* m_eventWidget = nullptr;
    ApiFieldWidget* m_fieldWidget = nullptr;
    ApiPinWidget* m_pinWidget = nullptr;

    QTabWidget* m_tabWidget = nullptr;
};
}}

#endif // APIDETAILWIDGET_H
