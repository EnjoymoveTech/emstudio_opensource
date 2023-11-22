#include "env.h"
#include <QString>
#include <QStringList>
#include <QDebug>
#include <QMutex>
#include <QSemaphore>

namespace Utils {

QMutex m_mutexLibrarySearchPath{QMutex::RecursionMode::Recursive};

Env::Env()
{

}

Env::~Env()
{

}

void Env::setLibrarySearchPath(const QString& strPath)
{
    lockLibrarySearchPathMutex();
#ifdef Q_OS_WIN//如果是WINDOWS系统
    QString path = QString::fromLocal8Bit(qgetenv("PATH"));
    QStringList pathList= path.split(";");
    int iPathIndex=pathList.indexOf(strPath);
    if (iPathIndex<0)
    {
        QString strPathNow=strPath+";"+path;
        qputenv("PATH", strPathNow.toLocal8Bit());
        qDebug()<<"setLibrarySearchPath:"<<strPath;
        //qDebug()<<"PathNow:"<<strPathNow;
    }
#else//LINUX
    QString path = QString::fromLocal8Bit(qgetenv("LD_LIBRARY_PATH"));
    QStringList pathList= path.split(":");
    int iPathIndex=pathList.indexOf(strPath);
    if (iPathIndex<0)
    {
        QString strPathNow=strPath+":"+path;
        qputenv("LD_LIBRARY_PATH", strPathNow.toLocal8Bit());
        qDebug()<<"setLibrarySearchPath:"<<strPath;
        qDebug()<<"PathNow:"<<strPathNow;
    }
#endif
    unlockLibrarySearchPathMutex();
}

void Env::removeLibrarySearchPath(const QString& strPath)
{
    lockLibrarySearchPathMutex();
#ifdef Q_OS_WIN//如果是WINDOWS系统
    QString path = QString::fromLocal8Bit(qgetenv("PATH"));
    QStringList pathList= path.split(";");
    int iPathIndex=pathList.indexOf(strPath);
    if (iPathIndex>=0)
    {
        pathList.removeAt(iPathIndex);
        QString strPathNow=pathList.join(";");
        qputenv("PATH", strPathNow.toLocal8Bit());
        qDebug()<<"removeLibrarySearchPath:"<<strPath;
        qDebug()<<"PathNow:"<<strPathNow;
    }
#else//LINUX
    QString path = QString::fromLocal8Bit(qgetenv("LD_LIBRARY_PATH"));
    QStringList pathList= path.split(":");
    int iPathIndex=pathList.indexOf(strPath);
    if (iPathIndex>=0)
    {
        pathList.removeAt(iPathIndex);
        QString strPathNow=pathList.join(":");
        qputenv("LD_LIBRARY_PATH", strPathNow.toLocal8Bit());
        qDebug()<<"removeLibrarySearchPath:"<<strPath;
        qDebug()<<"PathNow:"<<strPathNow;
    }
#endif
    unlockLibrarySearchPathMutex();
}

void Env::lockLibrarySearchPathMutex()
{
    m_mutexLibrarySearchPath.lock();
}

void Env::unlockLibrarySearchPathMutex()
{
    m_mutexLibrarySearchPath.unlock();
}
}
