#ifndef IDLWIZARDPAGE_H
#define IDLWIZARDPAGE_H

#include "mergewizardpage.h"
#include "../writer/idl_writer.h"

typedef struct IdlComparePair
{
    IDLStruct currentIdl;
    IDLStruct mergedIdl;
    Chosen choose;
    DataStatus currentStatus;
    DataStatus mergedStatus;
}IdlComparePair;

class IdlWizardPage : public MergeWizardPage
{
    Q_OBJECT
public:
    IdlWizardPage(QWidget* parent = Q_NULLPTR);

    void loadData();
    bool compare();
    void merge(QMap<QString,QPair<IDLStruct,DataStatus>> differ1
               ,QMap<QString,QPair<IDLStruct,DataStatus>> differ2);
    void addRow(IDLStruct currentIdl,IDLStruct mergedIdl,DataStatus currentStatus,DataStatus mergedStatus);
    void mergeIntoProject(IDLStruct idl, DataStatus status);
    IDLStruct findIdl(QList<IDLStruct> idlList,QString idlName);

    void initializePage() override;
    //void cleanupPage() override;
    bool validatePage() override;
public slots:
    void slotConflictViewClicked(const QModelIndex& index);
    void slotRadioButtonClicked();

private:
    QMap<QString,QPair<IDLStruct,DataStatus>> getConflictList(QList<IDLStruct> currentData,QList<IDLStruct> baseData
                                                                ,QStringList currentNameList,QStringList baseNameList);
    void showIdlData(IDLStruct idl,QStandardItemModel* model);

    QList<IDLStruct> m_baseIdlList;
    QStringList m_baseNameList;
    QList<IDLStruct> m_currentIdlList;
    QStringList m_currentNameList;
    QList<IDLStruct> m_mergedIdlList;
    QStringList m_mergedNameList;
    QMap<QString,IdlComparePair> m_conflictIdlMap;
    QModelIndex m_index;
};

#endif // IDLWIZARDPAGE_H
