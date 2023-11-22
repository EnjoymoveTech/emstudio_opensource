#ifndef TYPEWRITER_H
#define TYPEWRITER_H

#include <QObject>
#include <QDomDocument>
#include <emostools/type_description_parser.h>
#include "../qmakeprojectmanager_global.h"

#define RETURN_IFNOTZERO(result) if(result) return -1;

using namespace EmosTools;
class TypeWriterPrivate;
class TypeWriter : public QObject
{
    Q_OBJECT
public:
    static TypeWriter* getInstance();
    tTypeData typesData();

    void setPath(const QString&);

    QList<tDatatypeData> datatypesData();
    QList<tStructData> structsData();
    QList<tEnumData> enumsData();
    QList<tMacroData> macrosData();
    QList<tTypedefData> typedefsData();

    tDatatypeData getDatatype(QString name);
    tStructData getStruct(QString name);
    tEnumData getEnum(QString name);
    tMacroData getMacro(QString name);
    tTypedefData getTypedef(QString name);

    QStringList datatypesName();
    QStringList structsName();
    QStringList enumsName();
    QStringList macrosName();
    QStringList typedefsName();

    void addDatatype(tDatatypeData&);
    void addStruct(tStructData&);
    void addEnum(tEnumData&);
    void addMacro(tMacroData&);
    void addTypedef(tTypedefData&);

    void clear();

    void removeDatatype(QString name);
    void removeStruct(QString name);
    void removeEnum(QString name);
    void removeMacro(QString name);
    void removeTypedef(QString name);

    void replaceType(tTypeData&);
    void replaceDatatype(tDatatypeData&, QString);
    void replaceStruct(tStructData&, QString);
    void replaceEnum(tEnumData&, QString);
    void replaceMacro(tMacroData&, QString);
    void replaceTypedef(tTypedefData&, QString);

    void writeToXml();
    void reloadXml();
private:
    TypeWriter();
    ~TypeWriter();

    TypeWriterPrivate* d_ptr;
};

#endif
