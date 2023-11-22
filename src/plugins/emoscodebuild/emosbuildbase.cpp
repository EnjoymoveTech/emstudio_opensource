#include "emosbuildbase.h"

#include <coreplugin/icore.h>
#include <emostools/cfvparser.h>
#include <emostools/type_description_parser.h>
#include <emostools/api_description_parser.h>

#include <QMessageBox>
#include <buildoutpane/buildoutlogmanager.h>
#include "injamanager.h"

using namespace EmosTools;

namespace EmosCodeBuild {


EmosBuildBase::EmosBuildBase():
    m_engineManager(new InjaManager)
{

}

EmosBuildBase::~EmosBuildBase()
{
    delete m_engineManager;
}

void EmosBuildBase::setCodeRootDir(const QString &dir)
{
    m_engineManager->setCodeRootDir(dir);
}

bool EmosBuildBase::codeWriter(const QString& path,const QString& content)
{
    QFile file(path);
    for(int i = 0; i < 5; i++)
    {
        if(!file.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            if(i == 4)
            {
                QString errLog = "Open File Error: File " + path + " can't open, please Retry or Confirm whether the file is occupied!";
                BuildoutLogManager::instance()->writeWithTime(errLog, Utils::ErrorMessageFormat);
                return false;
            }
            Sleep(500);
            continue;
        }
        break;
    }
    QByteArray strBytes = content.toUtf8();
    file.write(strBytes,strBytes.length());
    file.close();
    return true;
}

bool EmosBuildBase::emptyLogPrint(const QString &ContentT,const QString &Path)
{
    if(ContentT.isEmpty())
    {
        QString errLog = Path + " render fail!";
        BuildoutLogManager::instance()->writeWithTime(errLog, Utils::ErrorMessageFormat);
        return false;
    }
    return true;
}

}
