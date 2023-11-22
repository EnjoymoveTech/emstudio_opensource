#include <QLineEdit>
#include <QRegExp>
#include "threadwidget.h"
#include "idleditor.h"
#include "datahandle.h"
#include "idldocument.h"
#include "datahandle.h"
#include "servicewidget.h"
#include <utils/emoswidgets/messagetips.h>
#include <emostools/api_description_parser.h>
#include <utils/stringutils.h>
namespace Idl{
namespace Internal{
ThreadWidget::ThreadWidget(IdlEditor* IdlEditor, QWidget *parent) :
    EmosWidgets::FlatTable(parent),
    m_editor(IdlEditor)
{
    m_threadTypeList<< "Event" << "Field" << "Data" << "Time" << "Normal";
    this->setEditTriggers(QAbstractItemView::DoubleClicked);
    this->setColumnCount(3);
    this->setHorizontalHeaderLabels(QStringList()<< "Name" << "ThreadType" << "Data" );
}

ThreadWidget::~ThreadWidget()
{
}

//加载xml到界面
void ThreadWidget::loadFromDoc()
{
    disconnect(this->model(),&QAbstractItemModel::dataChanged,this,&ThreadWidget::onModify);
    QDomDocument* doc = qobject_cast<IdlDocument*>(m_editor->document())->getRootDoc();
    restoreControl();
    QDomElement deRootNode = doc->documentElement();
    QDomNodeList localNodeList = deRootNode.elementsByTagName("Thread");
    QStringList QuoteServiceListName = Datahandle::getQuoteServiceList(doc);
    updateDataList(QuoteServiceListName);
    for(int i = 0; i < localNodeList.size(); i++){
        QString strName = localNodeList.item(i).toElement().attribute("name");
        QString strType = localNodeList.item(i).toElement().attribute("type");
        QString strData = localNodeList.item(i).toElement().attribute("data");
        appendRow(strName,strType,strData);
    }
    if(this->rowCount() > 0){
        this->selectRow(0);
    }
    connect(this->model(),&QAbstractItemModel::dataChanged,this,&ThreadWidget::onModify);
}

//从界面保存到xml
void ThreadWidget::exportToDoc()
{
    QDomDocument* doc = qobject_cast<IdlDocument*>(m_editor->document())->getRootDoc();
    QDomElement deRootNode = doc->documentElement();
    m_threadNameList.clear();
    Datahandle::removeChild(&deRootNode,"Thread");
    for(int modeIdx = 0; modeIdx < this->rowCount(); modeIdx++){
        QString name = this->item(modeIdx,0)->text();
        if(name.isEmpty()){
            continue;
        }
        if(checkNameRepeat(name) == false){
            continue;
        }
        QDomElement item = doc->createElement("Thread");
        item.setAttribute("name", name);
        if(this->cellWidget(modeIdx,1)){
            item.setAttribute("type",(reinterpret_cast<QComboBox*>(this->cellWidget(modeIdx,1)))->currentText());
        }else {
            item.setAttribute("type","");
        }
        if(this->cellWidget(modeIdx,2)){
            if(dynamic_cast<QComboBox*>(this->cellWidget(modeIdx,2)))
                item.setAttribute("data",(reinterpret_cast<QComboBox*>(this->cellWidget(modeIdx,2)))->currentText());
            else if(dynamic_cast<QLineEdit*>(this->cellWidget(modeIdx,2)))
                item.setAttribute("data",(reinterpret_cast<QLineEdit*>(this->cellWidget(modeIdx,2)))->text());
        }
        else if(this->item(modeIdx,2)){
            item.setAttribute("data",this->item(modeIdx,2)->text());
        }
        else {
            item.setAttribute("data","");
        }
        deRootNode.appendChild(item);
    }
}

//表格尾增加一行
void ThreadWidget::appendRow(QString name, QString type, QString data)
{
    if(name == ""){
        QStringList listName;
        for(int modeIdx = 0; modeIdx < this->rowCount(); modeIdx++){
            listName << this->item(modeIdx,0)->text();
        }
        this->appendRowItems(QStringList() << Utils::makeUniquelyNumbered(QString("thread"),listName) << type << data );
    }
    else
        this->appendRowItems(QStringList() << name << type << data );

    //第二列
    addCellWidget(ThreadType, this->rowCount() - 1, type);
    //第三列
    if(type == "Event"){
        addCellWidget(EventType, this->rowCount() - 1, data);
    }
    else if(type == "Field"){
        addCellWidget(FieldType, this->rowCount() - 1, data);
    }
    else if(type == "Data"){
        addCellWidget(DataType, this->rowCount() - 1, data);
    }
    else if(type == "Time"){
        addCellWidget(TimeType, this->rowCount() - 1, data);
    } else if(type == "Normal"){
        this->item(this->rowCount()-1, 2)->setFlags(Qt::ItemIsEnabled);
    }
}

//删除某一行
void ThreadWidget::removeOneRow()
{
    this->removeRow(this->currentRow());
    onModify();
}

void ThreadWidget::onWidgetChange(ServiceWidget* w)
{
    updateDataList(w->getQuetoServices());
}

//当用户修改runnable和thread的时候会触发这里
void ThreadWidget::onModify()
{
    if(m_changeFlag == false && m_editor != nullptr){
        //更新后保存到doc内,因为右上角显示需要从doc更新数据
        exportToDoc();
        emit threadChange();
        qobject_cast<TextEditor::TextDocument*>(m_editor->document())->document()->setModified(true);
    }
}

//清空状态
void ThreadWidget::restoreControl()
{
    this->clearContents();
    this->setRowCount(0);
}

//分别更新event field pin下拉框可以选择的类型
void ThreadWidget::updateDataList(const QStringList &serviceList)
{
    m_changeFlag = true;
    m_eventTypeList.clear();
    m_fieldTypeList.clear();
    m_pinTypeList.clear();
    QList<EmosTools::tSrvService> services = EmosTools::ApiParser::getInstance()->getServices();
    for(int i = 0; i < serviceList.size(); i++ ){
        for(int j = 0; j < services.size(); j++){

            if(serviceList[i] == services[j].name){
                for(int e = 0; e < services[j].events.size(); e++){
                    if(!m_eventTypeList.contains(services[j].events[e].data)){
                        m_eventTypeList.push_back(services[j].events[e].data);
                    }
                }
                for(int f = 0; f < services[j].fields.size(); f++){
                    if(!m_fieldTypeList.contains(services[j].fields[f].data)){
                        m_fieldTypeList.push_back(services[j].fields[f].data);
                    }
                }
                for(int p = 0; p < services[j].pins.size(); p++){
                    if(!m_pinTypeList.contains(services[j].pins[p].data)){
                        m_pinTypeList.push_back(services[j].pins[p].data);
                    }
                }
            }
        }
    }
    updateCombobox();
    m_changeFlag = false;
}

//增加特殊widget
void ThreadWidget::addCellWidget(INTERFACETYPE kind, int rowCount, QString content)
{
    if(kind == TimeType){
        QRegExp regexp_onlynumber("^[0-9]*$");
        QLineEdit* editor = new QLineEdit(this);
        if(content.isEmpty()){
            editor->setPlaceholderText("input the period in milliseconds");
        } else {
            editor->setText(content);
        }
        editor->setValidator(new QRegExpValidator(regexp_onlynumber, this));
        connect(editor,&QLineEdit::textChanged, this, &ThreadWidget::onModify);
        this->setCellWidget(rowCount, 2, editor);
        return;
    }
    QComboBox* comboBox =  new QComboBox();
    switch(kind)
    {
    case ThreadType :
        comboBox->addItems(m_threadTypeList);
        //不能统一放在下面，因为这边连接信号和槽了
        comboBox->setCurrentText(content);
        connect(comboBox,&QComboBox::currentTextChanged,this,&ThreadWidget::onthreadTypeChanged);
        this->setCellWidget(rowCount, 1, comboBox);
        break;
    case EventType :
        comboBox->addItems(m_eventTypeList);
        comboBox->setCurrentText(content);
        this->setCellWidget(rowCount, 2, comboBox);
        break;
    case FieldType :
        comboBox->addItems(m_fieldTypeList);
        comboBox->setCurrentText(content);
        this->setCellWidget(rowCount, 2, comboBox);
        break;
    case DataType :
        comboBox->addItems(m_pinTypeList);
        comboBox->setCurrentText(content);
        this->setCellWidget(rowCount, 2, comboBox);
        break;
    default :
        break;
    }
    connect(comboBox,&QComboBox::currentTextChanged,this,&ThreadWidget::onModify);
}

//切换thread的type的时候内容清空
void ThreadWidget::onthreadTypeChanged(QString type)
{
    if(type == "Event"){
        addCellWidget(EventType, this->currentRow(), "");
    }
    else if(type == "Field"){
        addCellWidget(FieldType, this->currentRow(), "");
    }
    else if(type == "Data"){
        addCellWidget(DataType, this->currentRow(), "");
    }
    else if(type == "Time"){
        addCellWidget(TimeType, this->currentRow(), "");
    }else {
        this->removeCellWidget(this->currentRow(),2);
        if(type == "Normal"){
            QTableWidgetItem* item = new QTableWidgetItem;
            this->setItem(this->currentRow(),2,item);
            item->setText("");
            item->setFlags(Qt::ItemIsEnabled);
        }

    }
}

bool ThreadWidget::checkNameRepeat(QString name)
{
    for(int i = 0;i < m_threadNameList.size(); i++){
        if(name == m_threadNameList[i]){
            EmosWidgets::MessageTips::showText(this, "thread name repeat", EmosWidgets::MessageTips::MediumText,
                                               EmosWidgets::MessageTips::PositionBottom, EmosWidgets::MessageTips::Warning);
            return false;
        }
    }
    m_threadNameList.push_back(name);
    return true;
}

void ThreadWidget::updateCombobox()
{
    for(int modeIdx = 0; modeIdx < this->rowCount(); modeIdx++)
    {
        if(this->item(modeIdx,0) == nullptr)
        {
            continue;
        }
        QString type = qobject_cast<QComboBox*>(this->cellWidget(modeIdx,1))->currentText();
        if(this->cellWidget(modeIdx,2) == nullptr){
            continue;
        }
        if(type == "Event")
        {
            QComboBox* box = qobject_cast<QComboBox*>(this->cellWidget(modeIdx,2));
            QString setValue = box->currentText();//之前设置的值
            box->clear();
            box->addItems(m_eventTypeList);
            box->setCurrentText(setValue);
        }
        else if(type == "Field")
        {
            QComboBox* box = qobject_cast<QComboBox*>(this->cellWidget(modeIdx,2));
            QString setValue = box->currentText();//之前设置的值
            box->clear();
            box->addItems(m_fieldTypeList);
            box->setCurrentText(setValue);
        }
        else if(type == "Data")
        {
            QComboBox* box = qobject_cast<QComboBox*>(this->cellWidget(modeIdx,2));
            QString setValue = box->currentText();//之前设置的值
            box->clear();
            box->addItems(m_pinTypeList);
            box->setCurrentText(setValue);
        }
    }

}

}
}
