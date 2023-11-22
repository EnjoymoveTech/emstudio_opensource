#include "idldocument.h"
#include "idleditorconstants.h"

#include <coreplugin/icore.h>
#include <coreplugin/editormanager/ieditor.h>
#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/actionmanager/commandbutton.h>
#include <coreplugin/editormanager/editormanager.h>
#include <coreplugin/documentmanager.h>
#include <utils/reloadpromptutils.h>
#include <utils/fileutils.h>
#include <emostools/api_description_parser.h>

#include <QFileInfo>
#include <QDir>
#include <qdebug.h>
#include <QHBoxLayout>
#include <QMenu>
#include <QToolBar>
#include <QInputDialog>
#include <QClipboard>
#include <QXmlStreamReader>
#include "idleditor.h"
using namespace Utils;

namespace Idl {
namespace Internal{
IdlDocument::IdlDocument(IdlEditor* editor, QObject *parent)
{
    m_editor = editor;
    m_doc = new QDomDocument;
    setId(Idl::Constants::C_IDLEDITOR);
    setMimeType(QLatin1String(Idl::Constants::C_IDL_MIMETYPE));
}

IdlDocument::~IdlDocument()
{
    delete m_doc;
    m_doc = nullptr;
}

Core::IDocument::OpenResult IdlDocument::open(QString *errorString, const QString &fileName, const QString &realFileName)
{
    Q_UNUSED(fileName)
    TextDocument::open(errorString,fileName,realFileName);
    //如果xml格式不对则返回错误
    if(!m_doc->setContent(this->plainText()))
    {
        return OpenResult::ReadError;
    }
    //EmosTools::ApiParser::getInstance()->reloadXml();
    m_editor->loadWidget();
    return OpenResult::Success;
}

bool IdlDocument::save(QString *errorString, const QString &name, bool autoSave)
{

    const FilePath oldFileName = filePath();
    const FilePath actualName = name.isEmpty() ? oldFileName : FilePath::fromString(name);
    if (actualName.isEmpty())
    {
         return false;
    }
    this->setPlainText(Utils::XmlFileSaver::serializeStr(m_editor->saveWidget()));
    if(!TextDocument::save(errorString,actualName.toString(),autoSave))
    {
        return false;
    }
    return true;
}

void IdlDocument::setFilePath(const FilePath &newName)
{
    IDocument::setFilePath(newName);
}

QDomDocument* IdlDocument::getRootDoc()
{
    return m_doc;
}

bool IdlDocument::reload(QString *errorString, ReloadFlag flag, ChangeType type)
{
    if (flag == FlagIgnore)
        return true;
    if (type == TypePermissions) {
        emit changed();
    } else {
        emit aboutToReload();
        QString fn = filePath().toString();
        const bool success = (open(errorString, fn, fn) == OpenResult::Success);
        emit reloadFinished(success);
        return success;
    }
    return true;
}

}
} // namespace Idleditor
