#include <QVBoxLayout>
#include <QHBoxLayout>
#include <emostools/api_description_parser.h>
#include <coreplugin/documentmanager.h>
#include "../idleditor.h"
#include "../idldocument.h"
#include "propertyeditorwidget.h"
using namespace EmosTools;
namespace Idl {
namespace Internal {

OidEditWidget::OidEditWidget(QWidget* parent)
    : QWidget(parent)
{
    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->setContentsMargins(0,0,15,0);
    layout->setSpacing(0);
    m_edit= new QLineEdit();
    m_label = new QLabel();
    layout->addWidget(m_edit);
    layout->addWidget(m_label);
    layout->addStretch();
    connect(m_edit, &QLineEdit::textChanged, this, &OidEditWidget::modify);
}

OidEditWidget::~OidEditWidget()
{
}

void OidEditWidget::setOid1(const QString &str)
{
    m_edit->setText(str);
}

void OidEditWidget::setOid2(const QString &str)
{
    m_label->setText(str);
}

QString OidEditWidget::getOid() const
{
    return m_edit->text() + m_label->text();
}

QString OidEditWidget::getOid1() const
{
    return m_edit->text();
}

QString OidEditWidget::getOid2() const
{
    return m_label->text();
}

PropertyEditorWidget::PropertyEditorWidget(IdlEditor* IdlEditor, QWidget* parent)
{
    m_editor = IdlEditor;

    this->setColumnCount(2);
    this->setRowCount(2);
    this->setHeaderSize({1,2});
    this->setHorizontalHeaderLabels(QStringList()<< "Object" << "Property");

    m_oidEditWidget = new OidEditWidget(this);
    this->setItem(0, 0, new QTableWidgetItem("oid"));
    this->setCellWidget(0, 1, m_oidEditWidget);

    m_priority = new QTableWidgetItem("10");
    this->setItem(1, 0, new QTableWidgetItem("priority"));
    this->setItem(1, 1, m_priority);
    this->item(0,0)->setFlags(this->item(0,0)->flags() & ~Qt::ItemIsEditable);
    this->item(1,0)->setFlags(this->item(0,0)->flags() & ~Qt::ItemIsEditable);
    connect(Core::DocumentManager::instance(),&Core::DocumentManager::allDocumentsRenamed,
            this,&PropertyEditorWidget::onIdlNameChange);
}

PropertyEditorWidget::~PropertyEditorWidget()
{
}

void PropertyEditorWidget::loadFromDoc()
{
    QDomDocument* doc = qobject_cast<IdlDocument*>(m_editor->document())->getRootDoc();
    QDomElement deRootNode = doc->documentElement();
    QString oldname = deRootNode.attribute("project:name");
    m_idlname = m_editor->document()->filePath().fileName().split(".").first();
    if(m_idlname != oldname){
        onModify();
    }
    QString name = m_idlname.toLower();
    QString oid = deRootNode.attribute("oid");
    oid = oid.mid(0,oid.lastIndexOf(".") + 1);
    oid += name;
    deRootNode.setAttribute("oid", oid);
    deRootNode.setAttribute("project:name", m_idlname);

    QString priority = deRootNode.toElement().attribute("priority");
    if(!priority.isEmpty()){
        m_priority->setText(priority);
    } else {
        m_priority->setText("10");
    }
    m_oidEditWidget->setOid1(oid.split(".")[0] + "." + oid.split(".")[1]);
    m_oidEditWidget->setOid2("." + m_idlname.toLower());

    //open完后才会连接信号避免导入数据导致显示更改
    connect(this, &QTableWidget::itemChanged, this, &PropertyEditorWidget::onModify);
    connect(m_oidEditWidget, &OidEditWidget::modify, this, &PropertyEditorWidget::onModify);
}

void PropertyEditorWidget::exportToDoc()
{
    QDomDocument* doc = qobject_cast<IdlDocument*>(m_editor->document())->getRootDoc();
    QDomElement deRootNode = doc->documentElement();
    deRootNode.toElement().setAttribute("oid", m_oidEditWidget->getOid().toLower());
    deRootNode.toElement().setAttribute("priority", m_priority->text());
    deRootNode.toElement().setAttribute("project:name", m_idlname);
}

void PropertyEditorWidget::onIdlNameChange(const QString& from, const QString& to)
{
    //判断是否是自己的路径变更
    if(qobject_cast<IdlDocument*>(m_editor->document())->filePath().toString() == to){
        loadFromDoc();
        onModify();
    }
}

void PropertyEditorWidget::onModify()
{
    qobject_cast<TextEditor::TextDocument*>(m_editor->document())->document()->setModified(true);
}

}}
