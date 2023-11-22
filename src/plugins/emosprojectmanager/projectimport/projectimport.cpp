#include "projectimport.h"

namespace QmakeProjectManager {
namespace Internal {

ProjectImport::ProjectImport(ProjectExplorer::Project * pro, QObject *parent)
    : QObject{parent},
      m_project(pro)
{
}

}}
