#ifndef DESCRIPTIONDOCUMENT_H
#define DESCRIPTIONDOCUMENT_H

#include <coreplugin/idocument.h>
#include <coreplugin/editormanager/ieditor.h>
#include <generaleditor/textdocument.h>
#include <QDomDocument>

namespace DescriptionEditor {
namespace Internal {

class DescriptionEditor;
class DescriptionDocument : public TextEditor::TextDocument
{
    Q_OBJECT

public:
    explicit DescriptionDocument(DescriptionEditor* editor, QObject *parent = nullptr);


    //IDocument
    OpenResult open(QString *errorString, const QString &fileName,
                    const QString &realFileName) override;
    bool save(QString *errorString, const QString &fileName, bool autoSave) override;
    bool reload(QString *errorString, ReloadFlag flag, ChangeType type) override;
    void setFilePath(const Utils::FilePath &newName) override;
signals:

private:
    DescriptionEditor* m_editor;
};
}}
#endif // DESCRIPTIONDOCUMENT_H
