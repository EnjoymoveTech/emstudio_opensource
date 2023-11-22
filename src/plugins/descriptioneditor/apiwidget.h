#ifndef APIWIDGET_H
#define APIWIDGET_H

#include <QWidget>
#include "descriptioneditorwidget.h"

namespace EmosTools { struct tSrvParsingService; }
class QListWidget;
class QStackedWidget;
class InspectorModel;
class QTreeView;
namespace DescriptionEditor {
namespace Internal {

class ApiListWidget;
class ApiDetailWidget;
class ApiWidget : public DescriptionEditorWidget
{
    Q_OBJECT

public:
    explicit ApiWidget(DescriptionEditor* editor, QWidget *parent = nullptr);
    ~ApiWidget();
    QDomDocument* doc() {return m_doc;}

    virtual QDomDocument saveWidget() override;
    virtual void loadWidget(QDomDocument* doc) override;

    void updateCombo();

public slots:
    void textChanged(const QString &currentText);
    void slotAddRow() override;
    void slotRemoveRow() override;
    void slotImportExcel() override;
    void slotAddService(const QString &service);
    void slotRemoveService(const QString &service, const QString &selectService);
    void slotModifyService(const QString &service, const QString &newName);
private:
    void init();
    int makeUniquelyServiceId();
    ApiListWidget* m_apiListWidget;
    Core::MiniSplitter *mainSplitter = nullptr;
    QList<ApiDetailWidget*> m_apiDetailListWidget;
    QStackedWidget* m_stackedWidget;
    QDomDocument* m_doc;
    QString projectDirectory;
};
}}
#endif // APIWIDGET_H
