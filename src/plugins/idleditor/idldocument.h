#ifndef IDLDOCUMENT
#define IDLDOCUMENT
#include <coreplugin/idocument.h>
#include <generaleditor/textdocument.h>
#include <QDomDocument>

namespace Idl {
namespace Internal{
class IdlEditor;

class IdlDocument : public TextEditor::TextDocument
{
    Q_OBJECT
public:
    IdlDocument(IdlEditor* editor, QObject *parent = nullptr);
    ~IdlDocument();
public:
    OpenResult open(QString *errorString, const QString &fileName,
                    const QString &realFileName) override;
    bool save(QString *errorString, const QString &fileName, bool autoSave) override;
    bool reload(QString *errorString, ReloadFlag flag, ChangeType type) override;
    void setFilePath(const Utils::FilePath &newName) override;
public:
    QDomDocument* getRootDoc();
private:
    QDomDocument *m_doc;
    IdlEditor* m_editor = nullptr;
};
}
} // namespace Idl

#endif
