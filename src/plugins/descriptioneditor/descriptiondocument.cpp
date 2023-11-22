#include "descriptiondocument.h"

#include "descriptioneditorconstants.h"
#include <coreplugin/icore.h>
#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/actionmanager/commandbutton.h>
#include <coreplugin/editormanager/editormanager.h>
#include <utils/reloadpromptutils.h>
#include <utils/fileutils.h>

#include <emostools/type_description_parser.h>

#include <QFileInfo>

#include "descriptioneditor.h"

using namespace Utils;
namespace DescriptionEditor {
namespace Internal {

DescriptionDocument::DescriptionDocument(DescriptionEditor* editor, QObject *parent):
    m_editor(editor)
{
    setId(Constants::DESCRIPTION_EDITOR_ID);
    setMimeType(QLatin1String(Constants::MIME_TYPE_DESCRIPTION));
}

Core::IDocument::OpenResult DescriptionDocument::open(QString *errorString,
                                                         const QString &fileName,
                                                         const QString &realFileName)
{
    EmosTools::TypeParser::getInstance()->reloadXml();
    TextDocument::open(errorString,fileName,realFileName);
    m_editor->loadWidget(this->plainText());
    return OpenResult::Success;
}

bool DescriptionDocument::save(QString *errorString, const QString &name, bool autoSave)
{
    const FilePath oldFileName = filePath();
    const FilePath actualName = name.isEmpty() ? oldFileName : FilePath::fromString(name);
    if (actualName.isEmpty())
        return false;

    this->setPlainText(Utils::XmlFileSaver::serializeStr(m_editor->saveWidget()));
    TextDocument::save(errorString,actualName.toString(),autoSave);

    return true;
}

void DescriptionDocument::setFilePath(const FilePath &newName)
{
    IDocument::setFilePath(newName);
}

bool DescriptionDocument::reload(QString *errorString, ReloadFlag flag, ChangeType type)
{
    if (flag == FlagIgnore)
        return true;
    if (type == TypePermissions) {
        emit changed();
    } else {
        emit aboutToReload();
        QString fn = filePath().toString();
        const bool success = (open(errorString, fn, fn) == OpenResult::Success);
        m_editor->loadWidget(this->plainText());

        emit reloadFinished(success);
        return success;
    }
    return true;
}

}}
