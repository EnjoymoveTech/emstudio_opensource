#include <utils/treemodel.h>
#include "inspectormodel.h"
#include <QDomDocument>
#include <emostools/api_description_parser.h>
#define EMC_SKEL_SERVICE 0
#define EMC_PROX_SERVICE 1
using namespace Idl;

InspectorModel::InspectorModel(QObject *parent)
{
    this->setHeader(QStringList() << "Object" << "Detail");
    m_ServiceRoot = new GroupNode(tr("Service"));
    m_UseServiceRoot = new GroupNode(tr("UseService"));
    m_TheadRoot = new GroupNode(tr("Thread"));
    m_PropertyRoot = new GroupNode(tr("Property"));
    rootItem()->appendChild(m_ServiceRoot);
    rootItem()->appendChild(m_UseServiceRoot);
    rootItem()->appendChild(m_TheadRoot);
    rootItem()->appendChild(m_PropertyRoot);
}

bool InspectorModel::LoadFromDoc(const QDomDocument* dDoc)
{
    m_doc = dDoc;
    updateData();
    return true;
}

void InspectorModel::updateData()
{
    m_ServiceRoot->removeChildren();
    m_UseServiceRoot->removeChildren();
    m_TheadRoot->removeChildren();
    m_PropertyRoot->removeChildren();
    QDomElement deRootNode = m_doc->documentElement();
    //导入service
    QDomNodeList serviceNodeList = deRootNode.elementsByTagName("Service");
    for(int i = 0; i < serviceNodeList.size(); i++)
    {
        QString serviceName = serviceNodeList.at(i).toElement().attribute("name");
        LoadService(serviceName,EMC_SKEL_SERVICE);
    }
    //导入QuoteService
    QDomNodeList useNodeList = deRootNode.elementsByTagName("QuoteService");
    for(int i = 0; i < useNodeList.size(); i++)
    {
        QString useserviceName = useNodeList.at(i).toElement().attribute("name");
        LoadService(useserviceName,EMC_PROX_SERVICE);
    }
    //导入thread
    QDomNodeList threadNodeList = deRootNode.elementsByTagName("Thread");
    for(int i = 0; i < threadNodeList.size(); i++)
    {
        m_TheadRoot->appendChild(new ApiTypeNode(threadNodeList.at(i).toElement().attribute("type"), threadNodeList.at(i).toElement().attribute("name")));
    }
    //导入property
    QDomNodeList propertyNodeList = deRootNode.elementsByTagName("Property");
    for(int i = 0; i < propertyNodeList.size(); i++)
    {
        m_PropertyRoot->appendChild(new ApiTypeNode(propertyNodeList.at(i).toElement().attribute("type"), propertyNodeList.at(i).toElement().attribute("name")));
    }

}

void InspectorModel::LoadService(QString serviceName,int type)
{
    EmosTools::ApiParser::getInstance()->reloadXml();
    Utils::TreeItem *ServiceNode = new GroupNode(serviceName);
    if(type == EMC_SKEL_SERVICE){
        m_ServiceRoot->appendChild(ServiceNode);
    } else {
        m_UseServiceRoot->appendChild(ServiceNode);
    }
    Utils::TreeItem *EventRoot = new GroupNode(tr("Event"));
    Utils::TreeItem *MethodRoot = new GroupNode(tr("Method"));
    Utils::TreeItem *FieldRoot = new GroupNode(tr("Field"));
     Utils::TreeItem *PinRoot = new GroupNode(tr("Pin"));
    ServiceNode->appendChild(EventRoot);
    ServiceNode->appendChild(MethodRoot);
    ServiceNode->appendChild(FieldRoot);
    ServiceNode->appendChild(PinRoot);

    EmosTools::tSrvService tSrv = EmosTools::ApiParser::getInstance()->getService(serviceName);
    //更新event节点
    for(auto it_event = tSrv.events.begin();it_event != tSrv.events.end();it_event++)
    {
        EventRoot->appendChild(new ApiTypeNode(it_event->data, it_event->name));
    }
    //更新method节点
    for(auto it_method = tSrv.functions.begin();it_method != tSrv.functions.end();it_method++){
        QString methodDetail;
        methodDetail.append(it_method->returntype);
        QString paramString = "(";
        for(auto it_param = it_method->params.begin();it_param != it_method->params.end();it_param++){
            paramString.append(it_param->type);
            paramString.append(", ");
        }
        paramString.chop(2);
        paramString.append(")");
        if(!it_method->params.empty()){
            methodDetail += paramString;
        }
        MethodRoot->appendChild(new ApiTypeNode(methodDetail, it_method->name));
    }
    //更新field节点
    for(auto it_field = tSrv.fields.begin();it_field != tSrv.fields.end();it_field++){
        FieldRoot->appendChild(new ApiTypeNode(it_field->data, it_field->name));
    }

    //更新pin节点
    for(auto it_pin = tSrv.pins.begin() ; it_pin != tSrv.pins.end(); it_pin++){
        PinRoot->appendChild(new ApiTypeNode(it_pin->data, it_pin->name));
    }
}
