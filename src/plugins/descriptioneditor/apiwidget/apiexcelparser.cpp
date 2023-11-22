#include "apiexcelparser.h"
#include <emostools/api_description_parser.h>
#include <coreplugin/messagemanager.h>
#include <coreplugin/editormanager/editormanager.h>
#include <utils/mimetypes/mimetype.h>
#include <utils/mimetypes/mimedatabase.h>
#include <explorer/projecttree.h>

using namespace EmosTools;

ApiExcelParser::ApiExcelParser()
{

}

ApiExcelParser::~ApiExcelParser()
{

}

void ApiExcelParser::setData(QString excelName, QDomDocument *doc)
{
    m_xlsx = new QXlsx::Document(excelName);
    m_doc = doc;
    excelRead();
}

bool ApiExcelParser::excelRead()
{
    if(!m_xlsx->selectSheet("PortDetails"))
    {
        QString errLog = "FormatError: Sheet \"PortDetails\" doesn't exist, please check your excel is right!";
        Core::MessageManager::instance()->writeWithTime(errLog, Utils::WarningMessageFormat);
        return false;
    }

    QXlsx::CellRange usedRange = m_xlsx->dimension();
    QHash<QString, int> colsHash = {
        {"PortName", 0},
        {"DataElement", 1},
        {"DataElementType", 2},
        {"Description", 3},
        {"Comment", 4},
    }; // Record the header information of Excel table
    QList<int> usefulCols = {0, 0, 0, 0, 0};
    for(int col = 1; col <= 5; col++)
    {
        QString key = m_xlsx->read(1, col).toString();
        usefulCols[colsHash.value(key)] = col;
    }

    QList<serviceInfo> serviceList;
    QList<portInfomation> portList;
    QDomElement serviceElement;
    int rows = usedRange.rowCount();
    for (int i = 2; i <= rows; i++)
    {
        portInfomation portInfo;
        portInfo.name = m_xlsx->read(i, usefulCols[0]).toString();
        portInfo.dataname = m_xlsx->read(i, usefulCols[1]).toString();
        portInfo.datatype = m_xlsx->read(i, usefulCols[2]).toString();
        portList.append(portInfo);
    }

    if(!m_xlsx->selectSheet("Connections"))
    {
        QString errLog = "FormatError: Sheet \"Connections\" doesn't exist, please check your excel is right!";
        Core::MessageManager::instance()->writeWithTime(errLog, Utils::WarningMessageFormat);
        return false;
    }
    usedRange = m_xlsx->dimension();
    colsHash = {
        {"Type", 0},
        {"SendSWC", 1},
        {"SendPort", 2},
        {"ReceiveSWC", 3},
        {"ReceivePort", 4},
        {"ConnectionType", 5},
        {"Description", 6},
        {"Comment", 7},
    }; // Record the header information of Excel table

    usefulCols = {0, 0, 0, 0, 0, 0, 0 ,0};
    for(int col = 1; col <= 8; col++)
    {
        QString key = m_xlsx->read(1, col).toString();
        usefulCols[colsHash.value(key)] = col;
    }

    QStringList serviceNameList;
    rows = usedRange.rowCount();
    for (int i = 2; i <= rows; i++)
    {
        QString portName = m_xlsx->read(i, usefulCols[2]).toString();
        QString serviceName = m_xlsx->read(i, usefulCols[1]).toString();
        if(serviceNameList.contains(serviceName))
        {
            serviceInfo service = serviceList[serviceNameList.indexOf(serviceName)];
            for(auto &port:portList)
            {
                if(port.name != portName)
                    continue;
                port.category = m_xlsx->read(i, usefulCols[5]).toString();
                if(!service.portList.contains(port))
                    service.portList.append(port);
            }
            serviceList.replace(serviceNameList.indexOf(serviceName), service);
        }
        else
        {
            serviceNameList.append(serviceName);
            serviceInfo service;
            service.name = serviceName;
            for(auto &port:portList)
            {
                if(port.name != portName)
                    continue;
                port.category = m_xlsx->read(i, usefulCols[5]).toString();
                service.portList.append(port);
            }
            serviceList.append(service);
        }
    }

    buildStruct(serviceList);
    Core::MessageManager::instance()->writeWithTime("Import Excel success!", Utils::LogMessageFormat);
    return true;
}

void ApiExcelParser::buildStruct(QList<serviceInfo> &serviceList)
{
    int serviceId = 1;
    QList<tSrvService> serviceStructList;
    for(auto &service : serviceList)
    {
        tSrvService serviceStruct;
        int methodId = 1, eventId= 1, fieldId = 16386, pinId = 8192;
        serviceStruct.name = service.name;
        serviceStruct.id = serviceId++;
        for(auto& port:service.portList)
        {
            if(port.category == "Event")
            {
                tSrvEvent event;
                event.name = port.name;
                event.id = eventId++;
                event.data = port.datatype;
                event.groupid = 1;
                serviceStruct.events.append(event);
            }
            else if(port.category == "Pin")
            {
                tSrvPin pin;
                pin.name = port.name;
                pin.data = port.datatype;
                pin.id = pinId++;
                serviceStruct.pins.append(pin);
            }
            else if(port.category == "Field")
            {
                tSrvField field;
                field.name = port.name;
                field.data = port.datatype;
                field.type = 7;
                field.id = fieldId;
                fieldId += 2;
                serviceStruct.fields.append(field);
            }
            else if(port.category == "Method")
            {
                tSrvFunction method;
                method.name = port.name;
                method.id = methodId++;
                QStringList typeList = port.datatype.split(QRegExp("\\(|,|\\)| "));
                typeList.removeAll("");
                method.returntype = typeList.first();
                for(int i = 1; i < typeList.size(); i++)
                {
                    tParam para;
                    para.direction = Param_In;
                    para.size = 0;
                    para.version = 1;
                    para.type = typeList.at(i);
                    para.name = paraName(typeList.at(i), i - 1);
                    method.params.append(para);
                }
                serviceStruct.functions.append(method);
            }
        }
        serviceStructList.append(serviceStruct);
    }
    Utils::MimeType type = Utils::mimeTypeForName("application/emos.description.xml");
    const void* data = &serviceStructList;
    emit Core::EditorManager::instance()->autoCreateNewFile(type, data, Core::EditorManager::DescriptionType::Api, ProjectExplorer::ProjectTree::currentProject());
}

QString ApiExcelParser::paraName(const QString &type, int index)
{
    QString name = "Para" + QString::number(index);
    if(type.toLower().indexOf("uint8") >= 0)
        name = "u8" + name;
    else if(type.toLower().indexOf("uint16") >= 0)
        name = "u16" + name;
    else if(type.toLower().indexOf("uint32") >= 0)
        name = "u32" + name;
    else if(type.toLower().indexOf("uint64") >= 0)
        name = "u64" + name;
    else if(type.toLower().indexOf("int8") >= 0)
        name = "i8" + name;
    else if(type.toLower().indexOf("int16") >= 0)
        name = "i16" + name;
    else if(type.toLower().indexOf("int32") >= 0)
        name = "i32" + name;
    else if(type.toLower().indexOf("int64") >= 0)
        name = "i64" + name;
    else if(type.toLower().indexOf("float32") >= 0)
        name = "f32" + name;
    else if(type.toLower().indexOf("float64") >= 0)
        name = "f64" + name;
    else
        name = "s" + name;

    return name;
}

QString ApiExcelParser::cellRead(int row, int col, QString name)
{
    QString res = m_xlsx->read(row, col).toString();
    if(res == "")
    {
        QString errLog = "Warning: The " + name + " is empty! In row " + QString::number(row) + ", col " + QString::number(col);
        Core::MessageManager::instance()->writeWithTime(errLog, Utils::WarningMessageFormat);
    }
    return res;
}
