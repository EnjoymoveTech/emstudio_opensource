#ifndef APIEVENTWIDGET_H
#define APIEVENTWIDGET_H

#include "apiwidgetbase.h"

class ApiEventWidget : public ApiWidgetBase
{
    Q_OBJECT

public:
    explicit ApiEventWidget(QDomDocument* doc, QWidget *parent = nullptr);
    ~ApiEventWidget();

    virtual void widgetData(QDomDocument* doc, QDomElement* element);
    virtual void loadWidget(EmosTools::tSrvParsingService srv) override;
public slots:
    void slotAddRow();
private:

};

#endif // APIEVENTWIDGET_H
