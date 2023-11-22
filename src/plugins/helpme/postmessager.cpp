#include "postmessager.h"
#include "app/app_version.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QByteArray>
#include <QFileInfo>
#include <QDir>

namespace Helpme {
namespace Internal {

const QString _URL = "http://x.x.x.x:xxxx";

PostMessager::PostMessager()
{
    m_manager = new QNetworkAccessManager;
    m_timer = new QTimer;
    m_isConnected = false;
    m_timer->setInterval(5000);
    connect(m_timer,&QTimer::timeout,this,&PostMessager::slotTimeout);
}

PostMessager::~PostMessager()
{
    delete m_manager;
    delete m_timer;
}

void PostMessager::postJson(int id,int priority,QString name,QString description,QStringList fileNameList)
{
    QJsonDocument document;
    QJsonObject rootObj;
    rootObj.insert("id",id);
    rootObj.insert("priority",priority);
    rootObj.insert("name",name);
    rootObj.insert("description",description);
    rootObj.insert("version",QLatin1String(Core::Constants::IDE_VERSION_DISPLAY));
    QJsonArray fileArray;
    foreach (const QString& fileName,fileNameList)
    {
        fileArray.append(QJsonValue(QFileInfo(fileName).fileName()));
    }
    rootObj.insert("files",fileArray);
    document.setObject(rootObj);
    qDebug()<<document.toJson();
    QUrl url(_URL + "/bugreport/send");
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader,QVariant("application/json"));
    m_reply = m_manager->post(request,document.toJson());
    connect(m_manager,&QNetworkAccessManager::finished,this,&PostMessager::slotPostJsonFinished);
    m_timer->start();
    m_loop.exec();
}

void PostMessager::postFile(QString fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly))
    {
        qDebug()<<"open file failed";
        return;
    }
    QFileInfo fileinfo(fileName);
    QUrl url(_URL + QString("/bugreport/send?id=%1&fileName=%2").arg(id()).arg(fileinfo.fileName()));
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader,QVariant("application/file"));
    QByteArray array = file.readAll();
    m_reply = m_manager->post(request,array);
    file.close();
    connect(m_manager,&QNetworkAccessManager::finished,this,&PostMessager::slotPostFileFinished);
    m_timer->start();
    m_loop.exec();
}

void PostMessager::postGetID()
{
    QUrl url(_URL + "/bugreport/getid");
    QNetworkRequest request(url);
    m_reply = m_manager->post(request,QByteArray());
    connect(m_manager,&QNetworkAccessManager::finished,this,&PostMessager::slotPostGetIDFinished);
    m_timer->start();
    m_loop.exec();
}

void PostMessager::slotPostGetIDFinished(QNetworkReply* reply)
{
    m_timer->stop();
    if (reply->error() == QNetworkReply::NoError)
    {
        QByteArray array = reply->readAll();
        m_id = array.toInt();
    }
    reply->deleteLater();
    disconnect(m_manager,&QNetworkAccessManager::finished,this,&PostMessager::slotPostGetIDFinished);
    m_loop.exit();
}

void PostMessager::slotPostJsonFinished(QNetworkReply* reply)
{
    m_timer->stop();
    disconnect(m_manager,&QNetworkAccessManager::finished,this,&PostMessager::slotPostJsonFinished);
    reply->deleteLater();
    m_loop.exit();
}

void PostMessager::slotPostFileFinished(QNetworkReply* reply)
{
    m_timer->stop();
    disconnect(m_manager,&QNetworkAccessManager::finished,this,&PostMessager::slotPostFileFinished);
    reply->deleteLater();
    m_loop.exit();
}

void PostMessager::slotTimeout()
{
    if (m_loop.isRunning())
    {
        m_isConnected = false;
        m_timer->stop();
        m_reply->abort();
    }
}

}
}
