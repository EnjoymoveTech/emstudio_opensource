#ifndef DESCRIPTIONEDITORWIDGET_H
#define DESCRIPTIONEDITORWIDGET_H

#include <utils/fancymainwindow.h>

#include <coreplugin/designmode.h>
#include <coreplugin/coreconstants.h>
#include <coreplugin/dialogs/ioptionspage.h>
#include <coreplugin/icore.h>
#include <coreplugin/helpmanager.h>
#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/actionmanager/actioncontainer.h>
#include <coreplugin/editormanager/editormanager.h>
#include <coreplugin/modemanager.h>
#include <coreplugin/minisplitter.h>
#include <coreplugin/outputpane.h>

#include <QStackedWidget>
#include <QDomDocument>

using namespace Core;
using namespace Utils;

namespace DescriptionEditor {
namespace Internal {

class DescriptionEditor;
class DescriptionEditorWidgetPrivate;
class DescriptionEditorWidget : public Utils::FancyMainWindow
{
    Q_OBJECT

public:
    DescriptionEditorWidget(DescriptionEditor* editor, QWidget *parent = nullptr);
    ~DescriptionEditorWidget();

    virtual QDomDocument saveWidget() = 0;
    virtual void loadWidget(QDomDocument* doc) = 0;

    DescriptionEditor* editor();
public slots:
    void slotText(bool checked);
    void slotModify();
    virtual void slotAddRow(){};
    virtual void slotRemoveRow(){};
    virtual void slotImportExcel(){};
    virtual void slotImportCover(){};
    virtual void slotImportMerge(){};
    virtual void slotExport(){};
    virtual void slotCreateNewFile(const Utils::MimeType& type, const void* data, const Core::EditorManager::DescriptionType& fileType, const ProjectExplorer::Project* project){};
protected:
    QWidget *mainWidget = nullptr;
private:
    DescriptionEditorWidgetPrivate* d = nullptr;
};

}}

#endif // DESCRIPTIONEDITORWIDGET_H
