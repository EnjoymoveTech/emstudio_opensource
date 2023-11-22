#ifndef TYPEWIDGET_H
#define TYPEWIDGET_H

#include <QWidget>
#include "descriptioneditorwidget.h"
#include "typedetailwidget.h"
#include "typestructwidget.h"
#include <QTreeView>
#include <QStyledItemDelegate>
#include <QDomDocument>
#include <QStandardItemModel>
#include <QListWidget>

namespace DescriptionEditor {
namespace Internal {

enum importMode
{
    Cover,
    Merge,
};

class TypeWidget : public DescriptionEditorWidget
{
    Q_OBJECT

public:
    explicit TypeWidget(DescriptionEditor* editor, QWidget *parent = nullptr);
    ~TypeWidget();

    virtual QDomDocument saveWidget() override;
    virtual void loadWidget(QDomDocument* doc) override;

public slots:
    void slotImportCover() override;
    void slotImportMerge() override;
    void slotExport() override;
private:
    void init();
    void slotImport(QString mode);
    void writeXml(QString fileName);
    //TypeDetailWidget* m_typeDetailWidget;
    //QStackedWidget* m_stackedWidget;

    QDomDocument m_doc;
    QString projectDirectory;
    TypeStructWidget* m_structWidget = nullptr;
};
}}
#endif // TYPEWIDGET_H
