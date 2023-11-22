#ifndef FORMATTYPES_H
#define FORMATTYPES_H

#include "emostools_global.h"

#include <QString>
#include <QVector>
#include <QMap>

namespace EmosTools
{
class EMOSTOOLS_EXPORT FormatTypes
{
public:
    FormatTypes();

    enum TypesEnum
    {
        type_8bit = 1,
        type_16bit = 2,
        type_32bit = 4,
        type_64bit = 8,
        type_string,
        type_array,
        type_struct,
        type_struct2,//二级以上的结构体符号
    };

    QString formatTypes(const QVector<QString> &types, bool& hasError);
    QString formatType(const QString& type);
private:
    bool isArrayType(const QString& type) const;
    bool isStructType(const QString& type) const;
    int getArrayNum(const QString& type) const;
    QString getArrayType(const QString& type) const;
    QVector<QString> getStructFormat(const QString& type);
    QString getSymbol(TypesEnum type) const;

    QMap<QString, TypesEnum> m_MapType;
    QString structRecord = "";
};
}
#endif // FORMATTYPES_H
