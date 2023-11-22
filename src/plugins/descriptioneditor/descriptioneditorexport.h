#ifndef DESCRIPTIONEDITOREXPORT_H
#define DESCRIPTIONEDITOREXPORT_H

#include <QObject>
#include <QDomDocument>
#include "descriptioneditor_global.h"

#define RETURN_IFNOTZERO(result) if(result) return -1;

namespace DescriptionEditor {
namespace Internal {

}

typedef struct paraInfo
{
    QString name;
    QString version = "1";
    QString type;
} paraData;

typedef struct methodInfo
{
    QString name;
    QString id;
    QString ret;
    QList<paraData> paraList;
} methodData;

typedef struct eventInfo
{
    QString name;
    QString data;
    QString id;
    QString groupId;
} eventData;

typedef struct fieldInfo
{
    QString name;
    QString id;
    QString selecttype;
    QString data;
} fieldData;

typedef struct serviceInfo
{
    QString name;
    QString id = "0";
    QString interfaceType;
    QString version = "1";
    QList<methodData> methodList;
    QList<eventData> eventList;
    QList<fieldData> fieldList;
} serviceData;

class DescriptionEditorExportPrivate;
class DESCRIPTIONEDITOR_EXPORT DescriptionEditorExport : public QObject
{
    Q_OBJECT
public:
    static DescriptionEditorExport* getInstance();
    QList<serviceData> servicesData();
    QDomElement servicesNode();
    serviceData getServiceData(QString);
    QDomElement getServiceNode(QString);

    void addService(serviceData&);
    void addService(QDomElement&);

    /// Remove all services from api.description
    void clear();
    /// Remove service by name
    void removeService(QString);

    void replaceService(serviceData&, QString);
    QDomElement toDomElement(serviceData&);
    serviceData toServiceData(QDomElement&);

    void writeToXml();
    void reloadXml();

private:
    DescriptionEditorExport();
    ~DescriptionEditorExport();

    DescriptionEditorExportPrivate* d_ptr;
};

}
#endif // DESCRIPTIONEDITOREXPORT_H
