#ifndef PROCESSSTATICWIDGET_H
#define PROCESSSTATICWIDGET_H

#include "qmt/infrastructure/qmt_global.h"
#include "qmt/diagram/dboundary.h"
#include "qmt/model_widgets_ui/propertiesviewmview.h"

namespace EmosWidgets {class FlatTable;}
namespace EmosTools {struct tSrvProperty;}

class QCheckBox;
class QTableWidgetItem;
namespace qmt {

struct endStruct
{
    QString endB;
    QString endA;
    bool operator== (const endStruct& other) const
    {
        if(endB == other.endB && endA == other.endA)
            return true;
        else
            return false;
    }
};

class MDiagram;
class ProcessStaticWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ProcessStaticWidget(const DBoundary* dboundary, PropertiesView::MView* m_view);
    ~ProcessStaticWidget();
    void load(const DBoundary* dboundary, PropertiesView::MView* m_view);

signals:
    void dataChanged(const QHash<QString, QString>& propertyInfo);
    void selectChanged(bool checked);

public slots:
    void onStaticTableChanged(QTableWidgetItem *item);
    void onStaticSelectChanged(bool checked);

private:
    QList<endStruct> findAssociations(MDiagram * mdiagram);
    QStringList findServiceName(const DBoundary* dboundary, MDiagram * mdiagram, QList<endStruct> ends, QString& eventNameList, QString& pinNameList);

    EmosWidgets::FlatTable* m_staticConfigTable;
    QCheckBox* m_staticConfigCheck;
    int m_basSrvCount;
    QString eventNameList;
    QString pinNameList;
    QStringList endBNameList;
};

} // namespace qmt

#endif // PROCESSSTATICWIDGET_H
