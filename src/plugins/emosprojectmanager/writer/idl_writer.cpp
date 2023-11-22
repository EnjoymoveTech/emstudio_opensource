#include "idl_writer.h"

#include <QDir>
#include <explorer/project.h>
#include <explorer/projecttree.h>

IdlWriter *IdlWriter::getInstance()
{
    static IdlWriter _instance_idl_writer;
    return &_instance_idl_writer;
}

class IdlWriterPrivate
{
public:
    IdlWriterPrivate()
    {
        defaultPath = ProjectExplorer::ProjectTree::currentProject()->projectDirectory().toString() + "/2_component_design/";
        init();
    }

    void init();
    void writeXml(const QString &fileName);
    void removeAllFile();
    void updateDoc();

    QString defaultPath;
    QStringList keyList;
    QStringList fileNameList;
    QHash<QString, IDLStruct> m_idlHash;
    QHash<QString, QDomDocument> idlDocHash;

private:
    void updateServices(QDomDocument*, QMap<QString, int>);
    void updateQuotes(QDomDocument*, QMap<QString, int>);
    void updateThreads(QDomDocument*, QList<ThreadDataStruct>);
    void updateProperties(QDomDocument*, QMap<QString, QString>);
    void removeAllChild(QDomElement&);
};

IdlWriter::IdlWriter():d_ptr(new IdlWriterPrivate())
{

}

IdlWriter::~IdlWriter()
{
    delete d_ptr;
    d_ptr = nullptr;
}

QHash<QString, IDLStruct> IdlWriter::idlsData()
{
    return d_ptr->m_idlHash;
}

IDLStruct IdlWriter::getIdlData(const QString& name)
{
    return d_ptr->m_idlHash.value(name);
}

IDLStruct IdlWriter::getIdlDataByFilename(const QString& file)
{
    return IdlParser::getIdlStructByFile(file);
}

void IdlWriter::setPath(const QString &filePath)
{
    d_ptr->defaultPath = filePath;
}

QStringList IdlWriter::getIdlsName()
{
    return d_ptr->keyList;
}

void IdlWriter::addIdl(IDLStruct& data)
{
    QString name = data.name;
    d_ptr->m_idlHash.insert(name, data);
    d_ptr->keyList.append(name);
    d_ptr->idlDocHash.insert(name, QDomDocument());
    d_ptr->fileNameList.append(d_ptr->defaultPath + name + ".idl");
}

void IdlWriter::addIdlByFilename(const QString& file)
{
    IDLStruct data = IdlParser::getIdlStructByFile(file);
    QString name = data.name;
    d_ptr->m_idlHash.insert(name, data);
    d_ptr->keyList.append(name);
    d_ptr->idlDocHash.insert(name, QDomDocument());
    d_ptr->fileNameList.append(d_ptr->defaultPath + name + ".idl");
}

void IdlWriter::clear()
{
    d_ptr->m_idlHash.clear();
    d_ptr->idlDocHash.clear();
    d_ptr->fileNameList.clear();
    d_ptr->keyList.clear();
}

void IdlWriter::removeIdl(const QString& name)
{
    d_ptr->m_idlHash.remove(name);
    d_ptr->idlDocHash.remove(name);
    d_ptr->keyList.removeOne(name);
    d_ptr->fileNameList.removeOne(d_ptr->defaultPath + name + ".idl");
}

void IdlWriter::replaceIdl(IDLStruct& data, const QString& name)
{
    d_ptr->m_idlHash.insert(name, data);
}

void IdlWriter::renameIdl(const QString &oldName, const QString &newName)
{
    IDLStruct data = d_ptr->m_idlHash.value(oldName);
    data.name = newName;
    d_ptr->m_idlHash.remove(oldName);
    d_ptr->m_idlHash.insert(newName, data);
    d_ptr->idlDocHash.insert(newName, d_ptr->idlDocHash.value(oldName));
    d_ptr->idlDocHash.remove(oldName);
    d_ptr->fileNameList.replace(d_ptr->fileNameList.indexOf(d_ptr->defaultPath + oldName + ".idl"), d_ptr->defaultPath + newName + ".idl");
    d_ptr->keyList.replace(d_ptr->keyList.indexOf(oldName), newName);
}

void IdlWriter::writeToXml()
{
    d_ptr->updateDoc();
    d_ptr->removeAllFile();
    for(auto& fileName : d_ptr->fileNameList)
    {
        d_ptr->writeXml(fileName);
    }
}

void IdlWriter::reloadXml()
{
    d_ptr->init();
}


void IdlWriterPrivate::init()
{
    idlDocHash.clear();
    fileNameList.clear();
    m_idlHash.clear();
    keyList.clear();
    QDir dir(defaultPath);
    QFileInfoList infoList = dir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot);
    QListIterator<QFileInfo> iter(infoList);
    while(iter.hasNext())
    {
        QFileInfo info = iter.next();
        if(info.isFile())
        {
            if(info.fileName().endsWith(".idl"))
            {
                IDLStruct data = IdlParser::getIdlStructByFile(info.filePath());
                m_idlHash.insert(data.name, data);
                keyList.append(data.name);
                fileNameList.append(info.filePath());
                QDomDocument idlDoc = IdlParser::getIdlDocumentByName(data.name);
                idlDocHash.insert(data.name, idlDoc);
            }
        }
    }
}

void IdlWriterPrivate::writeXml(const QString& fileName)
{
    if(QFile::exists(fileName)) //如果文件已存在，进行删除
    {
        QFile::remove(fileName);
    }
    QFile file(fileName);
    if(!file.open(QIODevice::ReadWrite))
    {
        return;
    }
    QTextStream stream(&file);
    QDomDocument doc = idlDocHash.value(keyList.at(fileNameList.indexOf(fileName)));
    doc.save(stream, 4, QDomNode::EncodingFromTextStream);
}

void IdlWriterPrivate::removeAllFile()
{
    QDir dir(defaultPath);
    QFileInfoList infoList = dir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot);
    QListIterator<QFileInfo> iter(infoList);
    while(iter.hasNext())
    {
        QFileInfo info = iter.next();
        if(info.isFile())
        {
            if(info.fileName().endsWith(".idl"))
            {
                QFile::remove(info.filePath());
            }
        }
    }
}

void IdlWriterPrivate::updateDoc()
{
    for(auto& key : keyList)
    {
        QDomDocument doc = idlDocHash.value(key);
        QDomElement rootNode = doc.firstChildElement("emos:idl").toElement();
        if(rootNode.isNull())
        {
            rootNode = doc.createElement("emos:idl");
            doc.appendChild(rootNode);
        }
        removeAllChild(rootNode);
        rootNode.setAttribute("oid",m_idlHash.value(key).oid);
        rootNode.setAttribute("priority",m_idlHash.value(key).priority);
        rootNode.setAttribute("project:name",m_idlHash.value(key).name);

        updateServices(&doc, m_idlHash.value(key).service);
        updateThreads(&doc, m_idlHash.value(key).thread);
        updateProperties(&doc, m_idlHash.value(key).property);
        updateQuotes(&doc, m_idlHash.value(key).quoteService);
        idlDocHash.remove(key);
        idlDocHash.insert(key,doc);
    }
}

void IdlWriterPrivate::updateServices(QDomDocument* doc, QMap<QString, int> services)
{
    QMap<QString, int>::iterator iter = services.begin();
    while(iter != services.end())
    {
        QDomElement serviceNode = doc->createElement("Service");
        doc->firstChildElement("emos:idl").appendChild(serviceNode);
        serviceNode.setAttribute("id", iter.value());
        serviceNode.setAttribute("name", iter.key());
        serviceNode.setAttribute("version", "1");
        iter++;
    }
}

void IdlWriterPrivate::updateQuotes(QDomDocument* doc, QMap<QString, int> quotes)
{
    QMap<QString, int>::iterator iter = quotes.begin();
    while(iter != quotes.end())
    {
        QDomElement quoteNode = doc->createElement("QuoteService");
        doc->firstChildElement("emos:idl").appendChild(quoteNode);
        quoteNode.setAttribute("name", iter.key());
        iter++;
    }
}

void IdlWriterPrivate::updateThreads(QDomDocument* doc, QList<ThreadDataStruct> threads)
{
    for(int i = 0; i < threads.size(); i++)
    {
        QDomElement threadNode = doc->createElement("Thread");
        doc->firstChildElement("emos:idl").appendChild(threadNode);
        threadNode.setAttribute("name", threads.at(i).name);
        threadNode.setAttribute("type", threads.at(i).type);
        threadNode.setAttribute("data", threads.at(i).data);
    }
}

void IdlWriterPrivate::updateProperties(QDomDocument* doc, QMap<QString, QString> properties)
{
    QMap<QString, QString>::iterator iter = properties.begin();
    while(iter != properties.end())
    {
        QDomElement propertyNode = doc->createElement("Property");
        doc->firstChildElement("emos:idl").appendChild(propertyNode);
        propertyNode.setAttribute("name", iter.key());
        propertyNode.setAttribute("type", iter.value());
        iter++;
    }
}


void IdlWriterPrivate::removeAllChild(QDomElement& node)
{
    QDomNodeList childList = node.childNodes();
    for(int i = 0; i < childList.size();)
    {
        node.removeChild(childList.at(0));
    }
}
