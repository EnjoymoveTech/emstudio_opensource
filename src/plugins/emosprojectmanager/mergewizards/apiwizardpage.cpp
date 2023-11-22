#include "apiwizardpage.h"
#include "typewizardpage.h"
#include <QMessageBox>

ApiWizardPage::ApiWizardPage(QWidget* parent) :
    MergeWizardPage(parent)
{
    setTitle("Api");
    setSubTitle("conflict:");
    UIInit();
    //setModel();
    connect(this,&ApiWizardPage::sigConflictViewClicked,this,&ApiWizardPage::slotConflictViewClicked);
    connect(this,&ApiWizardPage::sigRadioButtonClicked,this,&ApiWizardPage::slotRadioButtonClicked);
}

void ApiWizardPage::loadData()
{
    ApiWriter* writer = ApiWriter::getInstance();

    QString Path = field("mergedProjectPath").toString();
    writer->setPath(Path + "/1_interface_design/api.description");
    writer->reloadXml();
    m_mergedSrvList = writer->servicesData();
    m_mergedNameList = writer->getServicesName();

    Path = field("baseProjectPath").toString();
    writer->setPath(Path + "/1_interface_design/api.description");
    writer->reloadXml();
    m_baseSrvList = writer->servicesData();
    m_baseNameList = writer->getServicesName();

    Path = field("currentProjectPath").toString();
    writer->setPath(Path + "/1_interface_design/api.description");
    writer->reloadXml();
    m_currentSrvList = writer->servicesData();
    m_currentNameList = writer->getServicesName();
    StringListModel()->removeRows(0,StringListModel()->rowCount());
}

bool ApiWizardPage::compare()
{
    QMap<QString,QPair<tSrvService,DataStatus>> differ1 = getConflictList(m_currentSrvList,m_baseSrvList,m_currentNameList,m_baseNameList);
    QMap<QString,QPair<tSrvService,DataStatus>> differ2 = getConflictList(m_mergedSrvList,m_baseSrvList,m_mergedNameList,m_baseNameList);
    merge(differ1,differ2);
    return true;
}

void ApiWizardPage::merge(QMap<QString, QPair<tSrvService, DataStatus>> differ1, QMap<QString, QPair<tSrvService, DataStatus>> differ2)
{
    QSet<QString> conflictNameList = differ1.keys().toSet().unite(differ2.keys().toSet());
    foreach (const QString& dataName, conflictNameList)
    {
        QPair<tSrvService,DataStatus> differ1Data = differ1.value(dataName);
        QPair<tSrvService,DataStatus> differ2Data = differ2.value(dataName);
        if (differ1Data == QPair<tSrvService,DataStatus>())
        {
            mergeIntoProject(differ2Data.first,differ2Data.second);
        }
        else if (differ2Data == QPair<tSrvService,DataStatus>())
        {
            mergeIntoProject(differ1Data.first,differ1Data.second);
        }
        else if (differ1Data.second == differ2Data.second)
        {
            switch(differ1Data.second)
            {
            case STATUS_ADD:
            case STATUS_MODIFY:
            case STATUS_REMOVE:
            {
                if (differ1Data.first == differ2Data.first)
                {
                    mergeIntoProject(differ1Data.first,differ1Data.second);
                }
                else if (!(differ1Data.first == differ2Data.first))
                {
                    addRow(differ1Data.first,differ2Data.first,differ1Data.second,differ2Data.second);
                }
                break;
            }
            default:
                break;
            }
        }
        else if (differ1Data.second != differ2Data.second)
        {
            if (differ1Data.second == STATUS_EQUAL
                    && differ2Data.second != STATUS_EQUAL)
            {
                mergeIntoProject(differ2Data.first,differ1Data.second);
            }
            else if (differ2Data.second == STATUS_EQUAL
                     && differ1Data.second != STATUS_EQUAL)
            {
                mergeIntoProject(differ1Data.first,differ1Data.second);
            }
            else if (differ1Data.second == STATUS_REMOVE
                     && differ2Data.second == STATUS_MODIFY)
            {
                addRow(differ1Data.first,differ2Data.first,differ1Data.second,differ2Data.second);
            }
            else if (differ1Data.second == STATUS_MODIFY
                     && differ2Data.second == STATUS_REMOVE)
            {
                addRow(differ1Data.first,differ2Data.first,differ1Data.second,differ2Data.second);
            }
        }
    }
}

void ApiWizardPage::addRow(tSrvService currentSrv,tSrvService mergedSrv,DataStatus currentStatus,DataStatus mergedStatus)
{
    QString Name = currentSrv.name.isEmpty()?mergedSrv.name:currentSrv.name;
    StringListModel()->insertRow(StringListModel()->rowCount());
    StringListModel()->setData(StringListModel()->index(StringListModel()->rowCount() - 1)
                               ,QVariant("Service:" + Name),Qt::DisplayRole);
    m_conflictSrvMap.insert(Name,ApiComparePair{currentSrv,mergedSrv,Chosen::NO_CHOSEN,currentStatus,mergedStatus});
}

void ApiWizardPage::mergeIntoProject(tSrvService srv, DataStatus status)
{
    switch(status)
    {
    case STATUS_MODIFY:
    {
        if (m_baseNameList.contains(srv.name))
        {
            m_baseSrvList.replace(m_baseNameList.indexOf(srv.name),srv);
        }
        break;
    }
    case STATUS_ADD:
    {
        m_baseSrvList.append(srv);
        m_baseNameList.append(srv.name);
        break;
    }
    case STATUS_REMOVE:
    {
        if (m_baseNameList.contains(srv.name))
        {
            m_baseSrvList.removeOne(srv);
            m_baseNameList.removeOne(srv.name);
        }
        break;
    }
    default:
        break;
    }
}

tSrvService ApiWizardPage::findSrv(QList<tSrvService> srvList, QString srvName)
{
    tSrvService Srv = tSrvService();
    foreach (const tSrvService& srv,srvList)
    {
        if (srv.name == srvName)
        {
            Srv = srv;
            break;
        }
    }
    return Srv;
}

void ApiWizardPage::initializePage()
{
    loadData();
    if (!compare())
    {
        //qDebug()<<"detected conflicts in type.descreption";
        setSubTitle("conflict:");
    }

}

bool ApiWizardPage::validatePage()
{
    bool isok = true;
    QString errorString;
    foreach (const QString& key,m_conflictSrvMap.keys())
    {
        Chosen choose = m_conflictSrvMap.value(key).choose;
        if (choose == NO_CHOSEN)
        {
            errorString += key + "\n";
            isok = false;
            break;
        }
    }
    if (!isok)
    {
        QMessageBox::critical(this,"critical","There still have conflicts:\n" + errorString);
    }
    else
    {
        foreach (const QString& key,m_conflictSrvMap.keys())
        {
            Chosen choose = m_conflictSrvMap.value(key).choose;
            if (choose == CURRENT)
            {
                mergeIntoProject(m_conflictSrvMap.value(key).currentSrv,m_conflictSrvMap.value(key).currentStatus);
            }
            else if (choose == MERGED)
            {
                mergeIntoProject(m_conflictSrvMap.value(key).mergedSrv,m_conflictSrvMap.value(key).mergedStatus);
            }
        }
        for (int i = 0;i < m_baseSrvList.count();i++)
        {
            tSrvService srv = m_baseSrvList.at(i);
            srv.id = i + 1;
        }
        ApiWriter* writer = ApiWriter::getInstance();
        foreach (const QString& name,writer->getServicesName())
        {
            writer->removeService(name);
        }
        foreach (tSrvService srv,m_baseSrvList)
        {
            writer->addService(srv);
        }
        qDebug()<<"weqe";
    }
    return isok;
}

void ApiWizardPage::slotConflictViewClicked(const QModelIndex& index)
{
    if (index.row() >= 0)
    {
        m_index = index;
        QString srvName = StringListModel()->data(index).toString().split(":").at(1);
        ApiComparePair pair = m_conflictSrvMap.value(srvName);
        showSrvData(pair.currentSrv,CurrentModel());
        showSrvData(pair.mergedSrv,MergedModel());
        switch (pair.choose)
        {
        case NO_CHOSEN:
        {
            CurrentButton()->setAutoExclusive(false);
            CurrentButton()->setChecked(false);
            CurrentButton()->setAutoExclusive(true);
            MergedButton()->setAutoExclusive(false);
            MergedButton()->setChecked(false);
            MergedButton()->setAutoExclusive(true);
            break;
        }
        case CURRENT:
        {
            CurrentButton()->setChecked(true);
        }
        case MERGED:
        {
            MergedButton()->setChecked(true);
        }
        }
    }
}

void ApiWizardPage::slotRadioButtonClicked()
{
    if (m_index.row() >= 0)
    {
        QString Name = StringListModel()->data(m_index).toString().split(":").at(1);
        ApiComparePair pair = m_conflictSrvMap.value(Name);
        if (CurrentButton()->isChecked())
        {
            pair.choose = CURRENT;
        }
        else if (MergedButton()->isChecked())
        {
            pair.choose = MERGED;
        }
        m_conflictSrvMap.remove(Name);
        m_conflictSrvMap.insert(Name,pair);
    }
}

QMap<QString,QPair<tSrvService,DataStatus>> ApiWizardPage::getConflictList(QList<tSrvService> currentData, QList<tSrvService> baseData,QStringList currentNameList,QStringList baseNameList)
{
    QMap<QString,QPair<tSrvService,DataStatus>> conflictsList;

    for(int i = 0;i < currentData.size();i++)
    {
        const tSrvService data = currentData.at(i);
        if (baseData.contains(data))
        {
            conflictsList.insert(currentNameList.at(i),QPair<tSrvService,DataStatus>(data,STATUS_EQUAL));
        }
        else if (!baseData.contains(data) && baseNameList.contains(currentNameList.at(i)))
        {
            conflictsList.insert(currentNameList.at(i),QPair<tSrvService,DataStatus>(data,STATUS_MODIFY));
        }
        else if (!baseData.contains(data) && !baseNameList.contains(currentNameList.at(i)))
        {
            conflictsList.insert(currentNameList.at(i),QPair<tSrvService,DataStatus>(data,STATUS_ADD));
        }
    }
    for(int i = 0;i < baseData.size();i++)
    {
        const tSrvService data = baseData.at(i);
        if (!currentData.contains(data) && !currentNameList.contains(baseNameList.at(i)))
        {
            conflictsList.insert(baseNameList.at(i),QPair<tSrvService,DataStatus>(data,STATUS_REMOVE));
        }
    }
    return conflictsList;
}

void ApiWizardPage::showSrvData(tSrvService srv, QStandardItemModel *model)
{
    model->clear();
    CurrentModel()->setColumnCount(2);
    MergedModel()->setColumnCount(2);
    CurrentModel()->setHorizontalHeaderLabels(QStringList()<<"object"<<"detail");
    MergedModel()->setHorizontalHeaderLabels(QStringList()<<"object"<<"detail");
    model->setItem(0,0,new QStandardItem(srv.name));
    model->item(0,0)->setChild(0,0,new QStandardItem("id"));
    model->item(0,0)->setChild(0,1,new QStandardItem(QString::number(srv.id)));
    model->item(0,0)->setChild(1,0,new QStandardItem("interfaceType"));
    model->item(0,0)->setChild(1,1,new QStandardItem(srv.interfaceType));
    model->item(0,0)->setChild(2,0,new QStandardItem("inProcess"));
    model->item(0,0)->setChild(2,1,new QStandardItem(QVariant(srv.inProcess).toString()));
    model->item(0,0)->setChild(3,0,new QStandardItem("Event"));
    int i = 0;
    foreach (const tSrvEvent& event,srv.events)
    {
        model->item(0,0)->child(3,0)->setChild(i,0,new QStandardItem(event.name));
        model->item(0,0)->child(3,0)->setChild(i,1,new QStandardItem(event.data));
        i++;
    }
    model->item(0,0)->setChild(4,0,new QStandardItem("Method"));
    i = 0;
    foreach (const tSrvFunction& method,srv.functions)
    {
        model->item(0,0)->child(4,0)->setChild(i,0,new QStandardItem(method.name));
        QString methodInfo = method.returntype;
        methodInfo += "(";
        bool isok = false;
        foreach (const tParam& para,method.params)
        {
            isok = true;
            methodInfo += para.type + ",";
        }
        if (isok)
        {
            methodInfo.chop(1);
        }
        methodInfo += ")";
        model->item(0,0)->child(4,0)->setChild(i,1,new QStandardItem(methodInfo));
        i++;
    }
    model->item(0,0)->setChild(5,0,new QStandardItem("Field"));
    i = 0;
    foreach (const tSrvField& field,srv.fields)
    {
        model->item(0,0)->child(5,0)->setChild(i,0,new QStandardItem(field.name));
        model->item(0,0)->child(5,0)->setChild(i,1,new QStandardItem(field.type));
        i++;
    }
    model->item(0,0)->setChild(6,0,new QStandardItem("Pin"));
    i = 0;
    foreach (const tSrvPin& pin,srv.pins)
    {
        model->item(0,0)->child(6,0)->setChild(i,0,new QStandardItem(pin.name));
        model->item(0,0)->child(6,0)->setChild(i,1,new QStandardItem(pin.data));
        i++;
    }
}


