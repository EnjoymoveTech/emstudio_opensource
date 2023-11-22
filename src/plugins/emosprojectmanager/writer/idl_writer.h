#ifndef IDLWRITER_H
#define IDLWRITER_H

#include <QObject>
#include <QDomDocument>
#include <emostools/cfvparser.h>
#include "../qmakeprojectmanager_global.h"

#define RETURN_IFNOTZERO(result) if(result) return -1;

using namespace EmosTools;
class IdlWriterPrivate;
class EMOSPROJECTMANAGER_EXPORT IdlWriter : public QObject
{
    Q_OBJECT
public:
    static IdlWriter* getInstance();

    QHash<QString, IDLStruct> idlsData();
    IDLStruct getIdlData(const QString&);
    IDLStruct getIdlDataByFilename(const QString&);

    void setPath(const QString&);
    QStringList getIdlsName();

    void addIdl(IDLStruct&);
    void addIdlByFilename(const QString&);

    void clear();
    void removeIdl(const QString& name);

    void replaceIdl(IDLStruct&, const QString& name);
    void renameIdl(const QString& oldName, const QString& newName);

    void writeToXml();
    void reloadXml();
private:
    IdlWriter();
    ~IdlWriter();

    IdlWriterPrivate* d_ptr;
};

#endif
