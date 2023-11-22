#include "descriptioneditorfactory.h"
#include "descriptiondocument.h"

#include "descriptioneditor.h"
#include "descriptioneditorconstants.h"
#include <explorer/projectexplorerconstants.h>
#include <coreplugin/fileiconprovider.h>
#include <QCoreApplication>

namespace DescriptionEditor {
namespace Internal {

DescriptionEditorfactory::DescriptionEditorfactory()
{
    setId(Constants::DESCRIPTION_EDITOR_ID);
    setDisplayName(QCoreApplication::translate("OpenWith::Editors", Constants::DESCRIPTION_EDITOR_DISPLAY_NAME));
    addMimeType(Constants::MIME_TYPE_DESCRIPTION);
    Core::FileIconProvider::registerIconOverlayForSuffix(ProjectExplorer::Constants::FILE_DES, "description");
    setEditorCreator([] { return new DescriptionEditor(); });
}

}}
