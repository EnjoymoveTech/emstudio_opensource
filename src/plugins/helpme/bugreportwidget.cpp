#include "bugreportwidget.h"
#include "ui_bugreportwidget.h"
#include "postmessager.h"
#include <QMimeData>
#include <QFileInfo>
#include <QUrl>
#include <QMessageBox>
#include <explorer/projecttree.h>
#include <explorer/projectnodes.h>
#include <explorer/project.h>

namespace Helpme {
namespace Internal {
BugReportWidget::BugReportWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::BugReportWidget)
{
    ui->setupUi(this);
    setAcceptDrops(true);
    m_manager = new PostMessager;
    m_listModel = new QStringListModel(ui->m_enclosureView);
    ui->m_enclosureView->setModel(m_listModel);
    ui->m_progressBar->hide();
    connect(ui->m_cancelButton,&QPushButton::clicked,this,&BugReportWidget::slotCancelButtonClicked);
    connect(ui->m_clearEnclosureButton,&QPushButton::clicked,this,&BugReportWidget::slotClearEnclosureButtonClicked);
    connect(ui->m_reportButton,&QPushButton::clicked,this,&BugReportWidget::slotReportButtonClicked);
}

BugReportWidget::~BugReportWidget()
{
    delete ui;
}

void BugReportWidget::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasUrls())
    {
        event->acceptProposedAction();
    }
}

void BugReportWidget::dropEvent(QDropEvent *event)
{
    QList<QUrl> urls = event->mimeData()->urls();
    foreach (const QUrl& url,urls)
    {
        QString fileName = url.toLocalFile();
        addEnclosure(fileName);
    }
    event->accept();
}

void BugReportWidget::closeEvent(QCloseEvent *event)
{
    if (m_manager->isConnected())
    {
        QMessageBox::critical(this,"critical","can't close while reporting!");
        event->ignore();
        return;
    }
    event->accept();
}

void BugReportWidget::slotCancelButtonClicked()
{
    close();
}

void BugReportWidget::slotClearEnclosureButtonClicked()
{
    m_listModel->removeRows(0,m_listModel->rowCount());
}

void BugReportWidget::slotReportButtonClicked()
{
    QString errorString = "The following file is lost:\n";
    bool flag = true;
    if (ui->m_presenterEdit->text().isEmpty())
    {
        QMessageBox::critical(this,"Name missing","Please Enter the Name");
        return;
    }
    foreach (const QString& str,m_listModel->stringList())
    {
        if (!QFileInfo::exists(str))
        {
            errorString += str + "\n";
            flag = false;
        }
    }
    errorString += "Please check these files before report!";
    if (flag)
    {
        startReport();
        m_manager->postGetID();
        if (!m_manager->isConnected())
        {
            QMessageBox::critical(this,"get id failed","get id timeout!");
            endReport();
            return;
        }
        ProjectExplorer::Project *project = ProjectExplorer::ProjectTree::currentProject();
        QStringList list = m_listModel->stringList();
        if (project)
        {
            if (!project->projectDirectory().toString().isEmpty())
            {
                list<<project->projectDirectory().toString() + ".zip";
            }
        }
        m_manager->postJson(m_manager->id(),ui->m_priorityBox->currentIndex(),ui->m_presenterEdit->text(),ui->m_problemEdit->toPlainText(),list);
        if (!m_manager->isConnected())
        {
            QMessageBox::critical(this,"post json failed","post json timeout!");
            endReport();
            return;
        }
        if (project)
        {
            if (!project->projectDirectory().toString().isEmpty())
            {
                QString ProjectDirPath = project->projectDirectory().toString();
                QDir projectDir(ProjectDirPath);
                QZipWriter writer(ProjectDirPath + ".zip");
                addDictionary(&writer,ProjectDirPath, ".emos");
                addDictionary(&writer,ProjectDirPath, "1_interface_design");
                addDictionary(&writer,ProjectDirPath, "2_component_design");
                addDictionary(&writer,ProjectDirPath, "3_architecture_design");
                addDictionary(&writer,ProjectDirPath, "config");
                addDictionary(&writer,ProjectDirPath, "schedule_configuration");
                QFile projectFile(project->projectFilePath().toString());
                if (!projectFile.open(QIODevice::ReadOnly))
                {
                    qDebug()<<"Compress Dir failed";
                    return;
                }
                writer.addFile(projectDir.dirName() + "/" + project->projectFilePath().toString().split("/").back(),&projectFile);
                projectFile.close();
                writer.close();
                m_manager->postFile(ProjectDirPath + ".zip");
            }
        }
        foreach (const QString& fileName,m_listModel->stringList())
        {
            m_manager->postFile(fileName);
        }
        QMessageBox::information(this,"report success!","report success!");
        if (project)
        {
            QFile::remove(project->projectDirectory().toString() + ".zip");
        }
        endReport();
    }
    else if (!flag)
    {
        QMessageBox::critical(this,"file missing",errorString);
        return;
    }
}

void BugReportWidget::startReport()
{
    qDebug()<<"Start report!";
    m_manager->setConnected(true);
    ui->m_cancelButton->setEnabled(false);
    ui->m_reportButton->setEnabled(false);
    ui->m_clearEnclosureButton->setEnabled(false);
}

void BugReportWidget::endReport()
{
    qDebug()<<"End report";
    m_manager->setConnected(false);
    ui->m_cancelButton->setEnabled(true);
    ui->m_reportButton->setEnabled(true);
    ui->m_clearEnclosureButton->setEnabled(true);
}

void BugReportWidget::addEnclosure(QString fileName)
{
    m_listModel->insertRow(m_listModel->rowCount());
    QModelIndex index = m_listModel->index(m_listModel->rowCount() - 1);
    m_listModel->setData(index,fileName);
    ui->m_enclosureView->setCurrentIndex(index);
}

void BugReportWidget::addDictionary(QZipWriter* writer,QString dirPath,QString subDirName)
{
    if (!writer)
    {
        return;
    }
    QDir dir(dirPath + "/" + subDirName);
    dir.setFilter(QDir::Files | QDir::NoDotAndDotDot | QDir::NoSymLinks);
    QFileInfoList fileList = dir.entryInfoList();
    if (fileList.isEmpty())
    {
        return;
    }
    writer->addDirectory(QDir(dirPath).dirName() + "/" + subDirName);
    foreach (const QFileInfo& fileInfo,fileList)
    {
        QFile file(fileInfo.absoluteFilePath());
        if (!file.open(QIODevice::ReadOnly))
        {
            qDebug()<<"open file:"<<fileInfo.absoluteFilePath()<<" failed!";
            continue;
        }
        writer->addFile(QDir(dirPath).dirName() + "/" + subDirName + "/" + fileInfo.fileName(),&file);
        file.close();
    }
}

}
}
