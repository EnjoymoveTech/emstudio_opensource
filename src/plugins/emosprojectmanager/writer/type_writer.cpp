#include "type_writer.h"

#include <QDir>
#include <QTextStream>
#include <explorer/project.h>
#include <explorer/projecttree.h>

TypeWriter *TypeWriter::getInstance()
{
    static TypeWriter _instance_type_writer;
    return &_instance_type_writer;
}

class TypeWriterPrivate
{
public:
    TypeWriterPrivate()
    {
        QString Path = ProjectExplorer::ProjectTree::currentProject()->projectDirectory().toString();
        typePath = Path + "/1_interface_design/type.description";
        init();
    }

    void init();
    void writeXml();
    template<class T, typename name>
    int find(QList<T>& list, name Name);

    template<class T>
    QStringList nameList(QList<T>& list);

    tTypeData m_types;
    QString typePath;

private:
    void updateDoc();
    void updateDatatypes(QDomElement&);
    void updateStructs(QDomElement&);
    void updateEnums(QDomElement&);
    void updateMacros(QDomElement&);
    void updateTypedefs(QDomElement&);
    void removeAllChild(QDomElement&);
    QDomDocument typeDoc;
};

TypeWriter::TypeWriter():d_ptr(new TypeWriterPrivate())
{

}

TypeWriter::~TypeWriter()
{
    delete d_ptr;
    d_ptr = nullptr;
}

tTypeData TypeWriter::typesData()
{
    return d_ptr->m_types;
}

void TypeWriter::setPath(const QString& path)
{
    d_ptr->typePath = path;
}

QList<tDatatypeData> TypeWriter::datatypesData()
{
    return d_ptr->m_types.datatypes;
}

QList<tStructData> TypeWriter::structsData()
{
    return d_ptr->m_types.structs;
}

QList<tEnumData> TypeWriter::enumsData()
{
    return d_ptr->m_types.enums;
}

QList<tMacroData> TypeWriter::macrosData()
{
    return d_ptr->m_types.macros;
}

QList<tTypedefData> TypeWriter::typedefsData()
{
    return d_ptr->m_types.typedefs;
}

tDatatypeData TypeWriter::getDatatype(QString name)
{
    return d_ptr->m_types.datatypes[d_ptr->find(d_ptr->m_types.datatypes, name)];
}

tStructData TypeWriter::getStruct(QString name)
{
    return d_ptr->m_types.structs[d_ptr->find(d_ptr->m_types.structs, name)];
}

tEnumData TypeWriter::getEnum(QString name)
{
    return d_ptr->m_types.enums[d_ptr->find(d_ptr->m_types.enums, name)];
}

tMacroData TypeWriter::getMacro(QString name)
{
    return d_ptr->m_types.macros[d_ptr->find(d_ptr->m_types.macros, name)];
}

tTypedefData TypeWriter::getTypedef(QString name)
{
    return d_ptr->m_types.typedefs[d_ptr->find(d_ptr->m_types.typedefs, name)];
}

QStringList TypeWriter::datatypesName()
{
    return d_ptr->nameList(d_ptr->m_types.datatypes);
}

QStringList TypeWriter::structsName()
{
    return d_ptr->nameList(d_ptr->m_types.structs);
}

QStringList TypeWriter::enumsName()
{
    return d_ptr->nameList(d_ptr->m_types.enums);
}

QStringList TypeWriter::macrosName()
{
    return d_ptr->nameList(d_ptr->m_types.macros);
}

QStringList TypeWriter::typedefsName()
{
    return d_ptr->nameList(d_ptr->m_types.typedefs);
}

void TypeWriter::addDatatype(tDatatypeData& data)
{
    d_ptr->m_types.datatypes.append(data);
}

void TypeWriter::addStruct(tStructData& data)
{
    d_ptr->m_types.structs.append(data);
}

void TypeWriter::addEnum(tEnumData& data)
{
    d_ptr->m_types.enums.append(data);
}

void TypeWriter::addMacro(tMacroData& data)
{
    d_ptr->m_types.macros.append(data);
}

void TypeWriter::addTypedef(tTypedefData& data)
{
    d_ptr->m_types.typedefs.append(data);
}

void TypeWriter::clear()
{
    d_ptr->m_types.datatypes.clear();
    d_ptr->m_types.structs.clear();
    d_ptr->m_types.enums.clear();
    d_ptr->m_types.macros.clear();
    d_ptr->m_types.typedefs.clear();
}

void TypeWriter::removeDatatype(QString name)
{
    d_ptr->m_types.datatypes.removeAt(d_ptr->find(d_ptr->m_types.datatypes, name));
}

void TypeWriter::removeStruct(QString name)
{
    d_ptr->m_types.structs.removeAt(d_ptr->find(d_ptr->m_types.structs, name));
}

void TypeWriter::removeEnum(QString name)
{
    d_ptr->m_types.enums.removeAt(d_ptr->find(d_ptr->m_types.enums, name));
}

void TypeWriter::removeMacro(QString name)
{
    d_ptr->m_types.macros.removeAt(d_ptr->find(d_ptr->m_types.macros, name));
}

void TypeWriter::removeTypedef(QString name)
{
    d_ptr->m_types.typedefs.removeAt(d_ptr->find(d_ptr->m_types.typedefs, name));
}

void TypeWriter::replaceType(tTypeData& newTypeData)
{
    d_ptr->m_types = newTypeData;
}

void TypeWriter::replaceDatatype(tDatatypeData& newData, QString name)
{
    d_ptr->m_types.datatypes.replace(d_ptr->find(d_ptr->m_types.datatypes, name), newData);
}

void TypeWriter::replaceStruct(tStructData& newData, QString name)
{
    d_ptr->m_types.structs.replace(d_ptr->find(d_ptr->m_types.structs, name), newData);
}

void TypeWriter::replaceEnum(tEnumData& newData, QString name)
{
    d_ptr->m_types.enums.replace(d_ptr->find(d_ptr->m_types.enums, name), newData);
}

void TypeWriter::replaceMacro(tMacroData& newData, QString name)
{
    d_ptr->m_types.macros.replace(d_ptr->find(d_ptr->m_types.macros, name), newData);
}

void TypeWriter::replaceTypedef(tTypedefData& newData, QString name)
{
    d_ptr->m_types.typedefs.replace(d_ptr->find(d_ptr->m_types.typedefs, name), newData);
}

void TypeWriter::writeToXml()
{
    d_ptr->writeXml();
}

void TypeWriter::reloadXml()
{
    d_ptr->init();
}


void TypeWriterPrivate::init()
{
    QFile typefile(typePath);
    typeDoc.clear();
    if(typefile.open(QFile::ReadOnly))
    {
        typeDoc.setContent(&typefile);
        typefile.close();
    }
    TypeParser* typeParser = TypeParser::getInstance();
    typeParser->reloadXml(typePath);
    m_types = typeParser->getTypes();
}

void TypeWriterPrivate::writeXml()
{
    updateDoc();
    if(QFile::exists(typePath)) //如果文件已存在，进行删除
    {
        QFile::remove(typePath);
    }
    QFile file(typePath);
    if(!file.open(QIODevice::ReadWrite))
    {
        return;
    }
    QTextStream stream(&file);
    typeDoc.save(stream, 4, QDomNode::EncodingFromTextStream);
}

template<class T, typename name>
int TypeWriterPrivate::find(QList<T>& list, name Name)
{
    int index = 0;
    for(index; index < list.size(); index++)
    {
        if(list[index].name == Name)
        {
            return index;
        }
    }
    return -1;
}

template<class T>
QStringList TypeWriterPrivate::nameList(QList<T>& list)
{
    QStringList res;
    for(auto& item : list)
    {
        res << item.name;
    }
    return res;
}

void TypeWriterPrivate::updateDoc()
{
    QDomElement rootNode = typeDoc.firstChildElement("emos:ddl");
    QDomElement datatypesNode = rootNode.firstChildElement("datatypes");
    QDomElement structsNode = rootNode.firstChildElement("structs");
    QDomElement enumsNode = rootNode.firstChildElement("enums");
    QDomElement macrosNode = rootNode.firstChildElement("macros");
    QDomElement typedefsNode = rootNode.firstChildElement("typedefs");

    updateDatatypes(datatypesNode);
    updateStructs(structsNode);
    updateEnums(enumsNode);
    updateMacros(macrosNode);
    updateTypedefs(typedefsNode);
}

void TypeWriterPrivate::updateDatatypes(QDomElement& node)
{
    removeAllChild(node);
    QDomElement datatypeNode;
    for(int i = 0; i < m_types.datatypes.size(); i++)
    {
        datatypeNode = typeDoc.createElement("datatype");
        tDatatypeData data = m_types.datatypes.at(i);
        datatypeNode.setAttribute("name", data.name);
        datatypeNode.setAttribute("size", data.size);
        datatypeNode.setAttribute("description", "predefined  " + data.name + " datatype");
        node.appendChild(datatypeNode);
    }
}

void TypeWriterPrivate::updateStructs(QDomElement& node)
{
    removeAllChild(node);
    QDomElement structNode;
    for(int i = 0; i < m_types.structs.size(); i++)
    {
        structNode = typeDoc.createElement("struct");
        tStructData data = m_types.structs.at(i);
        structNode.setAttribute("name", data.name);
        structNode.setAttribute("size", data.size);
        structNode.setAttribute("version", data.version);
        structNode.setAttribute("filename", data.filename);
        structNode.setAttribute("alignment", data.alignment);
        QDomElement memberNode;
        for(int j = 0; j < data.structInfo.size(); j++)
        {
            memberNode = typeDoc.createElement("element");
            tStructInfo member = data.structInfo.at(j);
            memberNode.setAttribute("name", member.name);
            memberNode.setAttribute("type", member.type);
            memberNode.setAttribute("alignment", data.alignment);
            memberNode.setAttribute("byteorder", "LE");
            memberNode.setAttribute("arraysize", member.arraysize);
            memberNode.setAttribute("bytepos", 0);
            memberNode.setAttribute("description", "");
            structNode.appendChild(memberNode);
        }
        node.appendChild(structNode);
    }
}

void TypeWriterPrivate::updateEnums(QDomElement& node)
{
    removeAllChild(node);
    QDomElement enumNode;
    for(int i = 0; i < m_types.enums.size(); i++)
    {
        enumNode = typeDoc.createElement("enum");
        tEnumData data = m_types.enums.at(i);
        enumNode.setAttribute("name", data.name);
        enumNode.setAttribute("filename", data.filename);
        enumNode.setAttribute("type", data.type);
        QDomElement memberNode;
        for(int j = 0; j < data.enuminfo.size(); j++)
        {
            memberNode = typeDoc.createElement("element");
            tEnumInfo member = data.enuminfo.at(j);
            memberNode.setAttribute("name", member.name);
            memberNode.setAttribute("value", member.value);
            memberNode.setAttribute("description", "");
            enumNode.appendChild(memberNode);
        }
        node.appendChild(enumNode);
    }
}

void TypeWriterPrivate::updateMacros(QDomElement& node)
{
    removeAllChild(node);
    QDomElement macroNode;
    for(int i = 0; i < m_types.macros.size(); i++)
    {
        macroNode = typeDoc.createElement("macro");
        tMacroData data = m_types.macros.at(i);
        macroNode.setAttribute("name", data.name);
        macroNode.setAttribute("value", data.value);
        macroNode.setAttribute("description", "");
        node.appendChild(macroNode);
    }
}

void TypeWriterPrivate::updateTypedefs(QDomElement& node)
{
    removeAllChild(node);
    QDomElement typedefNode;
    for(int i = 0; i < m_types.typedefs.size(); i++)
    {
        typedefNode = typeDoc.createElement("typedef");
        tTypedefData data = m_types.typedefs.at(i);
        typedefNode.setAttribute("name", data.name);
        typedefNode.setAttribute("value", data.value);
        node.appendChild(typedefNode);
    }
}

void TypeWriterPrivate::removeAllChild(QDomElement& node)
{
    QDomNodeList childList = node.childNodes();
    for(int i = 0; i < childList.size();)
    {
        node.removeChild(childList.at(0));
    }
}
