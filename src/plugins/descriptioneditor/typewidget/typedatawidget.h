#ifndef TYPEDATAWIDGET_H
#define TYPEDATAWIDGET_H

#include "typewidgetbase.h"

class TypeDataWidget : public TypeWidgetBase
{
    Q_OBJECT

public:
    explicit TypeDataWidget(QDomDocument* doc, QWidget *parent = nullptr);
    ~TypeDataWidget();

    virtual void widgetData(QDomDocument* doc, QDomElement* element) override;

public slots:
    void slotAddRow();
private:
    virtual void loadWidget() override;
};

#endif // TYPEDATAWIDGET_H
