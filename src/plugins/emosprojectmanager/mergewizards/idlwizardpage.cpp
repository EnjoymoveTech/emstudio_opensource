#include "idlwizardpage.h"
#include "../writer/api_writer.h"
#include "../writer/idl_writer.h"
#include "../writer/type_writer.h"
#include <QMessageBox>

IdlWizardPage::IdlWizardPage(QWidget* parent) :
    MergeWizardPage(parent)
{
    setTitle("Idl");
    setSubTitle("conflict:");
    UIInit();
    connect(this,&IdlWizardPage::sigConflictViewClicked,this,&IdlWizardPage::slotConflictViewClicked);
    connect(this,&IdlWizardPage::sigRadioButtonClicked,this,&IdlWizardPage::slotRadioButtonClicked);
}

void IdlWizardPage::loadData()
{
    IdlWriter* writer = IdlWriter::getInstance();

    QString Path = field("mergedProjectPath").toString();
    writer->setPath(Path + "/2_component_design/");
    writer->reloadXml();
    QHash<QString,IDLStruct> mergedHash = writer->idlsData();
    m_mergedIdlList.clear();
    m_mergedNameList = writer->getIdlsName();
    foreach (const QString name,m_mergedNameList)
    {
        m_mergedIdlList.append(mergedHash.value(name));
    }
    //m_mergedIdlList = writer->idlsData();

    Path = field("baseProjectPath").toString();
    writer->setPath(Path + "/2_component_design/");
    writer->reloadXml();
    QHash<QString,IDLStruct> baseHash = writer->idlsData();
    m_baseIdlList.clear();
    m_baseNameList = writer->getIdlsName();
    foreach (const QString name,m_baseNameList)
    {
        m_baseIdlList.append(baseHash.value(name));
    }

    Path = field("currentProjectPath").toString();
    writer->setPath(Path + "/2_component_design/");
    writer->reloadXml();
    QHash<QString,IDLStruct> currentHash = writer->idlsData();
    m_currentIdlList.clear();
    m_currentNameList = writer->getIdlsName();
    foreach (const QString name,m_currentNameList)
    {
        m_currentIdlList.append(currentHash.value(name));
    }
    StringListModel()->removeRows(0,StringListModel()->rowCount());
}

bool IdlWizardPage::compare()
{
    QMap<QString,QPair<IDLStruct,DataStatus>> differ1 = getConflictList(m_currentIdlList,m_baseIdlList,m_currentNameList,m_baseNameList);
    QMap<QString,QPair<IDLStruct,DataStatus>> differ2 = getConflictList(m_mergedIdlList,m_baseIdlList,m_mergedNameList,m_baseNameList);
    merge(differ1,differ2);
    return true;
}

void IdlWizardPage::merge(QMap<QString, QPair<IDLStruct, DataStatus>> differ1, QMap<QString, QPair<IDLStruct, DataStatus>> differ2)
{
    QSet<QString> conflictNameList = differ1.keys().toSet().unite(differ2.keys().toSet());
    foreach (const QString& dataName, conflictNameList)
    {
        QPair<IDLStruct,DataStatus> differ1Data = differ1.value(dataName);
        QPair<IDLStruct,DataStatus> differ2Data = differ2.value(dataName);
        if (differ1Data.first.isEmpty())
        {
            mergeIntoProject(differ2Data.first,differ2Data.second);
        }
        else if (differ2Data.first.isEmpty())
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
                mergeIntoProject(differ2Data.first,differ2Data.second);
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

void IdlWizardPage::addRow(IDLStruct currentSrv,IDLStruct mergedSrv,DataStatus currentStatus,DataStatus mergedStatus)
{
    QString Name = currentSrv.name.isEmpty()?mergedSrv.name:currentSrv.name;
    StringListModel()->insertRow(StringListModel()->rowCount());
    StringListModel()->setData(StringListModel()->index(StringListModel()->rowCount() - 1)
                               ,QVariant("Service:" + Name),Qt::DisplayRole);
    m_conflictIdlMap.insert(Name,IdlComparePair{currentSrv,mergedSrv,Chosen::NO_CHOSEN,currentStatus,mergedStatus});
}

void IdlWizardPage::mergeIntoProject(IDLStruct idl, DataStatus status)
{
    switch(status)
    {
    case STATUS_MODIFY:
    {
        if (m_baseNameList.contains(idl.name))
        {
            m_baseIdlList.replace(m_baseNameList.indexOf(idl.name),idl);
        }
        break;
    }
    case STATUS_ADD:
    {
        m_baseIdlList.append(idl);
        m_baseNameList.append(idl.name);
        break;
    }
    case STATUS_REMOVE:
    {
        if (m_baseNameList.contains(idl.name))
        {
            m_baseIdlList.removeOne(idl);
            m_baseNameList.removeOne(idl.name);
        }
        break;
    }
    default:
        break;
    }
}

void IdlWizardPage::initializePage()
{
    loadData();
    if (!compare())
    {
        qDebug()<<"detected conflicts in type.descreption";
        setSubTitle("conflict:");
    }
}

bool IdlWizardPage::validatePage()
{
    bool isok = true;
    QString errorString;
    foreach (const QString& key,m_conflictIdlMap.keys())
    {
        Chosen choose = m_conflictIdlMap.value(key).choose;
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
        foreach (const QString& key,m_conflictIdlMap.keys())
        {
            Chosen choose = m_conflictIdlMap.value(key).choose;
            if (choose == CURRENT)
            {
                mergeIntoProject(m_conflictIdlMap.value(key).currentIdl,m_conflictIdlMap.value(key).currentStatus);
            }
            else if (choose == MERGED)
            {
                mergeIntoProject(m_conflictIdlMap.value(key).mergedIdl,m_conflictIdlMap.value(key).mergedStatus);
            }
        }
        IdlWriter* writer = IdlWriter::getInstance();
        foreach (const IDLStruct& idl,writer->idlsData())
        {
            writer->removeIdl(idl.name);
        }
        foreach (IDLStruct idl,m_baseIdlList)
        {
            writer->addIdl(idl);
        }

        TypeWriter::getInstance()->writeToXml();
        ApiWriter::getInstance()->writeToXml();
        IdlWriter::getInstance()->writeToXml();
        QMessageBox::information(Q_NULLPTR,"information","merge success!");
    }
    return isok;
}

void IdlWizardPage::slotConflictViewClicked(const QModelIndex& index)
{
    if (index.row() >= 0)
    {
        m_index = index;
        QString srvName = StringListModel()->data(index).toString().split(":").at(1);
        IdlComparePair pair = m_conflictIdlMap.value(srvName);
        showIdlData(pair.currentIdl,CurrentModel());
        showIdlData(pair.mergedIdl,MergedModel());
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

void IdlWizardPage::slotRadioButtonClicked()
{
    if (m_index.row() >= 0)
    {
        QString Name = StringListModel()->data(m_index).toString().split(":").at(1);
        IdlComparePair pair = m_conflictIdlMap.value(Name);
        if (CurrentButton()->isChecked())
        {

            pair.choose = CURRENT;
        }
        else if (MergedButton()->isChecked())
        {
            pair.choose = MERGED;
        }
        m_conflictIdlMap.remove(Name);
        m_conflictIdlMap.insert(Name,pair);
    }
}

QMap<QString,QPair<IDLStruct,DataStatus>> IdlWizardPage::getConflictList(QList<IDLStruct> currentData, QList<IDLStruct> baseData,QStringList currentNameList,QStringList baseNameList)
{
    QMap<QString,QPair<IDLStruct,DataStatus>> conflictsList;

    for(int i = 0;i < currentData.size();i++)
    {
        const IDLStruct data = currentData.at(i);
        if (baseData.contains(data))
        {
            conflictsList.insert(currentNameList.at(i),QPair<IDLStruct,DataStatus>(data,STATUS_EQUAL));
        }
        else if (!baseData.contains(data) && baseNameList.contains(currentNameList.at(i)))
        {
            conflictsList.insert(currentNameList.at(i),QPair<IDLStruct,DataStatus>(data,STATUS_MODIFY));
        }
        else if (!baseData.contains(data) && !baseNameList.contains(currentNameList.at(i)))
        {
            conflictsList.insert(currentNameList.at(i),QPair<IDLStruct,DataStatus>(data,STATUS_ADD));
        }
    }
    for(int i = 0;i < baseData.size();i++)
    {
        const IDLStruct data = baseData.at(i);
        if (!currentData.contains(data) && !currentNameList.contains(baseNameList.at(i)))
        {
            conflictsList.insert(baseNameList.at(i),QPair<IDLStruct,DataStatus>(data,STATUS_REMOVE));
        }
    }
    return conflictsList;
}

void IdlWizardPage::showIdlData(IDLStruct idl, QStandardItemModel *model)
{
    model->clear();

    CurrentModel()->setColumnCount(2);
    MergedModel()->setColumnCount(2);
    CurrentModel()->setHorizontalHeaderLabels(QStringList()<<"object"<<"detail");
    MergedModel()->setHorizontalHeaderLabels(QStringList()<<"object"<<"detail");
    model->setItem(0,0,new QStandardItem(idl.name));
    model->item(0,0)->setChild(0,0,new QStandardItem("oid"));
    model->item(0,0)->setChild(0,1,new QStandardItem(idl.oid));
    model->item(0,0)->setChild(1,0,new QStandardItem("QuoteService"));
    int i = 0;
    foreach (const QString& quotesrvName,idl.quoteService.keys())
    {
        model->item(0,0)->child(1,0)->setChild(i,0,new QStandardItem(quotesrvName));
        i++;
    }
    model->item(0,0)->setChild(2,0,new QStandardItem("Service"));
    i = 0;
    foreach (const QString& srvName,idl.service.keys())
    {
        model->item(0,0)->child(2,0)->setChild(0,0,new QStandardItem(srvName));
        i++;
    }
    model->item(0,0)->setChild(3,0,new QStandardItem("Property"));
    i = 0;
    foreach (const QString& propertyName,idl.property.keys())
    {
        model->item(0,0)->child(3,0)->setChild(i,0,new QStandardItem(propertyName));
        model->item(0,0)->child(3,0)->setChild(i,1,new QStandardItem(idl.property.value(propertyName)));
        i++;
    }
    model->item(0,0)->setChild(4,0,new QStandardItem("Thread"));
    i = 0;
    foreach (const ThreadDataStruct& thread,idl.thread)
    {
        model->item(0,0)->child(4,0)->setChild(i,0,new QStandardItem(thread.name));
        QString threadInfo;
        threadInfo += thread.type + "(" + thread.data + ")";
        model->item(0,0)->child(4,0)->setChild(i,1,new QStandardItem(threadInfo));
        i++;
    }
}

