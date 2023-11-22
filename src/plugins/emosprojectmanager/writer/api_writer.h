#ifndef APIWRITER_H
#define APIWRITER_H

#include <QObject>
#include <QDomDocument>
#include <emostools/api_description_parser.h>
#include "../qmakeprojectmanager_global.h"

#define RETURN_IFNOTZERO(result) if(result) return -1;

using namespace EmosTools;
class ApiWriterPrivate;
class EMOSPROJECTMANAGER_EXPORT ApiWriter : public QObject
{
    Q_OBJECT
public:
    static ApiWriter* getInstance();
    QList<tSrvService> servicesData();

    void setPath(const QString&);

    QDomElement servicesNode();
    tSrvService getServiceData(QString);
    QDomElement getServiceNode(QString);

    QStringList getServicesName();

    void addService(tSrvService&);
    void addService(QDomElement&);

    void clear();
    void removeService(QString);

    void replaceService(tSrvService&, QString);
    QDomElement toDomElement(tSrvService&);
    tSrvService toServiceData(QDomElement&);

    void writeToXml();
    void reloadXml();

private:
    ApiWriter();
    ~ApiWriter();

    ApiWriterPrivate* d_ptr;
};

#endif
