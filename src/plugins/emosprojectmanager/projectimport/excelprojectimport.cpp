#include "excelprojectimport.h"

namespace QmakeProjectManager {
namespace Internal {

ExcelProjectImport::ExcelProjectImport(ProjectExplorer::Project * pro, QObject *parent)
    : ProjectImport{pro, parent}
{
}

bool ExcelProjectImport::import()
{
    qDebug() << "ExcelProjectImport";
    return true;
}

}}
