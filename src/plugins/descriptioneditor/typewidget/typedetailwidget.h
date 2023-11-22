#ifndef TYPEDETAILWIDGET_H
#define TYPEDETAILWIDGET_H

#include <QTabWidget>

class QDomDocument;
class QDomElement;
class TypeDataWidget;
namespace DescriptionEditor {
namespace Internal {

class TypeDetailWidget : public QTabWidget
{
    Q_OBJECT

public:
    explicit TypeDetailWidget(QDomDocument* doc, QWidget *parent = nullptr);
    ~TypeDetailWidget();

    void widgetData(QDomDocument *doc, QDomElement* element);

signals:
    void modify();
public slots:
    void slotAddRow();
    void slotRemoveRow();
private:
    QDomDocument* m_doc;
    QString m_ServiceName;
    QString m_ServiceId;
    QString m_ServiceVersion;
    TypeDataWidget* m_dataWidget = nullptr;
};
}}

#endif // TYPEDETAILWIDGET_H
