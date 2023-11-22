#ifndef APIFIELDWIDGET_H
#define APIFIELDWIDGET_H

#include "apiwidgetbase.h"

class QCheckBox;
class ApiFieldWidget : public ApiWidgetBase
{
    Q_OBJECT

public:
    explicit ApiFieldWidget(QDomDocument* doc, QWidget *parent = nullptr);
    ~ApiFieldWidget();

    virtual void widgetData(QDomDocument* doc, QDomElement* element);
    virtual void loadWidget(EmosTools::tSrvParsingService srv) override;
public slots:
    void slotAddRow();
private:
    void setGroupBox(int row, int col, int type);
    QString getSelectType(int row);

    QHash<int, QCheckBox*> m_GETHash;
    QHash<int, QCheckBox*> m_SETHash;
    QHash<int, QCheckBox*> m_NOTIFYHash;
};

#endif // APIFIELDWIDGET_H
