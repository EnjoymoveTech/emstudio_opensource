#include "typewizardpage.h"
#include <explorer/project.h>
#include "explorer/projecttree.h"

#include <QMessageBox>
#include <QVBoxLayout>

TypeWizardPage::TypeWizardPage(QWidget* parent) :
    MergeWizardPage(parent)
{
    setTitle("Type");
    setSubTitle("conflict:");

    m_currentEdit = new QPlainTextEdit(parent);
    m_mergedEdit = new QPlainTextEdit(parent);

    m_currentEdit->setMaximumWidth(500);
    m_currentEdit->setMinimumWidth(500);
    m_mergedEdit->setMaximumWidth(500);
    m_mergedEdit->setMinimumWidth(500);

    m_currentEdit->setEnabled(false);
    m_mergedEdit->setEnabled(false);

    QVBoxLayout* currentlayout = new QVBoxLayout;
    currentlayout->addWidget(CurrentButton());
    currentlayout->addWidget(m_currentEdit);
    ((QHBoxLayout*)layout())->addLayout(currentlayout);

    ((QHBoxLayout*)layout())->addStretch(1);

    QVBoxLayout* mergedlayout = new QVBoxLayout;
    mergedlayout->addWidget(MergedButton());
    mergedlayout->addWidget(m_mergedEdit);
    ((QHBoxLayout*)layout())->addLayout(mergedlayout);

    CurrentButton()->show();
    MergedButton()->show();

    connect(ConflictView(),&QListView::clicked,this,&TypeWizardPage::slotConflictViewClicked);
    connect(CurrentButton(),&QRadioButton::clicked,this,&TypeWizardPage::slotRadioButtonClicked);
    connect(MergedButton(),&QRadioButton::clicked,this,&TypeWizardPage::slotRadioButtonClicked);
}

tTypeData TypeWizardPage::typeData()
{
    return m_currentTypeData;
}

void TypeWizardPage::loadData()
{
    TypeWriter* writer = TypeWriter::getInstance();

    QString Path = field("mergedProjectPath").toString();
    writer->setPath(Path + "/1_interface_design/type.description");
    writer->reloadXml();
    m_mergedTypeData = writer->typesData();

    Path = field("baseProjectPath").toString();
    writer->setPath(Path + "/1_interface_design/type.description");
    writer->reloadXml();
    m_baseTypeData = writer->typesData();
    m_datatypesNameList = writer->datatypesName();
    m_structsNameList = writer->structsName();
    m_enumsNameList = writer->enumsName();
    m_macrosNameList = writer->macrosName();
    m_typedefsNameList = writer->typedefsName();

    Path = field("currentProjectPath").toString();
    writer->setPath(Path + "/1_interface_design/type.description");
    writer->reloadXml();
    m_currentTypeData = writer->typesData();

    StringListModel()->removeRows(0,StringListModel()->rowCount());
}

bool TypeWizardPage::compare()
{
    TypeConfilictList differ1 = getDifferData(m_currentTypeData,m_baseTypeData);
    TypeConfilictList differ2 = getDifferData(m_mergedTypeData,m_baseTypeData);
    merge(differ1,differ2);
    //m_conflictList.datatypes = getDatatypeConflicts();
    //m_conflictList.enums = getEnumConflicts();
    //m_conflictList.macros = getMacroConflicts();
    //m_conflictList.structs = getStructConflicts();
    //m_conflictList.typedefs = getTypedefConflicts();
    return m_conflictList.isEmpty();
}

bool TypeWizardPage::isConflict()
{
    return m_conflictList.isEmpty();
}

tTypeData TypeWizardPage::BaseData()
{
    return m_baseTypeData;
}

void TypeWizardPage::initializePage()
{
    loadData();
    if (!compare())
    {
        //qDebug()<<"detected conflicts in type.descreption";
        setSubTitle("conflict:");
    }

}

bool TypeWizardPage::validatePage()
{
    bool isok = true;
    QString errorString;
    foreach (const QString& key,m_conflictTextMap.keys())
    {
        Chosen choose = m_conflictTextMap.value(key).choose;
        if (choose == NO_CHOSEN)
        {
            errorString += key + "\n";
            isok = false;
        }
    }
    if (!isok)
    {
        QMessageBox::critical(this,"critical","There still have conflicts:\n" + errorString);
    }
    else
    {
        foreach (const QString& key,m_conflictTextMap.keys())
        {
            QString type = key.split(":").at(0);
            QString name = key.split(":").at(1);
            Chosen choose = m_conflictTextMap.value(key).choose;
            if (type == "Datatype" && choose == CURRENT)
            {
                mergeIntoProject(findDatatype(m_currentTypeData,name),DATATYPE,m_conflictTextMap.value(key).currentStatus);
            }
            else if (type == "Datatype" && choose == MERGED)
            {
                mergeIntoProject(findDatatype(m_mergedTypeData,name),DATATYPE,m_conflictTextMap.value(key).mergedStatus);
            }
            if (type == "Typedef" && choose == CURRENT)
            {
                mergeIntoProject(findTypedef(m_currentTypeData,name),TYPEDEF,m_conflictTextMap.value(key).currentStatus);
            }
            else if (type == "Typedef" && choose == MERGED)
            {
                mergeIntoProject(findTypedef(m_mergedTypeData,name),TYPEDEF,m_conflictTextMap.value(key).mergedStatus);
            }
            if (type == "Struct" && choose == CURRENT)
            {
                mergeIntoProject(findStruct(m_currentTypeData,name),STRUCT,m_conflictTextMap.value(key).currentStatus);
            }
            else if (type == "Struct" && choose == MERGED)
            {
                mergeIntoProject(findStruct(m_mergedTypeData,name),STRUCT,m_conflictTextMap.value(key).mergedStatus);
            }
            if (type == "Macro" && choose == CURRENT)
            {
                mergeIntoProject(findMacro(m_currentTypeData,name),MACRO,m_conflictTextMap.value(key).currentStatus);
            }
            else if (type == "Macro" && choose == MERGED)
            {
                mergeIntoProject(findMacro(m_mergedTypeData,name),MACRO,m_conflictTextMap.value(key).mergedStatus);
            }
            if (type == "Enum" && choose == CURRENT)
            {
                mergeIntoProject(findEnum(m_currentTypeData,name),ENUM,m_conflictTextMap.value(key).currentStatus);
            }
            else if (type == "Enum" && choose == MERGED)
            {
                mergeIntoProject(findEnum(m_mergedTypeData,name),ENUM,m_conflictTextMap.value(key).mergedStatus);
            }
        }
        TypeWriter::getInstance()->replaceType(m_baseTypeData);
    }
    return isok;
}

void TypeWizardPage::slotConflictViewClicked(const QModelIndex& index)
{
    if (index.row() >= 0)
    {
        m_index = index;
        QString Name = StringListModel()->data(index).toString();
        QString currentText = m_conflictTextMap.value(Name).currentText;
        m_currentEdit->setPlainText(currentText);
        QString mergedText = m_conflictTextMap.value(Name).mergedText;
        m_mergedEdit->setPlainText(mergedText);
        switch (m_conflictTextMap.value(Name).choose)
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
            break;
        }
        case MERGED:
        {
            MergedButton()->setChecked(true);
            break;
        }
        }
    }
}

void TypeWizardPage::slotRadioButtonClicked()
{
    if (m_index.row() >= 0)
    {
        QString Name = StringListModel()->data(m_index).toString();
        CompareText text = m_conflictTextMap.value(Name);
        if (CurrentButton()->isChecked())
        {
            text.choose = CURRENT;
        }
        else if (MergedButton()->isChecked())
        {
            text.choose = MERGED;
        }
        m_conflictTextMap.remove(Name);
        m_conflictTextMap.insert(Name,text);
    }
}

TypeConfilictList TypeWizardPage::getDifferData(tTypeData currentData, tTypeData baseData)
{
    TypeConfilictList conflictlist;
    conflictlist.datatypes = getDatatypeConflicts(currentData.datatypes,baseData.datatypes);
    conflictlist.enums = getEnumConflicts(currentData.enums,baseData.enums);
    conflictlist.macros = getMacroConflicts(currentData.macros,baseData.macros);
    conflictlist.structs = getStructConflicts(currentData.structs,baseData.structs);
    conflictlist.typedefs = getTypedefConflicts(currentData.typedefs,baseData.typedefs);
    return conflictlist;
}

void TypeWizardPage::merge(TypeConfilictList differ1, TypeConfilictList differ2)
{
    QSet<QString> datatypeNameSet = differ1.datatypes.keys().toSet().unite(differ2.datatypes.keys().toSet());
    foreach (const QString& dataName, datatypeNameSet)
    {
        QPair<tDatatypeData,DataStatus> differ1Data = differ1.datatypes.value(dataName);
        QPair<tDatatypeData,DataStatus> differ2Data = differ2.datatypes.value(dataName);
        if (differ1Data == QPair<tDatatypeData,DataStatus>())
        {
            mergeIntoProject(differ2Data.first,DATATYPE,differ2Data.second);
        }
        else if (differ2Data == QPair<tDatatypeData,DataStatus>())
        {
            mergeIntoProject(differ1Data.first,DATATYPE,differ1Data.second);
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
                    mergeIntoProject(differ1Data.first,DATATYPE,differ1Data.second);
                }
                else if (!(differ1Data.first == differ2Data.first))
                {
                    addRow(differ1Data.first,differ2Data.first,DATATYPE,differ1Data.second,differ2Data.second);
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
                mergeIntoProject(differ2Data.first,DATATYPE,differ2Data.second);
            }
            else if (differ2Data.second == STATUS_EQUAL
                     && differ1Data.second != STATUS_EQUAL)
            {
                mergeIntoProject(differ1Data.first,DATATYPE,differ1Data.second);
            }
            else if (differ1Data.second == STATUS_REMOVE
                     && differ2Data.second == STATUS_MODIFY)
            {
                addRow(differ1Data.first,differ2Data.first,DATATYPE,differ1Data.second,differ2Data.second);
            }
            else if (differ1Data.second == STATUS_MODIFY
                     && differ2Data.second == STATUS_REMOVE)
            {
                addRow(differ1Data.first,differ2Data.first,DATATYPE,differ1Data.second,differ2Data.second);
            }
        }
    }
    QSet<QString> typedefNameSet = differ1.typedefs.keys().toSet().unite(differ2.typedefs.keys().toSet());
    foreach (const QString& dataName, typedefNameSet)
    {
        QPair<tTypedefData,DataStatus> differ1Data = differ1.typedefs.value(dataName);
        QPair<tTypedefData,DataStatus> differ2Data = differ2.typedefs.value(dataName);
        if (differ1Data == QPair<tTypedefData,DataStatus>())
        {
            mergeIntoProject(differ2Data.first,TYPEDEF,differ2Data.second);
        }
        else if (differ2Data == QPair<tTypedefData,DataStatus>())
        {
            mergeIntoProject(differ1Data.first,TYPEDEF,differ1Data.second);
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
                    mergeIntoProject(differ1Data.first,TYPEDEF,differ1Data.second);
                }
                else if (!(differ1Data.first == differ2Data.first))
                {
                    addRow(differ1Data.first,differ2Data.first,TYPEDEF,differ1Data.second,differ2Data.second);
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
                mergeIntoProject(differ2Data.first,TYPEDEF,differ2Data.second);
            }
            else if (differ2Data.second == STATUS_EQUAL
                     && differ1Data.second != STATUS_EQUAL)
            {
                mergeIntoProject(differ1Data.first,TYPEDEF,differ1Data.second);
            }
            else if (differ1Data.second == STATUS_REMOVE
                     && differ2Data.second == STATUS_MODIFY)
            {
                addRow(differ1Data.first,differ2Data.first,TYPEDEF,differ1Data.second,differ2Data.second);
            }
            else if (differ1Data.second == STATUS_MODIFY
                     && differ2Data.second == STATUS_REMOVE)
            {
                addRow(differ1Data.first,differ2Data.first,TYPEDEF,differ1Data.second,differ2Data.second);
            }
        }
    }
    QSet<QString> structNameSet = differ1.structs.keys().toSet().unite(differ2.structs.keys().toSet());
    foreach (const QString& dataName, structNameSet)
    {
        QPair<tStructData,DataStatus> differ1Data = differ1.structs.value(dataName);
        QPair<tStructData,DataStatus> differ2Data = differ2.structs.value(dataName);
        if (differ1Data == QPair<tStructData,DataStatus>())
        {
            mergeIntoProject(differ2Data.first,STRUCT,differ2Data.second);
        }
        else if (differ2Data == QPair<tStructData,DataStatus>())
        {
            mergeIntoProject(differ1Data.first,STRUCT,differ1Data.second);
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
                    mergeIntoProject(differ1Data.first,STRUCT,differ1Data.second);
                }
                else if (!(differ1Data.first == differ2Data.first))
                {
                    addRow(differ1Data.first,differ2Data.first,STRUCT,differ1Data.second,differ2Data.second);
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
                mergeIntoProject(differ2Data.first,STRUCT,differ2Data.second);
            }
            else if (differ2Data.second == STATUS_EQUAL
                     && differ1Data.second != STATUS_EQUAL)
            {
                mergeIntoProject(differ1Data.first,STRUCT,differ1Data.second);
            }
            else if (differ1Data.second == STATUS_REMOVE
                     && differ2Data.second == STATUS_MODIFY)
            {
                addRow(differ1Data.first,differ2Data.first,STRUCT,differ1Data.second,differ2Data.second);
            }
            else if (differ1Data.second == STATUS_MODIFY
                     && differ2Data.second == STATUS_REMOVE)
            {
                addRow(differ1Data.first,differ2Data.first,STRUCT,differ1Data.second,differ2Data.second);
            }
        }
    }
    QSet<QString> macroNameSet = differ1.macros.keys().toSet().unite(differ2.macros.keys().toSet());
    foreach (const QString& dataName, macroNameSet)
    {
        QPair<tMacroData,DataStatus> differ1Data = differ1.macros.value(dataName);
        QPair<tMacroData,DataStatus> differ2Data = differ2.macros.value(dataName);
        if (differ1Data == QPair<tMacroData,DataStatus>())
        {
            mergeIntoProject(differ2Data.first,MACRO,differ2Data.second);
        }
        else if (differ2Data == QPair<tMacroData,DataStatus>())
        {
            mergeIntoProject(differ1Data.first,MACRO,differ1Data.second);
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
                    mergeIntoProject(differ1Data.first,MACRO,differ1Data.second);
                }
                else if (!(differ1Data.first == differ2Data.first))
                {
                    addRow(differ1Data.first,differ2Data.first,MACRO,differ1Data.second,differ2Data.second);
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
                mergeIntoProject(differ2Data.first,MACRO,differ2Data.second);
            }
            else if (differ2Data.second == STATUS_EQUAL
                     && differ1Data.second != STATUS_EQUAL)
            {
                mergeIntoProject(differ1Data.first,MACRO,differ1Data.second);
            }
            else if (differ1Data.second == STATUS_REMOVE
                     && differ2Data.second == STATUS_MODIFY)
            {
                addRow(differ1Data.first,differ2Data.first,MACRO,differ1Data.second,differ2Data.second);
            }
            else if (differ1Data.second == STATUS_MODIFY
                     && differ2Data.second == STATUS_REMOVE)
            {
                addRow(differ1Data.first,differ2Data.first,MACRO,differ1Data.second,differ2Data.second);
            }
        }
    }
    QSet<QString> enumNameSet = differ1.enums.keys().toSet().unite(differ2.enums.keys().toSet());
    foreach (const QString& dataName, enumNameSet)
    {
        QPair<tEnumData,DataStatus> differ1Data = differ1.enums.value(dataName);
        QPair<tEnumData,DataStatus> differ2Data = differ2.enums.value(dataName);
        if (differ1Data == QPair<tEnumData,DataStatus>())
        {
            mergeIntoProject(differ2Data.first,ENUM,differ2Data.second);
        }
        else if (differ2Data == QPair<tEnumData,DataStatus>())
        {
            mergeIntoProject(differ1Data.first,ENUM,differ1Data.second);
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
                    mergeIntoProject(differ1Data.first,ENUM,differ1Data.second);
                }
                else if (!(differ1Data.first == differ2Data.first))
                {
                    addRow(differ1Data.first,differ2Data.first,ENUM,differ1Data.second,differ2Data.second);
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
                mergeIntoProject(differ2Data.first,ENUM,differ2Data.second);
            }
            else if (differ2Data.second == STATUS_EQUAL
                     && differ1Data.second != STATUS_EQUAL)
            {
                mergeIntoProject(differ1Data.first,ENUM,differ1Data.second);
            }
            else if (differ1Data.second == STATUS_REMOVE
                     && differ2Data.second == STATUS_MODIFY)
            {
                addRow(differ1Data.first,differ2Data.first,ENUM,differ1Data.second,differ2Data.second);
            }
            else if (differ1Data.second == STATUS_MODIFY
                     && differ2Data.second == STATUS_REMOVE)
            {
                addRow(differ1Data.first,differ2Data.first,ENUM,differ1Data.second,differ2Data.second);
            }
        }
    }
}

tDatatypeData TypeWizardPage::findDatatype(tTypeData data, QString name)
{
    tDatatypeData Datatype = tDatatypeData();
    foreach (const tDatatypeData& datatype,data.datatypes)
    {
        if (datatype.name == name)
        {
            Datatype = datatype;
            break;
        }
    }
    return Datatype;
}

tTypedefData TypeWizardPage::findTypedef(tTypeData data, QString name)
{
    tTypedefData Typedef = tTypedefData();
    foreach (const tTypedefData& typed,data.typedefs)
    {
        if (typed.name == name)
        {
            Typedef = typed;
            break;
        }
    }
    return Typedef;
}

tStructData TypeWizardPage::findStruct(tTypeData data, QString name)
{
    tStructData Struct = tStructData();
    foreach (const tStructData& struct1,data.structs)
    {
        if (struct1.name == name)
        {
            Struct = struct1;
            break;
        }
    }
    return Struct;
}

tMacroData TypeWizardPage::findMacro(tTypeData data, QString name)
{
    tMacroData Macro = tMacroData();
    foreach (const tMacroData& macro,data.macros)
    {
        if (macro.name == name)
        {
            Macro = macro;
            break;
        }
    }
    return Macro;
}

tEnumData TypeWizardPage::findEnum(tTypeData data, QString name)
{
    tEnumData Enum = tEnumData();
    foreach (const tEnumData& enum1,data.enums)
    {
        if (enum1.name == name)
        {
            Enum = enum1;
            break;
        }
    }
    return Enum;
}

template<typename T>
void TypeWizardPage::mergeIntoProject(T data,TypeName type,DataStatus status)
{
    //为了强转，先转化为指针再强转：
    void* dataAddr = &data;

    switch (type)
    {
    case DATATYPE:
    {
        tDatatypeData datatype = *(tDatatypeData*)dataAddr;
        switch(status)
        {
        case STATUS_MODIFY:
        {
            if (m_datatypesNameList.contains(datatype.name))
            {
                m_baseTypeData.datatypes.replace(m_datatypesNameList.indexOf(datatype.name),datatype);
            }
            break;
        }
        case STATUS_ADD:
        {
            m_baseTypeData.datatypes.append(datatype);
            m_datatypesNameList.append(datatype.name);
            break;
        }
        case STATUS_REMOVE:
        {
            if (m_datatypesNameList.contains(datatype.name))
            {
                m_baseTypeData.datatypes.removeOne(datatype);
                m_datatypesNameList.removeOne(datatype.name);
            }
            break;
        }
        default:
            break;
        }
        break;
    }
    case TYPEDEF:
    {
        tTypedefData typeDef = *(tTypedefData*)dataAddr;
        switch(status)
        {
        case STATUS_MODIFY:
        {
            if (m_typedefsNameList.contains(typeDef.name))
            {
                m_baseTypeData.typedefs.replace(m_typedefsNameList.indexOf(typeDef.name),typeDef);
            }
            break;
        }
        case STATUS_ADD:
        {
            m_baseTypeData.typedefs.append(typeDef);
            m_typedefsNameList.append(typeDef.name);
            break;
        }
        case STATUS_REMOVE:
        {
            if (m_typedefsNameList.contains(typeDef.name))
            {
                m_baseTypeData.typedefs.removeOne(typeDef);
                m_typedefsNameList.removeOne(typeDef.name);
            }
            break;
        }
        default:
            break;
        }
        break;
    }
    case MACRO:
    {
        tMacroData macro = *(tMacroData*)dataAddr;
        switch(status)
        {
        case STATUS_MODIFY:
        {
            if (m_macrosNameList.contains(macro.name))
            {
                m_baseTypeData.macros.replace(m_macrosNameList.indexOf(macro.name),macro);
            }
            break;
        }
        case STATUS_ADD:
        {
            m_baseTypeData.macros.append(macro);
            m_macrosNameList.append(macro.name);
            break;
        }
        case STATUS_REMOVE:
        {
            if (m_macrosNameList.contains(macro.name))
            {
                m_baseTypeData.macros.removeOne(macro);
                m_macrosNameList.removeOne(macro.name);
            }
            break;
        }
        default:
            break;
        }
        break;
    }
    case STRUCT:
    {
        tStructData Struct = *(tStructData*)dataAddr;
        switch(status)
        {
        case STATUS_MODIFY:
        {
            if (m_structsNameList.contains(Struct.name))
            {
                m_baseTypeData.structs.replace(m_structsNameList.indexOf(Struct.name),Struct);
            }
            break;
        }
        case STATUS_ADD:
        {
            m_baseTypeData.structs.append(Struct);
            m_structsNameList.append(Struct.name);
            break;
        }
        case STATUS_REMOVE:
        {
            if (m_structsNameList.contains(Struct.name))
            {
                m_baseTypeData.structs.removeOne(Struct);
                m_structsNameList.removeOne(Struct.name);
            }
            break;
        }
        default:
            break;
        }
        break;
    }
    case ENUM:
    {
        tEnumData Enum = *(tEnumData*)dataAddr;
        switch(status)
        {
        case STATUS_MODIFY:
        {
            if (m_enumsNameList.contains(Enum.name))
            {
                m_baseTypeData.enums.replace(m_enumsNameList.indexOf(Enum.name),Enum);
            }
            break;
        }
        case STATUS_ADD:
        {
            m_baseTypeData.enums.append(Enum);
            m_enumsNameList.append(Enum.name);
            break;
        }
        case STATUS_REMOVE:
        {
            if (m_enumsNameList.contains(Enum.name))
            {
                m_baseTypeData.enums.removeOne(Enum);
                m_enumsNameList.removeOne(Enum.name);
            }
            break;
        }
        default:
            break;
        }
        break;
    }
    }
}

QMap<QString,QPair<tDatatypeData,DataStatus>> TypeWizardPage::getDatatypeConflicts(QList<tDatatypeData> currentData,QList<tDatatypeData> baseData)
{
    QStringList dataNameList;
    QStringList mergedNameList;
    foreach(const tDatatypeData& data,currentData)
    {
        dataNameList.append(data.name);
    }
    foreach(const tDatatypeData& data,baseData)
    {
        mergedNameList.append(data.name);
    }
    return getConflicts(currentData,baseData,dataNameList,mergedNameList);
}

QMap<QString,QPair<tEnumData,DataStatus>> TypeWizardPage::getEnumConflicts(QList<tEnumData> currentData,QList<tEnumData> baseData)
{
    QStringList dataNameList;
    QStringList mergedNameList;
    foreach(const tEnumData& data,currentData)
    {
        dataNameList.append(data.name);
    }
    foreach(const tEnumData& data,baseData)
    {
        mergedNameList.append(data.name);
    }
    return getConflicts(currentData,baseData,dataNameList,mergedNameList);
}

QMap<QString,QPair<tMacroData,DataStatus>> TypeWizardPage::getMacroConflicts(QList<tMacroData> currentData,QList<tMacroData> baseData)
{
    QStringList dataNameList;
    QStringList mergedNameList;
    foreach(const tMacroData& data,currentData)
    {
        dataNameList.append(data.name);
    }
    foreach(const tMacroData& data,baseData)
    {
        mergedNameList.append(data.name);
    }
    return getConflicts(currentData,baseData,dataNameList,mergedNameList);
}

QMap<QString,QPair<tStructData,DataStatus>> TypeWizardPage::getStructConflicts(QList<tStructData> currentData,QList<tStructData> baseData)
{
    QStringList dataNameList;
    QStringList mergedNameList;
    foreach(const tStructData& data,currentData)
    {
        dataNameList.append(data.name);
    }
    foreach(const tStructData& data,baseData)
    {
        mergedNameList.append(data.name);
    }
    return getConflicts(currentData,baseData,dataNameList,mergedNameList);
}

QMap<QString,QPair<tTypedefData,DataStatus>> TypeWizardPage::getTypedefConflicts(QList<tTypedefData> currentData,QList<tTypedefData> baseData)
{
    QStringList dataNameList;
    QStringList mergedNameList;
    foreach(const tTypedefData& data,currentData)
    {
        dataNameList.append(data.name);
    }
    foreach(const tTypedefData& data,baseData)
    {
        mergedNameList.append(data.name);
    }
    return getConflicts(currentData,baseData,dataNameList,mergedNameList);
}

template<typename T>
QMap<QString,QPair<T,DataStatus>> TypeWizardPage::getConflicts(QList<T> dataList,QList<T> baseDataList,QStringList dataNameList,QStringList baseNameList)
{
    QMap<QString,QPair<T,DataStatus>> conflictsList;

    for(int i = 0;i < dataList.size();i++)
    {
        const T data = dataList.at(i);
        if (baseDataList.contains(data))
        {
            conflictsList.insert(dataNameList.at(i),QPair<T,DataStatus>(data,STATUS_EQUAL));
        }
        else if (!baseDataList.contains(data) && baseNameList.contains(dataNameList.at(i)))
        {
            conflictsList.insert(dataNameList.at(i),QPair<T,DataStatus>(data,STATUS_MODIFY));
        }
        else if (!baseDataList.contains(data) && !baseNameList.contains(dataNameList.at(i)))
        {
            conflictsList.insert(dataNameList.at(i),QPair<T,DataStatus>(data,STATUS_ADD));
        }
    }
    for(int i = 0;i < baseDataList.size();i++)
    {
        const T data = baseDataList.at(i);
        if (!dataList.contains(data) && !dataNameList.contains(baseNameList.at(i)))
        {
            conflictsList.insert(baseNameList.at(i),QPair<T,DataStatus>(data,STATUS_REMOVE));
        }
    }
    return conflictsList;
}

template<typename T>
void TypeWizardPage::addRow(T currentData,T mergedData,TypeName type,DataStatus currentStatus,DataStatus mergedStatus)
{
    void* currentDataAddr = &currentData;
    void* mergedDataAddr = &mergedData;
    switch (type)
    {
    case DATATYPE:
    {
        tDatatypeData currentData = *(tDatatypeData*)currentDataAddr;
        tDatatypeData mergedData = *(tDatatypeData*)mergedDataAddr;
        QString Name = (currentData.name.isEmpty())?mergedData.name:currentData.name;
        StringListModel()->insertRow(StringListModel()->rowCount());
        StringListModel()->setData(StringListModel()->index(StringListModel()->rowCount() - 1)
                                   ,QVariant("Datatype:" + Name)
                                   ,Qt::DisplayRole);
        QString currentText = displayText(currentData,type);
        QString mergedText = displayText(mergedData,type);
        m_conflictTextMap.insert("Datatype:" + Name,CompareText{currentText,mergedText,NO_CHOSEN,currentStatus,mergedStatus});
        break;
    }
    case TYPEDEF:
    {
        tTypedefData currentData = *(tTypedefData*)currentDataAddr;
        tTypedefData mergedData = *(tTypedefData*)mergedDataAddr;
        QString Name = (currentData.name.isEmpty())?mergedData.name:currentData.name;
        StringListModel()->insertRow(StringListModel()->rowCount());
        StringListModel()->setData(StringListModel()->index(StringListModel()->rowCount() - 1)
                                   ,QVariant("Typedef:" + Name)
                                   ,Qt::DisplayRole);
        QString currentText = displayText(currentData,type);
        QString mergedText = displayText(mergedData,type);
        m_conflictTextMap.insert("Typedef:" + Name,CompareText{currentText,mergedText,NO_CHOSEN,currentStatus,mergedStatus});
        break;
    }
    case STRUCT:
    {
        tStructData currentData = *(tStructData*)currentDataAddr;
        tStructData mergedData = *(tStructData*)mergedDataAddr;
        QString Name = (currentData.name.isEmpty())?mergedData.name:currentData.name;
        StringListModel()->insertRow(StringListModel()->rowCount());
        StringListModel()->setData(StringListModel()->index(StringListModel()->rowCount() - 1)
                                   ,QVariant("Struct:" + Name)
                                   ,Qt::DisplayRole);
        QString currentText = displayText(currentData,type);
        QString mergedText = displayText(mergedData,type);
        m_conflictTextMap.insert("Struct:" + Name,CompareText{currentText,mergedText,NO_CHOSEN,currentStatus,mergedStatus});
        break;
    }
    case MACRO:
    {
        tMacroData currentData = *(tMacroData*)currentDataAddr;
        tMacroData mergedData = *(tMacroData*)mergedDataAddr;
        QString Name = (currentData.name.isEmpty())?mergedData.name:currentData.name;
        StringListModel()->insertRow(StringListModel()->rowCount());
        StringListModel()->setData(StringListModel()->index(StringListModel()->rowCount() - 1)
                                   ,QVariant("Macro:" + Name)
                                   ,Qt::DisplayRole);
        QString currentText = displayText(currentData,type);
        QString mergedText = displayText(mergedData,type);
        m_conflictTextMap.insert("Macro:" + Name,CompareText{currentText,mergedText,NO_CHOSEN,currentStatus,mergedStatus});
        break;
    }
    case ENUM:
    {
        tEnumData currentData = *(tEnumData*)currentDataAddr;
        tEnumData mergedData = *(tEnumData*)mergedDataAddr;
        QString Name = (currentData.name.isEmpty())?mergedData.name:currentData.name;
        StringListModel()->insertRow(StringListModel()->rowCount());
        StringListModel()->setData(StringListModel()->index(StringListModel()->rowCount() - 1)
                                   ,QVariant("Enum:" + Name)
                                   ,Qt::DisplayRole);
        QString currentText = displayText(currentData,type);
        QString mergedText = displayText(mergedData,type);
        m_conflictTextMap.insert("Enum:" + Name,CompareText{currentText,mergedText,NO_CHOSEN,currentStatus,mergedStatus});
        break;
    }
    }
}

template<typename T>
QString TypeWizardPage::displayText(T data,TypeName type)
{
    void* dataAddr = &data;
    QString text = QString();
    switch (type){
    case DATATYPE:
    {
        tDatatypeData datatype = *(tDatatypeData*)dataAddr;
        text = "name:" + datatype.name + "\nsize:" + QString::number(datatype.size) + "\n";
    }
    case STRUCT:
    {
        tStructData Struct = *(tStructData*)dataAddr;
        text = "#program pack(" + QString::number(Struct.alignment) + ")"
                + "\ntypedef struct " + Struct.name + " {\n";
        foreach (const tStructInfo& info,Struct.structInfo)
        {
            text += info.type + " " + info.name;
            if (info.arraysize.toInt() > 1)
            {
                text += "[" + info.arraysize + "]";
            }
            text += ";\n";
        }
        text += "};\n";
        break;
    }
    case TYPEDEF:
    {
        tTypedefData typedefData = *(tTypedefData*)dataAddr;
        text = "typedef " + typedefData.value + " " + typedefData.name + ";\n";
        break;
    }
    case MACRO:
    {
        tMacroData macro = *(tMacroData*)dataAddr;
        text = "#define " + macro.name + " " + macro.value + "\n";
        break;
    }
    case ENUM:
    {
        tEnumData enumData = *(tEnumData*)dataAddr;
        text = "enum {\n";
        foreach (const tEnumInfo& info,enumData.enuminfo)
        {
            text += info.name;
            if (!info.value.isEmpty())
            {
                text += " = " + info.value;
            }
            text += ";\n";
        }
        text += "};";
        break;
    }
    default:
    {
        break;
    }
    }
    return text;
}

