#ifndef TYPEWIZARDPAGE_H
#define TYPEWIZARDPAGE_H

#include <QWizardPage>
#include <QMap>
#include <QPair>
#include <QPlainTextEdit>
#include <QRadioButton>
#include "mergewizard.h"
#include "mergewizardpage.h"
#include "../writer/type_writer.h"

typedef struct TypeConfilictList
{
    QMap<QString,QPair<tTypedefData,DataStatus>> typedefs;
    QMap<QString,QPair<tStructData,DataStatus>> structs;
    QMap<QString,QPair<tDatatypeData,DataStatus>> datatypes;
    QMap<QString,QPair<tEnumData,DataStatus>> enums;
    QMap<QString,QPair<tMacroData,DataStatus>> macros;
    bool isEmpty()
    {
        if (typedefs.isEmpty() && structs.isEmpty() && datatypes.isEmpty() && enums.isEmpty() && macros.isEmpty())
        {
            return true;
        }
        return false;
    }
}TypeConfilictList;

typedef enum TypeName
{
    DATATYPE = 0,
    TYPEDEF,
    STRUCT,
    ENUM,
    MACRO
}typeName;

typedef struct CompareText
{
    QString currentText;
    QString mergedText;
    Chosen choose;
    DataStatus currentStatus;
    DataStatus mergedStatus;
}CompareText;


class TypeWizardPage : public MergeWizardPage
{
    Q_OBJECT
public:
    TypeWizardPage(QWidget* parent = Q_NULLPTR);

    tTypeData typeData();
    void loadData();
    //存在冲突时返回false
    bool compare();
    bool isConflict();
    tTypeData BaseData();


    void initializePage() override;
    //void cleanupPage() override;
    bool validatePage() override;
    //bool isComplete() const override;
    //int nextId() const override;
public slots:
    void slotConflictViewClicked(const QModelIndex& index);
    void slotRadioButtonClicked();
private:
    TypeConfilictList getDifferData(tTypeData currentData,tTypeData baseData);
    void merge(TypeConfilictList differ1,TypeConfilictList differ2);

    tDatatypeData findDatatype(tTypeData data,QString name);
    tTypedefData findTypedef(tTypeData data,QString name);
    tStructData findStruct(tTypeData data,QString name);
    tMacroData findMacro(tTypeData data,QString name);
    tEnumData findEnum(tTypeData data,QString name);



    QMap<QString,QPair<tDatatypeData,DataStatus>> getDatatypeConflicts(QList<tDatatypeData> currentData,QList<tDatatypeData> baseData);
    QMap<QString,QPair<tEnumData,DataStatus>> getEnumConflicts(QList<tEnumData> currentData,QList<tEnumData> baseData);
    QMap<QString,QPair<tMacroData,DataStatus>> getMacroConflicts(QList<tMacroData> currentData,QList<tMacroData> baseData);
    QMap<QString,QPair<tStructData,DataStatus>> getStructConflicts(QList<tStructData> currentData,QList<tStructData> baseData);
    QMap<QString,QPair<tTypedefData,DataStatus>> getTypedefConflicts(QList<tTypedefData> currentData,QList<tTypedefData> baseData);

    template<typename T>
    void mergeIntoProject(T data,TypeName type,DataStatus status);

    template<typename T>
    QMap<QString,QPair<T,DataStatus>> getConflicts(QList<T> dataList,QList<T> mergedDataList,QStringList dataNameList,QStringList mergedNameList);

    template<typename T>
    void addRow(T currentData,T mergedData,TypeName type,DataStatus currentStatus,DataStatus mergedStatus);

    template<typename T>
    QString displayText(T data,TypeName type);

    QPlainTextEdit* m_currentEdit;
    QPlainTextEdit* m_mergedEdit;

    tTypeData m_baseTypeData;
    QStringList m_datatypesNameList;
    QStringList m_structsNameList;
    QStringList m_enumsNameList;
    QStringList m_macrosNameList;
    QStringList m_typedefsNameList;

    tTypeData m_currentTypeData;
    tTypeData m_mergedTypeData;
    TypeConfilictList m_conflictList;

    QMap<QString,CompareText> m_conflictTextMap;
    QModelIndex m_index;
};

#endif // TYPEWIZARDPAGE_H
