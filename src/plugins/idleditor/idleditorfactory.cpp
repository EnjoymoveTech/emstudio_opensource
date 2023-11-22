#include <coreplugin/fileiconprovider.h>
#include <explorer/projectexplorerconstants.h>
#include <QCoreApplication>

#include "idleditorconstants.h"
#include "idleditor.h"
#include "idleditorfactory.h"

namespace Idl{
namespace Internal{

IdlEditorFactory::IdlEditorFactory()
{
    setId(Constants::IDLEDITOR_ID);
    setDisplayName(QCoreApplication::translate("Idl", Constants::C_IDLEDITOR_DISPLAY_NAME));
    addMimeType(Constants::C_IDL_MIMETYPE);
    Core::FileIconProvider::registerIconOverlayForSuffix(ProjectExplorer::Constants::FILE_IDL, "idl");
    setEditorCreator([]() { return new IdlEditor(); });
}

}
}
