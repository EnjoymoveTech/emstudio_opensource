#include "formattypes.h"
#include <emostools/type_description_parser.h>
#include <QRegExp>
#include <QString>
#include <math.h>

namespace EmosTools
{
FormatTypes::FormatTypes()
{
    m_MapType.insert("bool", (TypesEnum)sizeof(bool));
    m_MapType.insert("char", (TypesEnum)sizeof(char));
    m_MapType.insert("long", (TypesEnum)sizeof(long));
    m_MapType.insert("longlong", (TypesEnum)sizeof(long long));

    m_MapType.insert("int", (TypesEnum)sizeof(int));
    m_MapType.insert("int8_t", (TypesEnum)sizeof(int8_t));
    m_MapType.insert("int16_t", (TypesEnum)sizeof(int16_t));
    m_MapType.insert("int32_t", (TypesEnum)sizeof(int32_t));
    m_MapType.insert("int64_t", (TypesEnum)sizeof(int64_t));
    m_MapType.insert("size_t", (TypesEnum)sizeof(size_t));

    m_MapType.insert("unsignedint", (TypesEnum)sizeof(unsigned int));
    m_MapType.insert("unsignedlong", (TypesEnum)sizeof(unsigned long));
    m_MapType.insert("uint8_t", (TypesEnum)sizeof(uint8_t));
    m_MapType.insert("uint16_t", (TypesEnum)sizeof(uint16_t));
    m_MapType.insert("uint32_t", (TypesEnum)sizeof(uint32_t));
    m_MapType.insert("uint64_t", (TypesEnum)sizeof(uint64_t));
    m_MapType.insert("size_t", (TypesEnum)sizeof(size_t));

    m_MapType.insert("float", (TypesEnum)sizeof(float));
    m_MapType.insert("float_t", (TypesEnum)sizeof(float_t));
    m_MapType.insert("double", (TypesEnum)sizeof(double));
    m_MapType.insert("double_t", (TypesEnum)sizeof(double_t));

    m_MapType.insert("char*", type_string);

    //emos
    m_MapType.insert("tChar*", type_string);

    m_MapType.insert("tBool", (TypesEnum)sizeof(bool));
    m_MapType.insert("tChar", (TypesEnum)sizeof(char));
    m_MapType.insert("tWChar", (TypesEnum)sizeof(signed short int));

    m_MapType.insert("tUInt", (TypesEnum)sizeof(unsigned int));
    m_MapType.insert("tUInt8", (TypesEnum)sizeof(uint8_t));
    m_MapType.insert("tUInt16", (TypesEnum)sizeof(uint16_t));
    m_MapType.insert("tUInt32", (TypesEnum)sizeof(uint32_t));
    m_MapType.insert("EM_ExitCode", (TypesEnum)sizeof(uint32_t));
    m_MapType.insert("tUInt64", (TypesEnum)sizeof(uint64_t));

    m_MapType.insert("tInt", (TypesEnum)sizeof(int));
    m_MapType.insert("tPid", (TypesEnum)sizeof(int));
    m_MapType.insert("tInt8", (TypesEnum)sizeof(int8_t));
    m_MapType.insert("tInt16", (TypesEnum)sizeof(int16_t));
    m_MapType.insert("tInt32", (TypesEnum)sizeof(int32_t));
    m_MapType.insert("result_t", (TypesEnum)sizeof(int32_t));
    m_MapType.insert("tResult", (TypesEnum)sizeof(int32_t));
    m_MapType.insert("tInt64", (TypesEnum)sizeof(int64_t));
    m_MapType.insert("tSHandle", (TypesEnum)sizeof(int64_t));
    m_MapType.insert("tFileSize", (TypesEnum)sizeof(int64_t));
    m_MapType.insert("tFilePos", (TypesEnum)sizeof(int64_t));

    m_MapType.insert("tFloat", (TypesEnum)sizeof(double));
    m_MapType.insert("tFloat32", (TypesEnum)sizeof(float));
    m_MapType.insert("tFloat64", (TypesEnum)sizeof(double));
    //m_MapType.insert("tFloat128", (TypesEnum)sizeof(long double));

    m_MapType.insert("tSize", (TypesEnum)sizeof(size_t));
    m_MapType.insert("tTimeStamp", (TypesEnum)sizeof(int64_t));
}

QString FormatTypes::formatTypes(const QVector<QString> &types, bool& hasError)
{
    QString format;
    hasError = false;
    TypeParser* typeparser = TypeParser::getInstance();
    bool testflag = typeparser->reloadXml();
    for(QString type : types)
    {
        structRecord = "";
        type.remove(QRegExp("\\s"));

        QString strType = formatType(type);
        if(!strType.isEmpty())
            format += strType + "/";
        else
            hasError = true;

    }
    return format;
}

QString FormatTypes::formatType(const QString& type)
{
    QString format = "";

    if(isArrayType(type))
    {
        format = formatType(getArrayType(type)) + getSymbol(type_array) + QString::number(getArrayNum(type));
    }
    else if(isStructType(type))
    {
        if(structRecord.indexOf(getSymbol(type_struct)) >= 0)
            format = getSymbol(type_struct2) + "(" ;
        else
            format = getSymbol(type_struct) + "(" ;

        structRecord += format;

        for(QString it : getStructFormat(type))
        {
            format += formatType(it);
        }

        format += ")";
    }
    else
    {
        if(m_MapType.contains(type))
        {
            format = getSymbol(m_MapType.value(type));
        }
    }

    return format;
}

bool FormatTypes::isArrayType(const QString &type) const
{
    if(type.indexOf("[") > 0 || type.indexOf("]") > 0)
        return true;
    else
        return false;
}

bool FormatTypes::isStructType(const QString &type) const
{
    TypeParser* typeparser = TypeParser::getInstance();
    QList<tStructData> mlist = typeparser->getStructs();
    for(tStructData s : mlist)
    {
        if(type == s.name)
            return true;
    }

    return false;
}

int FormatTypes::getArrayNum(const QString &type) const
{
    int num = 1;
    QString nowStr = type;
    while(nowStr.length() > 0)
    {
        int leftIndex = nowStr.indexOf("[");
        int rightIndex = nowStr.indexOf("]");

        num *= nowStr.mid(leftIndex + 1, rightIndex - leftIndex - 1).toInt();
        nowStr = type.right(nowStr.length() - rightIndex - 1);
    }

    return num;
}

QString FormatTypes::getArrayType(const QString &type) const
{
    return type.left(type.indexOf("["));
}

QVector<QString> FormatTypes::getStructFormat(const QString &type)
{
    QVector<QString> types;
    TypeParser* typeparser = TypeParser::getInstance();
    QList<tStructData> mlist = typeparser->getStructs();
    for(tStructData s : mlist)
    {
        if(type == s.name)
        {
            for(tStructInfo t : s.structInfo)
            {
                t.type.remove(QRegExp("\\s"));
                types.append(t.type);
            }
        }
    }
    return types;
}

QString FormatTypes::getSymbol(FormatTypes::TypesEnum type) const
{
    switch (type) {
    case type_8bit:
        return "c";
        break;
    case type_16bit:
        return "j";
        break;
    case type_32bit:
        return "i";
        break;
    case type_64bit:
        return "I";
        break;
    case type_string:
        return "s";
        break;
    case type_array:
        return "#";
        break;
    case type_struct:
        return "S";
        break;
    case type_struct2:
        return "$";
        break;
    default:
        break;
    }

    return "";
}
}
