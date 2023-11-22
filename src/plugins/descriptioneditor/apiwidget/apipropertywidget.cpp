#include <QVBoxLayout>
#include <QHBoxLayout>
#include <coreplugin/documentmanager.h>
#include <emostools/api_description_parser.h>
#include "apipropertywidget.h"
#include <QRadioButton>
#include <QCheckBox>

ApiPropertyWidget::ApiPropertyWidget(QWidget* parent)
{
    initTable();
}

ApiPropertyWidget::~ApiPropertyWidget()
{
}

void ApiPropertyWidget::initTable()
{
    this->setColumnCount(2);
    this->setRowCount(2);
    this->setHeaderSize({1,2});
    this->setHorizontalHeaderLabels(QStringList()<< "Object" << "Property");
    this->setItem(0, 0, new QTableWidgetItem("interfaceType"));
    this->setItem(1, 0, new QTableWidgetItem("process"));
    this->item(0, 0)->setFlags(this->item(0, 0)->flags() & ~Qt::ItemIsEditable);  // cannot edit first column
    this->item(1, 0)->setFlags(this->item(1, 0)->flags() & ~Qt::ItemIsEditable);

    //interface type
    QWidget* groupInterface= new QWidget(this);
    QHBoxLayout* groupLayout = new QHBoxLayout(groupInterface);
    m_radio_c = new QRadioButton("c", this);
    m_radio_cpp = new QRadioButton("c++", this);
    m_radio_c->setChecked(true);
    connect(m_radio_c, &QRadioButton::clicked, this, &ApiPropertyWidget::modify);
    connect(m_radio_cpp, &QRadioButton::clicked, this, &ApiPropertyWidget::modify);
    groupLayout->addWidget(m_radio_c);
    groupLayout->addWidget(m_radio_cpp);
    this->setCellWidget(0, 1, groupInterface);

    //in process
    QWidget* groupProcess= new QWidget(this);
    QHBoxLayout* processLayout = new QHBoxLayout(groupProcess);
    m_inProcess = new QCheckBox("in process", this);
    connect(m_inProcess, &QCheckBox::clicked, this, &ApiPropertyWidget::modify);
    processLayout->addWidget(m_inProcess);
    processLayout->addStretch();
    this->setCellWidget(1, 1, groupProcess);

    this->resizeRowsToContents();
}

void ApiPropertyWidget::loadWidget(EmosTools::tSrvParsingService srv)
{
    //接口类型
    if(srv.interfaceType == "cpp")
        m_radio_cpp->setChecked(true);
    else
        m_radio_c->setChecked(true);

    if(srv.inProcess)
        m_inProcess->setCheckState(Qt::Checked);
}

void ApiPropertyWidget::widgetData(QDomDocument *doc, QDomElement* element)
{
    //接口类型
    element->setAttribute("interface_type", QString::fromLocal8Bit(m_radio_cpp->isChecked() ? "cpp" : "c"));
    element->setAttribute("inprocess", m_inProcess->isChecked());
}
