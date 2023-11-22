#include "apimethodwidget.h"

#include <utils/emoswidgets/flattable.h>
#include <utils/emoswidgets/messagetips.h>
#include <utils/emoswidgets/automulticomobox.h>

#include <emostools/api_description_parser.h>
#include <emostools/type_description_parser.h>

#include "advancemethod.h"

#include <QAction>
#include <QTableWidgetItem>
#include <QDomDocument>
#include <QHeaderView>
#include <QListWidget>
#include <QComboBox>
#include <QPushButton>
#include <QWidgetAction>
#include <QLineEdit>

using namespace EmosWidgets;
using namespace EmosTools;

#define ParamRole  0x0100 + 1
Q_DECLARE_METATYPE(QList<EmosTools::tParam>)

ApiMethodWidget::ApiMethodWidget(QDomDocument* doc, QWidget *parent) :
    ApiWidgetBase(doc, parent)
{
    m_table->setColumnCount(4);
    m_table->setHorizontalHeaderLabels(QStringList()<< "Name"<<"ID"<< "Ret"<<"Para");
    m_table->setHeaderSize({1,1,1,2});
    m_table->setEditTriggers(QTableView::AllEditTriggers);
    m_tagName = "Method";
}

ApiMethodWidget::~ApiMethodWidget()
{
}

void ApiMethodWidget::widgetData(QDomDocument* doc, QDomElement* element)
{
    for(int modeIdx = 0; modeIdx < m_table->rowCount(); modeIdx++)
    {
        if(m_table->item(modeIdx,0) == nullptr)
        {
            return;
        }

        QDomElement methodElement = doc->createElement("element");

        methodElement.setAttribute("name", m_table->item(modeIdx,0)->text());
        methodElement.setAttribute("type", "Method");
        methodElement.setAttribute("id", m_table->item(modeIdx,1)->text());
        methodElement.setAttribute("ret", qobject_cast<QComboBox*>(m_table->cellWidget(modeIdx,2))->currentText());

        QList<tParam> param = getInputParams(modeIdx,3);
        for(int paraIdx = 0; paraIdx< param.size();paraIdx++)
        {
            QDomElement itemPara = doc->createElement("para");

            if(param[paraIdx].name == "" || param[paraIdx].name.contains("Para", Qt::CaseSensitive))
            {
                param[paraIdx].name = paraName(param[paraIdx].type, paraIdx);
            }
            itemPara.setAttribute("name", param[paraIdx].name);
            itemPara.setAttribute("type", param[paraIdx].type);
            itemPara.setAttribute("version",param[paraIdx].version);
            itemPara.setAttribute("direction",param[paraIdx].direction);
            itemPara.setAttribute("size",param[paraIdx].size);
            methodElement.appendChild(itemPara);
        }
        element->appendChild(methodElement);
    }
}

void ApiMethodWidget::slotAddRow()
{
    QList<int> idList;
    for(int modeIdx = 0; modeIdx < m_table->rowCount(); modeIdx++)
    {
        idList << m_table->item(modeIdx,1)->text().toInt();
    }
    m_table->appendRowItems(QStringList() << getDefaultName("Method") << QString::number(makeUniquelyId(idList)) << "" << "");
    QTableWidgetItem* item = m_table->item(m_table->rowCount() - 1, 1);
    item->setFlags(item->flags() & ~Qt::ItemIsEditable);

    appendComboBox(m_typeWithoutPointer, m_table->rowCount() - 1, 2, "tResult");

    //param
    AutoMultiComoBox* comboBox = new AutoMultiComoBox(m_table);
    comboBox->setSeparator(",");
    comboBox->setEditable(true);
    comboBox->setCompleteList(m_typeList);
    addButtonForMethod(comboBox);

    connect(comboBox, &QComboBox::currentTextChanged, this, &ApiMethodWidget::paraChanged);
    m_table->setCellWidget(m_table->rowCount() - 1, 3, comboBox);
    //参数列记录参数信息
    m_table->item(m_table->rowCount() - 1, 3)->setData(ParamRole, QVariant::fromValue(QList<EmosTools::tParam>()));

    selectLastRow();
    emit modify();
}

QString ApiMethodWidget::paraName(const QString &type, int index)
{
    QString name = "Para" + QString::number(index);
    if(type.toLower().indexOf("uint8") >= 0)
        name = "u8" + name;
    else if(type.toLower().indexOf("uint16") >= 0)
        name = "u16" + name;
    else if(type.toLower().indexOf("uint32") >= 0)
        name = "u32" + name;
    else if(type.toLower().indexOf("uint64") >= 0)
        name = "u64" + name;
    else if(type.toLower().indexOf("int8") >= 0)
        name = "i8" + name;
    else if(type.toLower().indexOf("int16") >= 0)
        name = "i16" + name;
    else if(type.toLower().indexOf("int32") >= 0)
        name = "i32" + name;
    else if(type.toLower().indexOf("int64") >= 0)
        name = "i64" + name;
    else if(type.toLower().indexOf("float32") >= 0)
        name = "f32" + name;
    else if(type.toLower().indexOf("float64") >= 0)
        name = "f64" + name;
    else
        name = "s" + name;

    return name;
}

void ApiMethodWidget::addButtonForMethod(EmosWidgets::AutoMultiComoBox *comboBox)
{
    QLineEdit *edit = comboBox->lineEdit();
    QPushButton *btn = new QPushButton(edit);
    btn->setText("##");
    btn->setCursor(Qt::PointingHandCursor);
    QWidgetAction* action = new QWidgetAction(edit);
    action->setDefaultWidget(btn);
    edit->addAction(action, QLineEdit::TrailingPosition);

    connect(btn, &QPushButton::clicked, this, &ApiMethodWidget::showAdvanceMethod);
}

bool ApiMethodWidget::isUseAdvance(const QStringList &paraList, QList<EmosTools::tParam>& param)
{
    if(param.size() == 0)
        return false;
    if(param.size()  != paraList.size())
        return false;

    for(int paraIdx = 0; paraIdx< paraList.size();paraIdx++)
    {
        if(param[paraIdx].name == "")
        {
            param[paraIdx].name = paraName(param[paraIdx].type, paraIdx);
        }
        QStringList paraInfoList = paraList.at(paraIdx).split(" ");
        paraInfoList.removeAll(QString(""));

        int limitSize = 1;
        if(paraInfoList.contains("const"))//const 会多一个组成
            limitSize = 2;

        if(paraInfoList.size() > limitSize)
        {
            if(paraInfoList.last() != param[paraIdx].name)
            {
                return false;
            }
            if(paraList.at(paraIdx).left(paraList.at(paraIdx).size() - paraInfoList.last().size() - 1) != param[paraIdx].type)
            {
                return false;
            }
        }
        else
        {
            if(paraList.at(paraIdx) != param[paraIdx].type)
            {
                return false;
            }
        }
    }

    return true;
}

QList<tParam> ApiMethodWidget::getInputParams(int row, int col)
{
    EmosWidgets::AutoMultiComoBox* comboBox = qobject_cast<EmosWidgets::AutoMultiComoBox*>(m_table->cellWidget(row,col));
    QList<EmosTools::tParam> param = m_table->item(row,col)->data(ParamRole).value<QList<EmosTools::tParam>>();

    QStringList paraList;
    if(!comboBox->currentText().isEmpty())
    {
        paraList = comboBox->currentText().split(',');
        paraList.removeAll(QString(""));
    }

    if(isUseAdvance(paraList, param))//使用高级设置
    {
        return param;
    }
    else
    {
        param.clear();
        for(int paraIdx = 0; paraIdx< paraList.size();paraIdx++)
        {
            EmosTools::tParam tp;

            QStringList paraInfoList = paraList.at(paraIdx).split(" ");
            paraInfoList.removeAll(QString(""));

            int limitSize = 1;
            if(paraInfoList.contains("const"))//const 会多一个组成
                limitSize = 2;

            if(paraInfoList.size() > limitSize)
            {
                tp.name = paraInfoList.last();
                tp.type = paraList.at(paraIdx).left(paraList.at(paraIdx).size() - paraInfoList.last().size() - 1);
            }
            else
            {
                tp.name = paraName(paraList.at(paraIdx), paraIdx);
                tp.type = paraList.at(paraIdx);
            }
            tp.direction = EmosTools::Param_In;
            tp.version = 1;
            tp.size = 0;

            param << tp;
        }
    }

    return param;
}

void ApiMethodWidget::loadWidget(EmosTools::tSrvParsingService srv)
{
    disconnect(m_table, &QTableWidget::itemChanged, this, &ApiWidgetBase::slotItemChanged);
    getTypeInfo();
    QList<tSrvFunction> functions = srv.functions;

    for(int i = 0; i < m_table->rowCount();)
    {
        m_table->removeRow(i);
    }

    for(int i = 0; i < functions.size(); i++)
    {
        QString strID = QString::number(functions[i].id);
        QString strName = functions[i].name;
        QString strRet = functions[i].returntype;
        QString strPara;
        QString strParaName;

        for(int paraIdx = 0; paraIdx < functions[i].params.size(); paraIdx++)
        {
            if(0 != paraIdx)
            {
                strPara += ",";
                strParaName += ",";
            }
            strPara += functions[i].params[paraIdx].type;
            QString defaultName = paraName(functions[i].params[paraIdx].type, paraIdx);
            if(functions[i].params[paraIdx].name != defaultName)
            {
                strPara += " " + functions[i].params[paraIdx].name;
            }
        }
        appendRow(QStringList() << strName << strID << strRet << strPara);
        QTableWidgetItem* item = m_table->item(i, 1);
        item->setFlags(item->flags() & ~Qt::ItemIsEditable);

        //param
        AutoMultiComoBox* comboBox = new AutoMultiComoBox(m_table);
        comboBox->setSeparator(",");
        comboBox->setEditable(true);
        comboBox->setCompleteList(m_typeList);
        comboBox->setCurrentText(strPara);
        addButtonForMethod(comboBox);

        connect(comboBox,&QComboBox::currentTextChanged,this,&ApiMethodWidget::paraChanged);
        m_table->setCellWidget(i, 3, comboBox);
        //参数列记录参数信息
        m_table->item(m_table->rowCount() - 1, 3)->setData(ParamRole, QVariant::fromValue(functions[i].params));
    }
    selectFirstRow();
    connect(m_table, &QTableWidget::itemChanged, this, &ApiWidgetBase::slotItemChanged);
}

void ApiMethodWidget::showAdvanceMethod()
{
    QPushButton* btn = qobject_cast<QPushButton*>(sender());
    int x = btn->parentWidget()->parentWidget()->frameGeometry().x();
    int y = btn->parentWidget()->parentWidget()->frameGeometry().y();
    QModelIndex index = m_table->indexAt(QPoint(x,y));

    if(index.isValid())
    {
        getTypeInfo();

        DescriptionEditor::AdvanceMethod advanceMethod;
        advanceMethod.setTypes(m_typeList);
        //从参数列获取记录的参数信息
        advanceMethod.setParams(getInputParams(index.row(), 3));
        advanceMethod.setMinimumSize(this->width()/2, this->height()/2);
        advanceMethod.showDialog();
        advanceMethod.exec();

        QList<EmosTools::tParam> params = advanceMethod.getParams();

        QString strParam;
        for(const auto& param : params)
        {
            strParam += param.type + ((param.name == "")?"":" ") + param.name + ",";
        }
        m_table->setCurrentItem(m_table->item(index.row(), 3));
        qobject_cast<EmosWidgets::AutoMultiComoBox*>(m_table->cellWidget(index.row(), 3))->setCurrentText(strParam);
        //参数列设置参数信息
        m_table->item(index.row(), 3)->setData(ParamRole, QVariant::fromValue(params));
    }
}

void ApiMethodWidget::paraChanged(QString nowText)
{
    QTableWidgetItem* itemTable = m_table->currentItem();
    itemTable->setText(nowText);
}
