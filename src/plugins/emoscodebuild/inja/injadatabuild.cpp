#include "injadatabuild.h"
#include <QFile>
#include <app/app_version.h>
#include <emostools/cfvparser.h>
#include <emostools/type_description_parser.h>
#include <emostools/api_description_parser.h>

using namespace EmosTools;
using namespace Core::Constants;
injaDataBuild::injaDataBuild()
{

}

injaDataBuild::~injaDataBuild()
{

}

void injaDataBuild::setTypeData()
{
    typeData(jsondata);
}

void injaDataBuild::setApiData()
{
    apiData(jsondata);
}

void injaDataBuild::setIdlData(const EmosTools::IDLStruct& idlStruct)
{
    idlData(jsondata, idlStruct);
    //idlData(m_idljsonData, IdlParser::getIdlStructByFile(idlPath));
}
//三个json拼凑而成
QString injaDataBuild::getdata()
{
    return QString::fromStdString(jsondata.dump());
}

QString injaDataBuild::getidldata()
{
    return QString::fromStdString(m_idljsonData.dump());
}

void injaDataBuild::setSingleData(const QString &key, const QVariant &value)
{
    jsondata[key.toStdString()] = value.toString().toStdString();
}

void injaDataBuild::idlData(Json& data, const EmosTools::IDLStruct& idlData)
{
    /***************** idlbuild ******************/
    data["oid"] = idlData.oid.toStdString();
    data["name"] = idlData.name.toStdString();
    data["serviceList"] = Json::array();
    data["version"] = IDE_VERSION_COMPAT;
    QMapIterator<QString, int> serviceIt(idlData.service);
    while (serviceIt.hasNext())
    {
        serviceIt.next();
        tSrvService serviceNode = EmosTools::ApiParser::getInstance()->getService(serviceIt.key());
        if(serviceNode.name == "")  // if the service is disappear, continue
        {
            qDebug() << "Invalid Service:" << serviceIt.key();
            continue;
        }
        data["serviceList"].push_back(serverData(serviceNode));
    }

    data["QuoteServiceList"] = Json::array();
    QMapIterator<QString, int> quoteServiceIt(idlData.quoteService);
    while (quoteServiceIt.hasNext())
    {
        quoteServiceIt.next();
        tSrvService quoteNode = EmosTools::ApiParser::getInstance()->getService(quoteServiceIt.key());
        if(quoteNode.name == "")
        {
            continue;
        }

        Json quote;
        quote["name"] = quoteNode.name.toStdString();
        quote["version"] = quoteNode.version;
        quote["id"] = quoteNode.id;
        data["QuoteServiceList"].push_back(quote);
    }

    data["propertyList"] = Json::array();
    QMapIterator<QString, QString> propertyListIt(idlData.property);
    while (propertyListIt.hasNext())
    {
        propertyListIt.next();

        Json property;
        property["type"] = propertyListIt.value().toStdString();
        property["name"] = propertyListIt.key().toStdString();
        data["propertyList"].push_back(property);
    }

    data["threadList"] = Json::array();
    QList<ThreadDataStruct> threadList = idlData.thread;
    bool flag[3] = {false, false, false};
    for(const auto& threadData : threadList)
    {
        Json thread;
        QString type = threadData.type;
        int bid;
        if(type == "Normal")
        {
            bid = 0;
        }
        else if(type == "Time")
        {
            bid = 1;
        }
        else
        {
            bid = 2;
        }

        if(flag[bid])
        {
            thread["haselse"] = "else ";
        }
        else
        {
            thread["haselse"] = "";
        }
        flag[bid] = true;
        thread["name"] = threadData.name.toStdString();
        thread["data"] = threadData.data.toStdString();
        thread["type"] = type.toStdString();
        data["threadList"].push_back(thread);
    }
}

void injaDataBuild::typeData(Json &data)
{
    TypeParser* m_typeparser = TypeParser::getInstance();
    m_typedefs = m_typeparser->getTypedefs();
    m_struct = m_typeparser->getStructs();
    m_datatypes = m_typeparser->getDatatypes();
    m_enums = m_typeparser->getEnums();
    m_macros = m_typeparser->getMacros();
    includeText = m_typeparser->getIncludes();

    /***************** includebuild ******************/
    data["includes"] = includeText.toStdString();

    /***************** structbuild ******************/
    structFileSort();
    data["structIncludeList"] = includeTextBuild("struct").toStdString();
    data["structList"] = Json::array();
    for(int i = 0; i < m_struct.size(); i++)
    {
        tStructData structItem = m_struct[i];
        Json sut;
        sut["name"] = structItem.name.toStdString();
        sut["alignment"] = structItem.alignment;
        sut["filename"] = structItem.filename.toStdString();
        sut["infoList"] = Json::array();
        for(int j = 0; j < structItem.structInfo.size(); j++)
        {
            tStructInfo structInfoItem = structItem.structInfo[j];
            Json info;
            info["type"] = structInfoItem.type.toStdString();
            info["name"] = structInfoItem.name.toStdString();
            info["arraysize"] = structInfoItem.arraysize.toStdString();
            info["description"] = structInfoItem.description.toStdString();
            sut["infoList"].push_back(info);
        }
        data["structList"].push_back(sut);
    }

    /***************** datatypesbuild ******************/
    data["datatypesList"] = Json::array();
    for(int i = 0; i < m_datatypes.size(); i++)
    {
        tDatatypeData datatype = m_datatypes[i];
        Json dt;
        dt["type"] = datatype.name.toStdString();
        dt["size"] = datatype.size;
        data["datatypesList"].push_back(dt);
    }

    /***************** enumsbuild ******************/
    enumFileSort();
    data["enumIncludeList"] = includeTextBuild("enum").toStdString();
    data["enumsList"] = Json::array();
    for(int i = 0; i < m_enums.size(); i++)
    {
        tEnumData enumData = m_enums[i];
        Json enumItem;
        enumItem["type"] = enumData.type.toStdString();
        enumItem["name"] = enumData.name.toStdString();
        enumItem["filename"] = enumData.filename.toStdString();
        for(int j = 0; j < enumData.enuminfo.size(); j++)
        {
            tEnumInfo enumInfoItem = enumData.enuminfo[j];
            Json info;
            info["name"] = enumInfoItem.name.toStdString();
            info["value"] = enumInfoItem.value.toStdString();
            info["description"] = enumInfoItem.description.toStdString();
            enumItem["infoList"].push_back(info);
        }
        data["enumsList"].push_back(enumItem);
    }

    /***************** macrosbuild ******************/
    data["macrosList"] = Json::array();
    for(int i = 0; i < m_macros.size(); i++)
    {
        tMacroData macroData = m_macros[i];
        Json macroItem;
        macroItem["value"] = macroData.value.toStdString();
        macroItem["name"] = macroData.name.toStdString();
        macroItem["description"] = macroData.description.toStdString();
        data["macrosList"].push_back(macroItem);
    }

    /***************** typedefsbuild ******************/
    data["typedefsList"] = Json::array();
    for(int i = 0; i < m_typedefs.size(); i++)
    {
        tTypedefData typedefData = m_typedefs[i];
        Json typedefItem;
        typedefItem["name"] = typedefData.name.toStdString();
        typedefItem["value"] = typedefData.value.toStdString();
        data["typedefsList"].push_back(typedefItem);
    }
}

void injaDataBuild::structFileSort()
{
    filenameList.clear();
    QSet<QString> filenameSet;
    QHash<QString, QString> filenameHash;
    for(int i = 0; i < m_struct.size(); i++)
    {
        filenameSet.insert(m_struct.at(i).filename);
        filenameHash.insert(m_struct.at(i).name, m_struct.at(i).filename);
    }
    QSet<QString>::iterator iter = filenameSet.begin();
    while(iter != filenameSet.end())
    {
        filenameList.append(*iter);
        iter++;
    }
    for(int i = 0; i < m_struct.size(); i++)
    {
        for(int j = 0; j < m_struct.at(i).structInfo.size(); j++)
        {
            QString dependentFile = filenameHash.value(m_struct.at(i).structInfo.at(j).type, "");
            if(dependentFile != "")
            {
                if(filenameList.indexOf(dependentFile) > filenameList.indexOf(m_struct.at(i).filename))
                {
                    filenameList.swap(filenameList.indexOf(dependentFile), filenameList.indexOf(m_struct.at(i).filename));
                }
            }
        }
    }
}

void injaDataBuild::enumFileSort()
{
    filenameList.clear();
    QSet<QString> filenameSet;
    QHash<QString, QString> filenameHash;
    for(int i = 0; i < m_enums.size(); i++)
    {
        filenameSet.insert(m_enums.at(i).filename);
        filenameHash.insert(m_enums.at(i).name, m_enums.at(i).filename);
    }
    QSet<QString>::iterator iter = filenameSet.begin();
    while(iter != filenameSet.end())
    {
        filenameList.append(*iter);
        iter++;
    }
    for(int i = 0; i < m_enums.size(); i++)
    {
        for(int j = 0; j < m_enums.at(i).enuminfo.size(); j++)
        {
            QString dependentFile = filenameHash.value(m_enums.at(i).enuminfo.at(j).value, "");
            if(dependentFile != "")
            {
                if(filenameList.indexOf(dependentFile) > filenameList.indexOf(m_enums.at(i).filename))
                {
                    filenameList.swap(filenameList.indexOf(dependentFile), filenameList.indexOf(m_enums.at(i).filename));
                }
            }
        }
    }
}

int64_t injaDataBuild::getTypeSize(const QString &type)
{
    QString type_t = type;
    type_t.replace("const", "");
    type_t.replace("*","");
    type_t.remove(QRegExp("\\s"));

    int size = 0;
    for(const auto& d : m_datatypes)
    {
        if(type_t.indexOf(d.name) >= 0)
        {
            size = d.size/8;
            break;
        }
    }

    for(const auto& e : m_enums)
    {
        if(type_t.indexOf(e.name) >= 0)
        {
            size =  4;//暂定4字节
            break;
        }
    }

    for(const auto& s : m_struct)
    {
        if(type_t.indexOf(s.name) >= 0)
        {
            size = s.size;
            break;
        }
    }

    if(type_t.indexOf("[") >= 0 && type_t.indexOf("]") >= 0)
    {
        int i = 0;
        bool start = false;
        bool end = true;
        QString num = "";
        while(i < type_t.size())
        {
            if(end && type_t.at(i) == '[')
            {
                start = true;
                end = false;
                num = "";
            }
            else if(start && type_t.at(i) == ']')
            {
                start = false;
                end = true;
                size *= num.toInt();
            }
            else if(start && !end)
            {
                num += type_t.at(i);
            }

            i++;
        }
    }

    return size;
}

QString injaDataBuild::includeTextBuild(QString mode)
{
    QString includeText = "";
    if(mode == "struct")
    {
        includeText = "macro_common/enum_common/";
    }
    for(int i = 0; i < filenameList.size(); i++)
    {
        if(filenameList.at(i) != "common")
        {
            includeText += mode + "_" + filenameList.at(i) + "/";
        }
    }
    return includeText;
}

void injaDataBuild::apiData(Json &data)
{
    QList<tSrvService> services = EmosTools::ApiParser::getInstance()->getServices();

    data["apiList"] = Json::array();
    for(int i = 0; i < services.size(); ++i)
    {
        tSrvService serviceNode = services.at(i);
        data["apiList"].push_back(serverData(serviceNode));
    }
}

Json injaDataBuild::serverData(const EmosTools::tSrvService &serviceNode)
{
    Json service;
    service["name"] = serviceNode.name.toStdString();
    service["version"] = serviceNode.version;
    service["id"] = serviceNode.id;
    service["interfaceType"] = serviceNode.interfaceType.toStdString();
    service["inprocess"] = serviceNode.inProcess;

    //method
    QList<tSrvFunction> methodList = serviceNode.functions;
    service["methodList"] = Json::array();
    for(int j = 0; j < methodList.size(); ++j)
    {
        tSrvFunction methodNode = methodList.at(j);
        Json method;
        method["name"] = methodNode.name.toStdString();
        method["id"] = methodNode.id;
        method["ret"] = methodNode.returntype.toStdString();
        method["ret_size"] = getTypeSize(methodNode.returntype);

        QList<tParam> paraList = methodNode.params;
        method["paraList"] = Json::array();
        for(int k = 0; k < paraList.size(); ++k)
        {
            tParam paraNode = paraList.at(k);
            Json para;
            para["name"] = paraNode.name.toStdString();

            //检测数组，拆分成两部分
            QString typeString = paraNode.type;
            if(typeString.indexOf("[") >= 0 || typeString.indexOf("]") >= 0)
            {
                para["type"] = typeString.left(typeString.indexOf("[")).toStdString();
                para["array"] = typeString.right(typeString.size() - typeString.indexOf("[")).toStdString();
            }
            else
            {
                para["type"] = typeString.toStdString();
                para["array"] = "";
            }

            para["size"] = getTypeSize(paraNode.type);

            //void* 表示一段数据，由界面设置
            if(paraNode.type.indexOf("void*") >= 0 || paraNode.type.indexOf("tVoid*") >= 0)
                para["size"] = paraNode.size;

            auto direction2String = [](EmosTools::tParamDirection direction)
            {
                switch(direction)
                {
                case EmosTools::Param_In:
                        return "in";
                case EmosTools::Param_Out:
                        return "out";
                case EmosTools::Param_InOut:
                        return "inout";
                }
                return "in";
            };
            para["direction"] = direction2String(paraNode.direction);

            method["paraList"].push_back(para);
        }
        service["methodList"].push_back(method);
    }

    //event
    QSet<int> srvGroupId;
    QList<tSrvEvent> eventList = serviceNode.events;
    service["eventList"] = Json::array();
    for(int j = 0; j < eventList.size(); ++j)
    {
        tSrvEvent eventNode = eventList.at(j);
        Json event;
        event["name"] = eventNode.name.toStdString();
        event["data"] = eventNode.data.toStdString();
        event["id"] = eventNode.id;
        event["groupid"] = eventNode.groupid;
        service["eventList"].push_back(event);

        srvGroupId.insert(eventNode.groupid);
    }

    //eventgroup
    service["eventgroupList"] = Json::array();
    for(const auto& g : srvGroupId)
    {
        Json eventgroup;
        eventgroup["value"] = g;
        eventgroup["id"] = g;
        service["eventgroupList"].push_back(eventgroup);
    }

    //field
    QList<tSrvField> fieldList = serviceNode.fields;
    service["fieldList"] = Json::array();
    for(int i = 0; i < fieldList.size(); ++i)
    {
        tSrvField fieldNode = fieldList.at(i);
        Json field;
        field["name"] = fieldNode.name.toStdString();
        field["id"] = fieldNode.id;
        field["data"] = fieldNode.data.toStdString();
        field["type"] = fieldNode.type;
        service["fieldList"].push_back(field);
    }

    //pin
    QList<tSrvPin> pinList = serviceNode.pins;
    service["pinList"] = Json::array();
    for(int i = 0; i < pinList.size(); ++i)
    {
        tSrvPin pinNode = pinList.at(i);
        Json pin;
        pin["name"] = pinNode.name.toStdString();
        pin["id"] = pinNode.id;
        pin["data"] = pinNode.data.toStdString();
        service["pinList"].push_back(pin);
    }

    return service;
}
