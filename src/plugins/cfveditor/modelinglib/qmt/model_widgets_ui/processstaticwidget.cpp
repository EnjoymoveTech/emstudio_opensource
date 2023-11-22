#include "processstaticwidget.h"

#include <QCheckBox>
#include <QToolBox>
#include <QHeaderView>
#include <QTableWidgetItem>
#include "qmt/infrastructure/contextmenuaction.h"
#include "qmt/model/melement.h"

#include <utils/emoswidgets/flattable.h>
#include <utils/utilsicons.h>
#include <utils/stringutils.h>
#include <emostools/srv_parser.h>

#include <QStyleFactory>
#include <QVBoxLayout>
#include "qmt/model/mdiagram.h"
#include "qmt/diagram/dcomponent.h"
#include "qmt/diagram/dassociation.h"
#include "qmt/diagram/ddevice.h"

namespace qmt {

ProcessStaticWidget::ProcessStaticWidget(const DBoundary* dboundary, PropertiesView::MView* m_view)
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0,0,0,0);
    layout->setSpacing(0);

    m_staticConfigCheck = new QCheckBox(this);
    m_staticConfigCheck->setText("StaticConfiguration");
    connect(m_staticConfigCheck, &QCheckBox::clicked, this, &ProcessStaticWidget::onStaticSelectChanged);

    m_staticConfigTable = new EmosWidgets::FlatTable(this);
    m_staticConfigTable->setColumnCount(2);
    m_staticConfigTable->setHorizontalHeaderLabels(QStringList()<<"ServiceName" <<"LocalPort");
    m_staticConfigTable->setHeaderSize(QVector<int>()<< 1 << 1);

    layout->addWidget(m_staticConfigCheck);
    layout->addWidget(m_staticConfigTable);
}

ProcessStaticWidget::~ProcessStaticWidget()
{
}

void ProcessStaticWidget::load(const DBoundary* dboundary, PropertiesView::MView* m_view)
{
    m_staticConfigCheck->setChecked(dboundary->staticSelected());
    m_staticConfigCheck->isChecked()?m_staticConfigTable->show():m_staticConfigTable->hide();

    QMap<QString,EmosTools::tSrvFixedModule> modules = EmosTools::SrvParser::getInstance()->getModules();
    QMapIterator<QString, EmosTools::tSrvFixedModule> itmodule(modules);
    QHash<QString, QString> staticInfo = dboundary->staticInfo();

    QList<endStruct> ends = findAssociations(m_view->mdiagram());
    eventNameList.clear();
    pinNameList.clear();
    QStringList proxServices = findServiceName(dboundary, m_view->mdiagram(), ends, eventNameList, pinNameList);

    for(auto& serviceName : proxServices)
    {
        if(!staticInfo.contains(serviceName))
        {
            int port = m_view->getDefaultProt();
            m_staticConfigTable->appendRowItems(QStringList() << serviceName << QString::number(port));
        }
        else
        {
            QString port = staticInfo.value(serviceName).split(";").at(0);
            m_staticConfigTable->appendRowItems(QStringList() << serviceName << port);
        }
        QTableWidgetItem* item = m_staticConfigTable->item(m_staticConfigTable->rowCount() - 1, 0);
        item->setFlags(item->flags() & ~Qt::ItemIsEditable);
    }
    connect(m_staticConfigTable, &EmosWidgets::FlatTable::itemChanged, this, &ProcessStaticWidget::onStaticTableChanged);
    if(m_staticConfigTable->rowCount())
    {
        onStaticTableChanged(m_staticConfigTable->item(0, 0));
    }
}

void ProcessStaticWidget::onStaticTableChanged(QTableWidgetItem *item)
{
    QHash<QString, QString> propertyInfo;
    for(int row = 0; row < m_staticConfigTable->rowCount(); row++)
    {
        propertyInfo.insert(m_staticConfigTable->item(row, 0)->text(), m_staticConfigTable->item(row, 1)->text() + eventNameList + pinNameList);
    }
    emit dataChanged(propertyInfo);
}

void ProcessStaticWidget::onStaticSelectChanged(bool checked)
{
    checked?m_staticConfigTable->show():m_staticConfigTable->hide();
    emit selectChanged(checked);
}

QList<endStruct> ProcessStaticWidget::findAssociations(MDiagram * mdiagram)
{
    QList<endStruct> ends;
    const QList<DElement *> elementList = mdiagram->diagramElements();
    for(auto &element : elementList)
    {
        DAssociation* association = dynamic_cast<DAssociation*>(element);
        if(association)
        {
            ends.append({association->endBName(), association->endAName()});
            endBNameList.append(association->endBName());
        }
    }
    return ends;
}

QStringList ProcessStaticWidget::findServiceName(const DBoundary *dboundary, MDiagram *mdiagram, QList<endStruct> ends, QString& eventNameList, QString& pinNameList)
{
    QStringList serviceList;
    QStringList AsName;
    const QList<DElement *> elementList = mdiagram->diagramElements();
    for(auto &element : elementList)
    {
        DComponent* component = dynamic_cast<DComponent*>(element);
        if(!component)
            continue;
        if(!endBNameList.contains(component->name()))
            continue;
        QPointF compPoint = component->pos();
        QPointF boundPoint = dboundary->pos();
        QRectF compRect = component->rect();
        QRectF boundRect = dboundary->rect();
        int compTop = compPoint.y() - compRect.height();
        int compLeft = compPoint.x() - compRect.width();
        int compBottom = compPoint.y() + compRect.height();
        int compRight = compPoint.x() + compRect.width();
        int boundTop = boundPoint.y() - boundRect.height();
        int boundLeft = boundPoint.x() - boundRect.width();
        int boundBottom = boundPoint.y() + boundRect.height();
        int boundRight = boundPoint.x() + boundRect.width();
        if(boundTop < compTop &&
                boundLeft < compLeft &&
                boundBottom > compBottom &&
                boundRight > compRight)
        {
            for(int index = 0; index < endBNameList.size(); index++)
            {
                if(endBNameList.at(index) == component->name())
                {
                    AsName.append(ends.at(index).endA);
                }
            }
            for(auto& association : elementList)
            {
                DAssociation* connection = dynamic_cast<DAssociation*>(association);
                if(!connection)
                    continue;
                if(!AsName.contains(connection->endAName()))
                    continue;
                for(auto & compo : elementList)
                {
                    DComponent* comp = dynamic_cast<DComponent*>(compo);
                    if(!comp)
                        continue;
                    if(comp->name() != connection->endAName())
                        continue;
                    compPoint = comp->pos();
                    compRect = comp->rect();
                    compTop = compPoint.y() - compRect.height();
                    compLeft = compPoint.x() - compRect.width();
                    compBottom = compPoint.y() + compRect.height();
                    compRight = compPoint.x() + compRect.width();
                    if(boundTop < compTop &&
                            boundLeft < compLeft &&
                            boundBottom > compBottom &&
                            boundRight > compRight)
                        continue; // skip the component in the same process
                    if(connection->endA().kind() == MAssociationEnd::Pin)
                    {
                        QHash<QString,QString> serviceHash = connection->getPinMap();
                        QHash<QString,QString>::const_iterator iter = serviceHash.constBegin();
                        while(iter != serviceHash.constEnd())
                        {
                            QString serviceName = iter.value().split(" : ").at(0);
                            if(!serviceList.contains(serviceName))
                                serviceList.append(serviceName);
                            pinNameList += ";" + iter.value().split(" : ").at(1);
                            iter++;
                        }
                    }
                    else if(connection->endA().kind() != MAssociationEnd::Event ||
                            connection->getEventMap().size() == 0)
                    {
                        QHash<QString,QString> serviceHash = comp->serviceInfo();
                        QHash<QString,QString>::const_iterator iter = serviceHash.constBegin();
                        while(iter != serviceHash.constEnd())
                        {
                            QString serviceName = iter.key();
                            if(!serviceList.contains(serviceName))
                                serviceList.append(serviceName);
                            iter++;
                        }
                    }
                    else
                    {
                        QHash<QString,QString> eventHash = connection->getEventMap();
                        QHash<QString,QString>::const_iterator iter = eventHash.constBegin();
                        while(iter != eventHash.constEnd())
                        {
                            QString serviceName = iter.value().split(" : ").at(0);
                            if(!serviceList.contains(serviceName))
                                serviceList.append(serviceName);
                            eventNameList += ";" + iter.value().split(" : ").at(1);
                            iter++;
                        }
                    }
                }
            }
        }
    }
    return serviceList;
}

}
