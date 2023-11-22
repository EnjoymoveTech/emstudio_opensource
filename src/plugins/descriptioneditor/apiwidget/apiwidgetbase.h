#ifndef APIWIDGETBASE_H
#define APIWIDGETBASE_H

#include <QWidget>
#include <QStandardItemModel>

namespace EmosWidgets {class FlatTable;}
namespace EmosTools{struct tSrvParsingService;}
class QDomDocument;
class QDomElement;
class QTableWidgetItem;
class ApiWidgetBase : public QWidget
{
    Q_OBJECT

public:
    explicit ApiWidgetBase(QDomDocument* doc, QWidget *parent = nullptr);
    ~ApiWidgetBase();

    QString serviceName() const;
    void setServiceName(const QString& name);

    void getTypeInfo();

    void appendRow(QStringList items);
    void appendComboBox(QStringList &items, int row, int col, QString currentText, bool editorEnable = true);
    void appendSpinBox(int min, int max, int row, int col, int currentValue);
    void removeCurrentRow();
    void selectFirstRow();
    void selectLastRow();
    QString getDefaultName(QString tagName);

    virtual void widgetData(QDomDocument* doc, QDomElement* element) = 0;
    virtual void loadWidget(EmosTools::tSrvParsingService srv) = 0;

    EmosWidgets::FlatTable* getTable();
    void updateCombo();
signals:
    void modify();
public slots:
    void slotAddRow();
    void slotRemoveRow();
    void slotItemChanged(QTableWidgetItem *item);
protected:
    EmosWidgets::FlatTable* m_table = nullptr;
    QString m_ServiceName;
    QDomDocument* m_doc;
    QStringList m_typeList;
    QStringList m_typeWithoutPointer;
    QString m_tagName;

    int makeUniquelyId(QList<int> idList, int step = 1, int start = 1);
};

#endif // APIMETHODWIDGET_H
