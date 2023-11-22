#ifndef APIWIZARDPAGE_H
#define APIWIZARDPAGE_H

#include "mergewizardpage.h"
#include "../writer/api_writer.h"

typedef struct ApiComparePair
{
    tSrvService currentSrv;
    tSrvService mergedSrv;
    Chosen choose;
    DataStatus currentStatus;
    DataStatus mergedStatus;

}ApiComparePair;

class ApiWizardPage : public MergeWizardPage
{
    Q_OBJECT
public:
    ApiWizardPage(QWidget* parent = Q_NULLPTR);

    void loadData();
    bool compare();
    void merge(QMap<QString,QPair<tSrvService,DataStatus>> differ1
               ,QMap<QString,QPair<tSrvService,DataStatus>> differ2);
    void addRow(tSrvService currentSrv,tSrvService mergedSrv,DataStatus currentStatus,DataStatus mergedStatus);
    void mergeIntoProject(tSrvService srv, DataStatus status);
    tSrvService findSrv(QList<tSrvService> srvList,QString srvName);

    void initializePage() override;
    //void cleanupPage() override;
    bool validatePage() override;
public slots:
    void slotConflictViewClicked(const QModelIndex& index);
    void slotRadioButtonClicked();

private:
    QMap<QString,QPair<tSrvService,DataStatus>> getConflictList(QList<tSrvService> currentData,QList<tSrvService> baseData
                                                                ,QStringList currentNameList,QStringList baseNameList);
    void showSrvData(tSrvService srv,QStandardItemModel* model);

    QList<tSrvService> m_baseSrvList;
    QStringList m_baseNameList;
    QList<tSrvService> m_currentSrvList;
    QStringList m_currentNameList;
    QList<tSrvService> m_mergedSrvList;
    QStringList m_mergedNameList;
    QMap<QString,ApiComparePair> m_conflictSrvMap;
    QModelIndex m_index;
};

#endif // APIWIZARDPAGE_H
