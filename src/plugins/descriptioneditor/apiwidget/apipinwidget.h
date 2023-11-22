#ifndef APIPINWIDGET_H
#define APIPINWIDGET_H

#include "apiwidgetbase.h"

class ApiPinWidget : public ApiWidgetBase
{
    Q_OBJECT

public:
    explicit ApiPinWidget(QDomDocument* doc, QWidget *parent = nullptr);
    ~ApiPinWidget();

    virtual void widgetData(QDomDocument* doc, QDomElement* element);
    virtual void loadWidget(EmosTools::tSrvParsingService srv) override;
public slots:
    void slotAddRow();
private:
};

#endif // APIPINWIDGET_H
