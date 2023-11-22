#ifndef INJADATABUILD_H
#define INJADATABUILD_H

#include <QDomDocument>
#include <inja/inja.hpp>
#include <emostools/type_description_parser.h>
#include <emostools/api_description_parser.h>
using Json = inja::json;

namespace EmosTools{struct IDLStruct;}

class injaDataBuild
{
public:
    injaDataBuild();
    ~injaDataBuild();
    void setTypeData();
    void setApiData();
    void setIdlData(const EmosTools::IDLStruct& idlStruct);
    void setSingleData(const QString & key, const QVariant &value);
    QString getdata();
    QString getidldata();
private:
    void idlData(Json& data, const EmosTools::IDLStruct& idlData);
    void typeData(Json& data);
    void apiData(Json &data);
    Json serverData(const EmosTools::tSrvService& serviceNode);
    QString includeTextBuild(QString mode);
    void structFileSort();
    void enumFileSort();
    int64_t getTypeSize(const QString& type);

    QList<EmosTools::tStructData> m_struct;
    QList<EmosTools::tEnumData>     m_enums;
    QList<EmosTools::tDatatypeData> m_datatypes;
    QList<EmosTools::tMacroData> m_macros;
    QList<EmosTools::tTypedefData> m_typedefs;
    QString includeText;
    QList<QString> filenameList;
    Json jsondata;
    Json m_idljsonData;
};

#endif // INJADATABUILD_H
