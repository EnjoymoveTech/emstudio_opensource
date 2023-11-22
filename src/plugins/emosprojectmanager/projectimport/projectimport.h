#ifndef PROJECTIMPORT_H
#define PROJECTIMPORT_H

#include <QObject>

namespace ProjectExplorer { class Project; }

namespace QmakeProjectManager {
namespace Internal {

class ProjectImport : public QObject
{
    Q_OBJECT
public:
    explicit ProjectImport(ProjectExplorer::Project * pro, QObject *parent = nullptr);

    virtual bool import() = 0;

private:
    ProjectExplorer::Project * m_project;
};
}}
#endif // PROJECTMERGE_H
