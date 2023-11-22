#include "type_description_parser.h"
#include <QDomDocument>
#include <QFile>
#include <explorer/project.h>
#include <explorer/projectnodes.h>
#include <explorer/projecttree.h>
#include <coreplugin/messagemanager.h>

namespace EmosTools
{
TypeParser TypeParser::_instance_type_parse;

TypeParser *TypeParser::getInstance()
{
    return &_instance_type_parse;
}

bool matchNodesType(const ProjectExplorer::Node* n)
{
    if(!n)
        return false;

    if(n->filePath().exists() && !n->filePath().isDir() && n->filePath().fileName() == "type.description")
        return true;

    return false;
}

bool TypeParser::reloadXml()
{
    return reloadXml(filePath());
}

bool TypeParser::reloadXml(const QString &path)
{
    //不用在每次退出的时候去解锁，等到生命周期结束的时候自然会解锁
    QMutexLocker lock(&m_mutex);

    types.typedefs.clear();
    types.structs.clear();
    types.datatypes.clear();
    types.fulldatatypes.clear();
    types.enums.clear();
    types.macros.clear();

    QFile xmlFile(path);
    if(!xmlFile.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    QDomDocument doc;
    doc.setContent(&xmlFile);
    xmlFile.close();
    types = loadFromDocument(&doc);
    return true;
}

tTypeData TypeParser::getTypes()
{
    QMutexLocker lock(&m_mutex);
    return types;
}

QList<tTypedefData> TypeParser::getTypedefs()
{
    QMutexLocker lock(&m_mutex);
    return types.typedefs;
}

QList<tStructData> TypeParser::getStructs()
{
    QMutexLocker lock(&m_mutex);
    return types.structs;
}

QList<tDatatypeData> TypeParser::getDatatypes()
{
    QMutexLocker lock(&m_mutex);
    return types.datatypes;
}

QStringList TypeParser::getFullDatatypes()
{
    QMutexLocker lock(&m_mutex);
    return types.fulldatatypes;
}

QList<tEnumData> TypeParser::getEnums()
{
    QMutexLocker lock(&m_mutex);
    return types.enums;
}

QList<tMacroData> TypeParser::getMacros()
{
    QMutexLocker lock(&m_mutex);
    return types.macros;
}

QString TypeParser::getIncludes()
{
    QMutexLocker lock(&m_mutex);
    return types.includes;
}

tTypeData TypeParser::loadFromDocument(QDomDocument* doc)
{
    tTypeData type;
    //加载基础类型
    QDomNodeList commontypeList = doc->elementsByTagName("datatype");
    for(int i = 0; i < commontypeList.size(); i++)
    {
        QDomElement commontype =commontypeList.at(i).toElement();
        QString name = commontype.attribute("name");
        int size = commontype.attribute("size").toInt();
        tDatatypeData datatype = {name, size};
        type.datatypes.push_back(datatype);
        type.fulldatatypes << name << datatype.name+"*" << "const " + datatype.name;

        if(!(datatype.name.contains("const") || datatype.name.contains("*")))
        {
            //增加const
            type.datatypes.push_back({"const " + datatype.name,datatype.size});
            //增加指针
            type.datatypes.push_back({datatype.name+"*", 64});//先设置64
        }
    }

    QDomNodeList typedefList = doc->elementsByTagName("typedef");
    for(int i = 0; i < typedefList.size(); i++)
    {
        tTypedefData typedefItem;
        typedefItem.name = typedefList.at(i).toElement().attribute("name");
        typedefItem.value = typedefList.at(i).toElement().attribute("value");
        type.typedefs.append(typedefItem);
        if(!type.fulldatatypes.contains(typedefItem.name))
        {
            type.fulldatatypes << typedefItem.name << typedefItem.name + "*" << "const " + typedefItem.name;
        }
    }

    QDomNodeList enumNodesList = doc->elementsByTagName("enum");
    for(int i = 0; i < enumNodesList.size(); i++)
    {
        QDomElement enumNode = enumNodesList.at(i).toElement();
        tEnumData enumitem;
        QList<tEnumInfo> enuminfo;
        enumitem.type = enumNode.attribute("type");
        enumitem.name = enumNode.attribute("name");
        enumitem.filename = enumNode.attribute("filename", "test1");
        type.fulldatatypes.push_back(enumitem.name);
        QDomNodeList enummemberList = enumNode.elementsByTagName("element");
        for(int j = 0; j < enummemberList.size(); j++)
        {
            QDomElement enumMemberNode = enummemberList.at(j).toElement();
            tEnumInfo oneenuminfo;
            oneenuminfo.name = enumMemberNode.attribute("name");
            oneenuminfo.value = enumMemberNode.attribute("value");
            oneenuminfo.description = enumMemberNode.attribute("description", "");
            enuminfo.push_back(oneenuminfo);
        }
        enumitem.enuminfo = enuminfo;
        type.enums.push_back(enumitem);
    }

    QDomNodeList structNodesList = doc->elementsByTagName("struct");
    for(int i = 0; i < structNodesList.size(); i++)
    {
        QDomElement structNode = structNodesList.at(i).toElement();
        tStructData onestruct;
        QList<tStructInfo> structinfo;
        onestruct.name = structNode.attribute("name");
        onestruct.alignment = structNode.attribute("alignment").toInt();
        onestruct.filename = structNode.attribute("filename");
        onestruct.size = structNode.attribute("size").toInt();
        onestruct.version = structNode.attribute("version").toInt();

        //将结构体当成普通类型存入便于校验
        type.fulldatatypes.push_back(onestruct.name);
        type.fulldatatypes.push_back(onestruct.name + "*");
        QDomNodeList structmemberList = structNode.elementsByTagName("element");
        for(int j =0 ; j < structmemberList.size(); j++)
        {
            QDomElement structMemberNode = structmemberList.at(j).toElement();
            tStructInfo onestructinfo;
            onestructinfo.name = structMemberNode.attribute("name");
            onestructinfo.type = structMemberNode.attribute("type");
            onestructinfo.arraysize = structMemberNode.attribute("arraysize", "0");
            onestructinfo.description = structMemberNode.attribute("description", "");
            if(!type.fulldatatypes.contains(onestructinfo.type))
            {
                if(!onestructinfo.type.contains("signed") && !onestructinfo.type.contains("long"))
                {
                    QString errLog = "ERROR occured: " + onestructinfo.type + " is not a known data types!";
                    Core::MessageManager::instance()->writeWithTime(errLog, Utils::ErrorMessageFormat);
                    continue;
                }
            }
            structinfo.push_back(onestructinfo);
        }
        onestruct.structInfo = structinfo;
        type.structs.push_back(onestruct);
    }

    QDomNodeList macroNodesList = doc->elementsByTagName("macro");
    for(int i = 0; i < macroNodesList.size(); i++)
    {
        QDomElement macroNode = macroNodesList.at(i).toElement();
        tMacroData macroData;
        macroData.value = macroNode.attribute("value");
        macroData.name = macroNode.attribute("name");
        macroData.description = macroNode.attribute("description").replace("&quot;", "\"");
        type.macros.push_back(macroData);
    }

    QDomElement includeNode = doc->firstChildElement("emos:ddl").firstChildElement("includes");
    if(includeNode.isNull())
    {
        type.includes = "";
    }
    else
    {
        type.includes = includeNode.attribute("text");
    }
    return type;
}

tTypeData TypeParser::loadFromFile(const QString& fileName)
{
    QDomDocument *doc = new QDomDocument();
    QFile docFile(fileName);
    docFile.open(QIODevice::ReadOnly);
    doc->setContent(&docFile);
    docFile.close();
    return loadFromDocument(doc);
}

TypeParser::TypeParser()
{

}

TypeParser::~TypeParser()
{

}

QString TypeParser::filePath()
{
    const ProjectExplorer::Project *pro = ProjectExplorer::ProjectTree::currentProject();
    if(!pro)
        return "";

    Utils::FilePaths files = pro->files(matchNodesType);
    if(files.size() == 0)
        return "";

    return files.at(0).toString();
}
}
