#ifndef IDLEDITOR
#define IDLEDITOR
#include <generaleditor/generaleditor.h>
#include <coreplugin/editormanager/ieditor.h>

namespace Idl{
namespace Internal{
class IdlEditor : public Core::IEditor
{
    Q_OBJECT
public:
    IdlEditor();
    ~IdlEditor();
    Core::IDocument *document() const override;
    QWidget *toolBar() override;
public:
    QString saveWidget();
    void loadWidget();
public slots:
    void updateData();
private:
    class IdlEditorPrivate* d = nullptr;
    void widgetInit();
    void initToolBar();
};
}
} // namespace Idl

#endif
