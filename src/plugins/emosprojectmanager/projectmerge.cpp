#include "projectmerge.h"
#include "writer/type_writer.h"
#include "writer/api_writer.h"

#include "mergewizards/typewizardpage.h"
#include "mergewizards/apiwizardpage.h"
#include "mergewizards/idlwizardpage.h"
#include "mergewizards/cfvwizardpage.h"

#include <QDir>
#include <QFileInfo>
#include <QFile>


ProjectMerge::ProjectMerge(QObject *parent)
    : QObject{parent}
{
    m_wizard = new MergeWizard;
}

bool ProjectMerge::merge(QString projectPath)
{
    //QString mergedProjectPath = getMergedProjectPath();
    //if (mergedProjectPath.isEmpty())
    {
        //return false;
    }
    if (m_wizard)
    {
        delete m_wizard;
        m_wizard = new MergeWizard;
    }
    m_wizard->setProjectPath(projectPath);
    m_wizard->show();    
    return true;
}

bool ProjectMerge::typeMerge()
{
    return true;
}

bool ProjectMerge::apiMerge()
{
    return true;
}

bool ProjectMerge::idlMerge()
{
    return true;
}

bool ProjectMerge::cfvMerge()
{
    return true;
}

QString ProjectMerge::getMergedProjectPath()
{
    QString mergedFilePath = QFileDialog::getOpenFileName(Q_NULLPTR,"Choose merged file...",".","*.emos");
    if (mergedFilePath.isEmpty())
    {
        qDebug()<<"choose merged file failed";
        return QString();
    }
    return QFileInfo(mergedFilePath).dir().path();
}

void ProjectMerge::slotMerge(const QString &projectPath)
{
    merge(projectPath);
}
