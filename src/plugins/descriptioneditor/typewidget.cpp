#include "typewidget.h"
#include "descriptioneditorconstants.h"
#include "typedatawidget.h"
#include "typedetailwidget.h"
#include "descriptioneditor.h"

#include <emostools/type_description_parser.h>
#include <utils/styledbar.h>
#include <utils/utilsicons.h>
#include <coreplugin/actionmanager/command.h>

#include <QHBoxLayout>
#include <QPushButton>
#include <QListWidget>
#include <QDockWidget>

#include <QDomText>
#include <QToolButton>
#include <QFileDialog>
#include <QProcess>
#include <xlsx/xlsxdocument.h>
#include "fileparser/excelparser.h"
#include "fileparser/excelgenerator.h"
#include "typestructwidget.h"
#include <coreplugin/messagemanager.h>
#include <fileparser/headerparser.h>
#include <explorer/projecttree.h>
#include <explorer/project.h>

using namespace EmosTools;

#include "descriptioneditor.h"

namespace DescriptionEditor {
namespace Internal {

TypeWidget::TypeWidget(DescriptionEditor* editor, QWidget *parent) :
    DescriptionEditorWidget(editor, parent)
{
    init();
}

TypeWidget::~TypeWidget()
{

}

void TypeWidget::init()
{
    QHBoxLayout* layout = new QHBoxLayout(mainWidget);
    layout->setContentsMargins(0,0,0,0);
    layout->setSpacing(0);

    //m_stackedWidget = new QStackedWidget(this);
    m_structWidget = new TypeStructWidget(this);
    layout->addWidget(m_structWidget);

    projectDirectory = ProjectExplorer::ProjectTree::currentProject()->projectDirectory().toString();
}

QDomDocument TypeWidget::saveWidget()
{
    return m_structWidget->saveWidget();
}

void TypeWidget::slotImportCover()
{
    slotImport("Cover");
}

void TypeWidget::slotImportMerge()
{
    slotImport("Merge");
}

void TypeWidget::slotExport()
{
    QString defaultPath = projectDirectory + "/config/DataDetails_Archive.xlsx";
    QString fileName = QFileDialog::getSaveFileName(mainWidget, QStringLiteral("Choose export excel:"), defaultPath, QStringLiteral("export file(*xlsx)"));
    if(fileName == "")
    {
        return;
    }

    if(QFile::exists(fileName))
    {
        QFile::remove(fileName);
    }
    ExcelGenerator excel;
    excel.setData(fileName, &m_doc);
}

void TypeWidget::slotImport(QString mode)
{
    QString defaultPath = projectDirectory + "/config";
    QString dialogTitle;
    QString iconText;
    if(mode == Cover)
    {
        dialogTitle = "Choose file to cover:";
        iconText = "cover file (*h);;cover file (*xlsx)";
    }
    else if(mode == Merge)
    {
        dialogTitle = "Choose file to merge:";
        iconText = "merge file (*h);;merge file ( *xlsx)";
    }
    QStringList fileNameList = QFileDialog::getOpenFileNames(mainWidget, dialogTitle, defaultPath, iconText);

    if(fileNameList.size() == 0)
    {
        return;
    }

    QDomNode backupNode = m_doc.cloneNode(true);
    HeaderParser* headerParser = new HeaderParser();
    headerParser->getFileNameList(fileNameList);
    if(!headerParser->getInfoFromHeader(m_doc, mode))
    {
        m_doc.replaceChild(backupNode, m_doc.firstChild());
    }
    delete headerParser;
    headerParser = nullptr;

    for(int i = 0; i < fileNameList.size(); i++)
    {
        if(fileNameList[i].endsWith(".xlsx"))
        {
            ExcelParser excelParser;
            if(!excelParser.setData(fileNameList[i], m_doc, mode))
            {
                m_doc.replaceChild(backupNode, m_doc.firstChild());
                QString errLog = "LoadError: Load file " + fileNameList[i] + " failed!";
                Core::MessageManager::instance()->writeWithTime(errLog, Utils::ErrorMessageFormat);
            }
        }
    }
    writeXml(ProjectExplorer::ProjectTree::currentFilePath().toString());
    m_structWidget->loadWidget();
}

void TypeWidget::writeXml(QString fileName)
{
    if(QFile::exists(fileName))
    {
        QFile::remove(fileName);
    }
    QFile xml(fileName);
    if(!xml.open(QIODevice::ReadWrite))
    {
        QString errLog = "FileError: Can't open file " + fileName;
        Core::MessageManager::instance()->writeWithTime(errLog, Utils::ErrorMessageFormat);
        return;
    }
    QTextStream stream(&xml);
    m_doc.save(stream, 4, QDomNode::EncodingFromTextStream);
    xml.close();
}

void TypeWidget::loadWidget(QDomDocument* doc)
{
    m_doc = *doc;
    disconnect(m_structWidget, &TypeStructWidget::modify, this, &TypeWidget::slotModify);
    QString defaultPath = projectDirectory + "/config/struct.h";

    m_structWidget->getPath(defaultPath);
    m_structWidget->getDoc(m_doc);
    m_structWidget->loadWidget();
    connect(m_structWidget, &TypeStructWidget::modify, this, &TypeWidget::slotModify);
}

}}
