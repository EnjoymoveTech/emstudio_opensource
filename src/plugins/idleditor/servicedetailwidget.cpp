#include <emostools/api_description_parser.h>
#include "idleditor.h"
#include "idldocument.h"
#include "subwindows/tabwidget/datahandle.h"
#include "servicedetailwidget.h"

namespace Idl {
namespace Internal {

ServiceDetailWidget::ServiceDetailWidget(IdlEditor* IdlEditor,INTERFACETYPE kind,QWidget* parent):
    EmosWidgets::FlatTable(parent),
    m_editor(IdlEditor),
    m_INTERFACETYPE(kind)
{
    this->setEditTriggers(QAbstractItemView::NoEditTriggers);
    switch(m_INTERFACETYPE)
    {
    case MethodType :
        this->setColumnCount(6);
        this->setHorizontalHeaderLabels(QStringList()<< "Serivce"<< "ID" << "Name"<< "Ret"<< "Para"<< "ParaName");
        break;
    case EventType :
        this->setColumnCount(5);
        this->setHorizontalHeaderLabels(QStringList()<< "Service"<< "ID" << "Name"<< "EventData"<< "GroupId");
        break;
    case FieldType :
        this->setColumnCount(5);
        this->setHorizontalHeaderLabels(QStringList()<< "Service"<< "ID" << "Name"<< "Data" << "Type");
        break;
    case PinType:
        this->setColumnCount(4);
        this->setHorizontalHeaderLabels(QStringList()<< "Service"<< "ID" << "Name"<< "Data");
        break;
    default :
        return;
    }
}

void ServiceDetailWidget::loadFromApi()
{
    this->restoreControl();
    QStringList dataList;
    QStringList serviceNameList;

    QDomDocument* doc = qobject_cast<IdlDocument*>(m_editor->document())->getRootDoc();
    serviceNameList = Datahandle::getServiceList(doc);
    QList<EmosTools::tSrvService> services = EmosTools::ApiParser::getInstance()->getServices();
    QList<EmosTools::tSrvService> selectServices;
    for(int i = 0; i < serviceNameList.size(); i++){
        for(int s = 0; s < services.size(); s++){
            if(serviceNameList[i] == services[s].name){
                selectServices.push_back(services[s]);
                break;
            }
        }
    }
    //service的遍历
    for(int s = 0; s < selectServices.size(); s++){
        switch(m_INTERFACETYPE)
        {
        case MethodType :
            for(int i = 0;i < selectServices[s].functions.size(); i++){
                dataList.clear();
                QString strPara;
                QString strParaName;
                for(int paraIdx = 0; paraIdx < selectServices[s].functions[i].params.size(); paraIdx++)
                {
                    if(0 != paraIdx)
                    {
                        strPara += ",";
                        strParaName += ",";
                    }
                    strPara += selectServices[s].functions[i].params[paraIdx].type;
                    strParaName += selectServices[s].functions[i].params[paraIdx].name;
                }
                dataList << selectServices[s].name << QString::number((selectServices[s].functions[i]).id)
                         << (selectServices[s].functions[i]).name << (selectServices[s].functions[i]).returntype
                         << strPara << strParaName;
                this->appendRowItems(dataList);
            }
            break;
        case EventType :
            for(int i = 0; i< selectServices[s].events.size(); i++){
                dataList.clear();
                dataList << selectServices[s].name << QString::number((selectServices[s].events[i]).id)
                         << (selectServices[s].events[i]).name << (selectServices[s].events[i]).data
                         << QString::number(selectServices[s].events[i].groupid);
                this->appendRowItems(dataList);
            }
            break;
        case FieldType :
            for(int i = 0; i< selectServices[s].fields.size(); i++){
                dataList.clear();
                dataList << selectServices[s].name << QString::number(selectServices[s].fields[i].id)
                         << selectServices[s].fields[i].name << selectServices[s].fields[i].data
                         << QString::number(selectServices[s].fields[i].type);
                this->appendRowItems(dataList);
            }
            break;
        case PinType:
            for(int i = 0; i< selectServices[s].pins.size(); i++){
                dataList.clear();
                dataList << selectServices[s].name << QString::number(selectServices[s].pins[i].id)
                         << selectServices[s].pins[i].name << selectServices[s].pins[i].data;
                this->appendRowItems(dataList);
            }
            break;
        default :
            return;
        }
    }

    if(this->rowCount() > 0){
        this->selectRow(0);
    }
}

void ServiceDetailWidget::restoreControl()
{
    this->clearContents();
    this->setRowCount(0);
}

}
}
