#ifndef EXCELPROJECTIMPORT_H
#define EXCELPROJECTIMPORT_H

#include <QObject>

#include "projectimport/projectimport.h"

namespace QmakeProjectManager {
namespace Internal {

class ExcelProjectImport : public ProjectImport
{
    Q_OBJECT
public:
    explicit ExcelProjectImport(ProjectExplorer::Project * pro, QObject *parent = nullptr);

    virtual bool import() final;

private:

};
}}
#endif // PROJECTMERGE_H
