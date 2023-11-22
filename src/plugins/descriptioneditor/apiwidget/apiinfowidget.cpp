#include <utils/treemodel.h>
#include "apiinfowidget.h"
#include <utils/emoswidgets/flattable.h>
#include <QComboBox>
#include <utils/emoswidgets/automulticomobox.h>

InspectorModel::InspectorModel(QHash<QString, EmosWidgets::FlatTable*> tableHash, QObject *parent)
{
    m_tableHash = tableHash;
    this->setHeader(QStringList() << "Object" << "Detail");
}

bool InspectorModel::LoadWidget(QString serviceName)
{
    if(m_ServiceRoot)
        m_ServiceRoot->removeChildren();
    m_ServiceRoot = new GroupNode(serviceName);
    if(rootItem()->hasChildren())
        rootItem()->removeChildAt(0);
    rootItem()->appendChild(m_ServiceRoot);
    LoadService();
    return true;
}

void InspectorModel::LoadService()
{
    //load 4 types table into infoWidget, this function be used at the first time open api.description
    Utils::TreeItem *EventRoot = new GroupNode(tr("Event"));
    Utils::TreeItem *MethodRoot = new GroupNode(tr("Method"));
    Utils::TreeItem *FieldRoot = new GroupNode(tr("Field"));
    Utils::TreeItem *PinRoot = new GroupNode(tr("Pin"));
    m_ServiceRoot->appendChild(EventRoot);
    m_ServiceRoot->appendChild(MethodRoot);
    m_ServiceRoot->appendChild(FieldRoot);
    m_ServiceRoot->appendChild(PinRoot);

    //更新method节点
    EmosWidgets::FlatTable *methodTable = m_tableHash.value("Method");
    for(int row = 0; row < methodTable->rowCount(); row++)
    {
        QString methodDetail = qobject_cast<QComboBox*>(methodTable->cellWidget(row, 2))->currentText();
        QString paramString = qobject_cast<EmosWidgets::AutoMultiComoBox*>(methodTable->cellWidget(row, 3))->currentText();
        if(paramString != "")
        {
            methodDetail += "(" + paramString + ")";
        }
        MethodRoot->appendChild(new ApiTypeNode(methodDetail, methodTable->item(row, 0)->text()));
    }

    //更新event节点
    EmosWidgets::FlatTable *eventTable = m_tableHash.value("Event");
    for(int row = 0; row < eventTable->rowCount(); row++)
    {
        QString dataType = qobject_cast<QComboBox*>(eventTable->cellWidget(row, 2))->currentText();
        EventRoot->appendChild(new ApiTypeNode(dataType, eventTable->item(row, 0)->text()));
    }

    //更新field节点
    EmosWidgets::FlatTable *fieldTable = m_tableHash.value("Field");
    for(int row = 0; row < fieldTable->rowCount(); row++)
    {
        QString dataType = qobject_cast<QComboBox*>(fieldTable->cellWidget(row, 2))->currentText();
        FieldRoot->appendChild(new ApiTypeNode(dataType, fieldTable->item(row, 0)->text()));
    }

    //更新pin节点
    EmosWidgets::FlatTable *pinTable = m_tableHash.value("Pin");
    for(int row = 0; row < pinTable->rowCount(); row++)
    {
        QString dataType = qobject_cast<QComboBox*>(pinTable->cellWidget(row, 2))->currentText();
        PinRoot->appendChild(new ApiTypeNode(dataType, pinTable->item(row, 0)->text()));
    }

}
