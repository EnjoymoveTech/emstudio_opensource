#ifndef TYPEWIDGETBASE_H
#define TYPEWIDGETBASE_H

#include <QWidget>
#include <QStandardItemModel>

namespace EmosWidgets {class FlatTable;}

class QDomDocument;
class QDomElement;
class TypeWidgetBase : public QWidget
{
    Q_OBJECT

public:
    explicit TypeWidgetBase(QDomDocument* doc, QWidget *parent = nullptr);
    ~TypeWidgetBase();

    QString serviceName() const;
    void setServiceName(const QString& name);

    void appendRow(QStringList items);
    void removeCurrentRow();
    void selectFirstRow();
    void selectLastRow();

    virtual void widgetData(QDomDocument* doc, QDomElement* element) = 0;

signals:
    void modify();
public slots:
    void slotAddRow();
    void slotRemoveRow();
protected:
    virtual void loadWidget() = 0;

    EmosWidgets::FlatTable* m_table = nullptr;
    QString m_ServiceName;
    QDomDocument* m_doc;
};

#endif // TYPEWIDGETBASE_H
