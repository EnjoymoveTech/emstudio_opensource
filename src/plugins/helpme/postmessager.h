#ifndef PostMessager_H
#define PostMessager_H

#include <QObject>
#include <QtNetwork/QNetworkReply>
#include <QUrl>
#include <QEventLoop>
#include <QTimer>

namespace Helpme {
namespace Internal {

class PostMessager : public QObject
{
    Q_OBJECT
public:
    PostMessager();
    ~PostMessager();
    void postJson(int id,int priority,QString name,QString description,QStringList fileNameList);
    void postFile(QString fileName);
    void postGetID();

    void setConnected(bool connected){m_isConnected = connected;};
    bool isConnected(){return m_isConnected;};
    int id(){return m_id;};
public slots:
    void slotPostGetIDFinished(QNetworkReply*);
    void slotPostJsonFinished(QNetworkReply*);
    void slotPostFileFinished(QNetworkReply*);
    void slotTimeout();

private:
    class QNetworkAccessManager* m_manager = nullptr;
    int m_id;
    QEventLoop m_loop;
    QTimer* m_timer;
    QNetworkReply* m_reply;
    bool m_isConnected;
};
}
}
#endif // PostMessager_H
