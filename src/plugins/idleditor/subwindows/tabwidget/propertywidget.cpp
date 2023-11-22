#include "propertywidget.h"
#include "datahandle.h"
#include "../idleditor.h"
#include "../idldocument.h"
#include <utils/stringutils.h>
namespace Idl{
namespace Internal{

//lingfei.yu 真正的propertywidget
PropertyWidget::PropertyWidget(IdlEditor* IdlEditor, QWidget *parent) :
    EmosWidgets::FlatTable(parent),
    m_editor(IdlEditor)
{
    m_propertyTypeList<<"String"<<"Int"<<"Bool";
    this->setEditTriggers(QAbstractItemView::DoubleClicked);
    this->setColumnCount(2);
    this->setHorizontalHeaderLabels(QStringList()<< "Name" << "Type");
}

PropertyWidget::~PropertyWidget()
{
}

//从xml里面加载到界面，从类构造开始应该只会进入此函数一次
void PropertyWidget::loadFromDoc()
{
    disconnect(this->model(),&QAbstractItemModel::dataChanged,this,&PropertyWidget::onModify);
    restoreControl();
    QDomDocument* doc = qobject_cast<IdlDocument*>(m_editor->document())->getRootDoc();
    QDomElement deRootNode = doc->documentElement();
    QDomNodeList localNodeList = deRootNode.elementsByTagName("Property");
    for(int i = 0; i < localNodeList.size(); i++){
        QString strName;
        QString strType;
        strName = localNodeList.item(i).toElement().attribute("name");
        strType = localNodeList.item(i).toElement().attribute("type");
        appendRow(strName,strType);
    }
    if(this->rowCount() > 0){
        this->selectRow(0);
    }
    connect(this->model(),&QAbstractItemModel::dataChanged,this,&PropertyWidget::onModify);
}

//从界面保存到xml
void PropertyWidget::exportToDoc()
{
    m_propertyNameList.clear();
    QDomDocument* doc = qobject_cast<IdlDocument*>(m_editor->document())->getRootDoc();
    QDomElement deRootNode = doc->documentElement();
    Datahandle::removeChild(&deRootNode, "Property");
    for(int modeIdx = 0; modeIdx < this->rowCount(); modeIdx++){
        QString name = this->item(modeIdx,0)->text();
        if(name.isEmpty()){
            continue;
        }
        //返回正则重复
        if(checkNameRepeat(name)){
            EmosWidgets::MessageTips::showText(this, "property name repeat", EmosWidgets::MessageTips::MediumText,
                                               EmosWidgets::MessageTips::PositionBottom, EmosWidgets::MessageTips::Warning);
            continue;
        }
        QDomElement item = doc->createElement("Property");
        item.setAttribute("name",name);
        if(this->cellWidget(modeIdx,1)){
            item.setAttribute("type",(reinterpret_cast<QComboBox*>(this->cellWidget(modeIdx,1)))->currentText());
        }else {
            item.setAttribute("type","");
        }
        deRootNode.appendChild(item);
    }
}


//表格尾增加一行
void PropertyWidget::appendRow(QString name, QString type)
{
    if(name == ""){
        QStringList listName;
        for(int modeIdx = 0; modeIdx < this->rowCount(); modeIdx++){
            listName << this->item(modeIdx,0)->text();
        }
        this->appendRowItems(QStringList() << Utils::makeUniquelyNumbered(QString("property"),listName) << type);
    }
    else
        this->appendRowItems(QStringList() << name << type);
    QComboBox* comboBox = new QComboBox(this);
    comboBox->addItems(m_propertyTypeList);
    comboBox->setCurrentText(type);
    connect(comboBox,&QComboBox::currentTextChanged,this,&PropertyWidget::onModify);
    this->setCellWidget(this->rowCount() -1,1,comboBox);
    if(this->rowCount() > 0)
    {
        this->selectRow(this->rowCount() - 1);
    }
}

//删除当前的行
void PropertyWidget::removeOneRow()
{
    this->removeRow(this->currentRow());
    onModify();
}

void PropertyWidget::onModify()
{
    if(m_editor){
        exportToDoc();
        emit propertyChange();
        qobject_cast<TextEditor::TextDocument*>(m_editor->document())->document()->setModified(true);
    }
}

bool PropertyWidget::checkNameRepeat(const QString name)
{
    bool flagrepeat = false;
    for(int i = 0; i < m_propertyNameList.size(); i++){
        if(name == m_propertyNameList[i]){
            flagrepeat = true;
            break;
        }
    }
    if(flagrepeat == false){
        m_propertyNameList.push_back(name);
    }
    return flagrepeat;
}

//清空状态
void PropertyWidget::restoreControl()
{
    this->clearContents();
    this->setRowCount(0);
}

}
}
