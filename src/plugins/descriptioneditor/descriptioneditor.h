#ifndef DESCRIPTIONEDITOR_H
#define DESCRIPTIONEDITOR_H

#include <generaleditor/generaleditor.h>
#include <coreplugin/editormanager/ieditor.h>

class QToolButton;
namespace DescriptionEditor {

enum DescriptionType{
    API,
    TYPE,
    None,
};

namespace Internal {

class DescriptionEditor : public Core::IEditor
{
    Q_OBJECT
public:
    DescriptionEditor();
    ~DescriptionEditor();

    Core::IDocument *document() const override;
    QWidget *toolBar() override;

    QString saveWidget();
    void loadWidget(const QString& text);

    void updateCombo(const QString &text);

    QWidget* toolBarRight();

    static DescriptionType descriptionType(Utils::FilePath files);
    static DescriptionType descriptionType(QDomDocument* doc);
public slots:
    void editorChanged(IEditor *editor);
private:
    class DescriptionEditorPrivate* d = nullptr;

    void init();
    void addToorBar();
};
}}
#endif // DESCRIPTIONEDITOR_H
