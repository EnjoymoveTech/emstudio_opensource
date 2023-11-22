#ifndef APIMETHODWIDGET_H
#define APIMETHODWIDGET_H

#include "apiwidgetbase.h"
#include "qtablewidget.h"
#include <emostools/api_description_parser.h>

namespace EmosWidgets { class AutoMultiComoBox; }
class AdvanceMethod;
class QListWidget;
class QListWidgetItem;
class QPushButton;
class ApiMethodWidget : public ApiWidgetBase
{
    Q_OBJECT

public:
    explicit ApiMethodWidget(QDomDocument* doc, QWidget *parent = nullptr);
    ~ApiMethodWidget();

    virtual void widgetData(QDomDocument* doc, QDomElement* element) override;
    virtual void loadWidget(EmosTools::tSrvParsingService srv) override;
public slots:
    void slotAddRow();
    void showAdvanceMethod();
    void paraChanged(QString nowText);
private:
    QString paraName(const QString& type, int index);
    void addButtonForMethod(EmosWidgets::AutoMultiComoBox* comboBox);
    QList<EmosTools::tParam> getInputParams(int row, int col);
    bool isUseAdvance(const QStringList& paraList, QList<EmosTools::tParam> &param);

    QListWidget* m_typeChooser;
    AdvanceMethod* m_advanceMethod;
};

#endif // APIMETHODWIDGET_H
