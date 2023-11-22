#ifndef APIPROPERTYWIDGET_H
#define APIPROPERTYWIDGET_H
#include <QObject>
#include <QLineEdit>
#include <QLabel>
#include <QDomDocument>
#include <utils/emoswidgets/flattable.h>

namespace EmosTools{class tSrvParsingService;}
class QRadioButton;
class QCheckBox;
class ApiPropertyWidget : public EmosWidgets::FlatTable
{
    Q_OBJECT
public:
    ApiPropertyWidget(QWidget* parent = nullptr);
    ~ApiPropertyWidget();

    void loadWidget(EmosTools::tSrvParsingService srv);
    void widgetData(QDomDocument *doc, QDomElement *element);
signals:
    void modify();

private:
    void initTable();

    QString m_idlname;
    QTableWidgetItem* m_priority;
    QRadioButton* m_radio_c;
    QRadioButton* m_radio_cpp;
    QCheckBox* m_inProcess;
};
#endif // APIPROPERTYWIDGET_H
