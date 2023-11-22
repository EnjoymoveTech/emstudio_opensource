#ifndef TYPE_DESCRIPTION_PARSER_H
#define TYPE_DESCRIPTION_PARSER_H

#include "emostools_global.h"

#include <QString>
#include <QList>
#include <QMutex>
#include <QMutexLocker>

class QDomDocument;
namespace EmosTools
{

typedef struct tTypedefParsingData
{
    QString name;
    QString value;
    bool operator ==(const tTypedefParsingData& other) const
    {
        if(name != other.name ||
           value != other.value)
        {
            return false;
        }
        return true;
    }
}tTypedefData;

typedef struct tStructParsingInfo{
    QString type;
    QString name;
    QString arraysize;
    QString description;
    bool operator ==(const tStructParsingInfo& other) const
    {
        if(type != other.type ||
           name != other.name ||
           arraysize != other.arraysize)
        {
            return false;
        }
        return true;
    }
}tStructInfo;

typedef struct tStructParsingData{
    QString name;
    quint64 size;
    int version;
    int alignment;
    QString filename;
    QList<tStructInfo> structInfo;
    bool operator ==(const tStructParsingData& other) const
    {
        if(name != other.name ||
           size != other.size ||
           alignment != other.alignment ||
           structInfo != other.structInfo)
        {
            return false;
        }
        return true;
    }
}tStructData;

typedef struct tBasicDatatypeData
{
    QString name;
    int size;//bit
    bool operator ==(const tBasicDatatypeData& other) const
    {
        if(name != other.name ||
           size != other.size)
        {
            return false;
        }
        return true;
    }
}tDatatypeData;

typedef struct tEnumParsingInfo
{
    QString name;
    QString value;
    QString description;
    bool operator ==(const tEnumParsingInfo& other) const
    {
        if(name != other.name ||
           value != other.value)
        {
            return false;
        }
        return true;
    }
}tEnumInfo;

typedef struct tEnumParsingData
{
    QString type;
    QString name;
    QString filename;
    QList<tEnumInfo> enuminfo;
    bool operator ==(const tEnumParsingData& other) const
    {
        if(name != other.name ||
           type != other.type ||
           enuminfo != other.enuminfo)
        {
            return false;
        }
        return true;
    }
}tEnumData;

typedef struct tMacroParsingData
{
    QString name;
    QString value;
    QString description;
    int size = 4;
    bool operator ==(const tMacroParsingData& other) const
    {
        if(name != other.name ||
           value != other.value)
        {
            return false;
        }
        return true;
    }
}tMacroData;

typedef struct tTypeParsingData
{
    QList<tTypedefData> typedefs;
    QList<tStructData> structs;
    QList<tDatatypeData> datatypes;
    QStringList fulldatatypes;
    QList<tEnumData> enums;
    QList<tMacroData> macros;
    QString includes;
    bool operator ==(const tTypeParsingData& other) const
    {
        if(typedefs != other.typedefs ||
           structs != other.structs ||
           datatypes != other.datatypes ||
           fulldatatypes != other.fulldatatypes ||
           enums != other.enums ||
           macros != other.macros)
        {
            return false;
        }
        return true;
    }
    bool isEmpty()
    {
        if(typedefs.isEmpty() &&
            structs.isEmpty() &&
            datatypes.isEmpty() &&
            enums.isEmpty() &&
            macros.isEmpty())
        {
            return true;
        }
        else
        {
            return false;
        }
    }
}tTypeData;

class EMOSTOOLS_EXPORT TypeParser
{

public:
    static TypeParser* getInstance();

    bool reloadXml();
    bool reloadXml(const QString&);
    tTypeData getTypes();
    QList<tTypedefData> getTypedefs();
    QList<tStructData> getStructs();
    QList<tDatatypeData> getDatatypes();
    QStringList getFullDatatypes();
    QList<tEnumData> getEnums();
    QList<tMacroData> getMacros();
    QString getIncludes();

    static tTypeData loadFromDocument(QDomDocument* doc);
    static tTypeData loadFromFile(const QString& fileName);
private:
    TypeParser(const TypeParser&) = delete;
    TypeParser(TypeParser&&) = delete;
    TypeParser& operator=(const TypeParser&) = delete;
    TypeParser& operator=(TypeParser&&) = delete;
private:
    TypeParser();
    ~TypeParser();
    QString filePath();

    static TypeParser _instance_type_parse;
    QMutex m_mutex;
    tTypeData types;
};
}
#endif
