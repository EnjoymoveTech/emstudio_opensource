#include "basewizardpage.h"
#include <QFileDialog>
#include <QMessageBox>
#include "explorer/projecttree.h"
#include "explorer/projectnodes.h"
#include "explorer/project.h"


BaseWizardPage::BaseWizardPage(QWidget* parent) :
    QWizardPage(parent)
{
    setTitle("Choose base project");
    m_baseLabel = new QLabel("基础版本路径:",this);
    m_baseEdit = new QLineEdit(this);
    m_baseButton = new QPushButton("浏览",this);
    QHBoxLayout* baselayout = new QHBoxLayout;
    baselayout->addWidget(m_baseLabel);
    baselayout->addWidget(m_baseEdit);
    baselayout->addWidget(m_baseButton);
    //setLayout(baselayout);

    m_currentLabel = new QLabel("当前版本路径:",this);
    m_currentEdit = new QLineEdit(this);
    m_currentButton = new QPushButton("浏览",this);
    QHBoxLayout* currentlayout = new QHBoxLayout;
    currentlayout->addWidget(m_currentLabel);
    currentlayout->addWidget(m_currentEdit);
    currentlayout->addWidget(m_currentButton);
    //setLayout(currentlayout);

    m_mergedLabel = new QLabel("合并版本路径:",this);
    m_mergedEdit = new QLineEdit(this);
    m_mergedButton = new QPushButton("浏览",this);
    QHBoxLayout* mergedlayout = new QHBoxLayout;
    mergedlayout->addWidget(m_mergedLabel);
    mergedlayout->addWidget(m_mergedEdit);
    mergedlayout->addWidget(m_mergedButton);
    //setLayout(mergedlayout);

    QVBoxLayout* layout = new QVBoxLayout;
    layout->addLayout(baselayout);
    layout->addLayout(currentlayout);
    layout->addLayout(mergedlayout);
    setLayout(layout);

    connect(m_baseButton,&QPushButton::clicked,this,&BaseWizardPage::slotGetBaseProjectPath);
    connect(m_currentButton,&QPushButton::clicked,this,&BaseWizardPage::slotGetCurrentProjectPath);
    connect(m_mergedButton,&QPushButton::clicked,this,&BaseWizardPage::slotGetMergedProjectPath);

    registerField("baseProjectPath*",m_baseEdit);
    registerField("currentProjectPath*",m_currentEdit);
    registerField("mergedProjectPath*",m_mergedEdit);
}

BaseWizardPage::~BaseWizardPage()
{

}

void BaseWizardPage::setProjectPath(QString path)
{
    m_currentProjectPath = path;
}

void BaseWizardPage::slotGetBaseProjectPath()
{
    QString mergedFilePath = QFileDialog::getOpenFileName(Q_NULLPTR,"Choose merged file...",".","*.emos");
    if (mergedFilePath.isEmpty())
    {
        qDebug()<<"choose merged file failed";
        return;
    }
    m_baseProjectPath = QFileInfo(mergedFilePath).dir().path();
    m_baseEdit->setText(m_baseProjectPath);
}

void BaseWizardPage::slotGetCurrentProjectPath()
{
    QString currentFilePath = QFileDialog::getOpenFileName(Q_NULLPTR,"Choose current file...",".","*.emos");
    if (currentFilePath.isEmpty())
    {
        qDebug()<<"choose current file failed";
        return;
    }
    m_currentProjectPath = QFileInfo(currentFilePath).dir().path();
    m_currentEdit->setText(m_currentProjectPath);
}

void BaseWizardPage::slotGetMergedProjectPath()
{
    QString mergedFilePath = QFileDialog::getOpenFileName(Q_NULLPTR,"Choose merged file...",".","*.emos");
    if (mergedFilePath.isEmpty())
    {
        qDebug()<<"choose merged file failed";
        return;
    }
    m_mergedProjectPath = QFileInfo(mergedFilePath).dir().path();
    m_mergedEdit->setText(m_mergedProjectPath);
}

void BaseWizardPage::initializePage()
{
    m_baseEdit->setText(m_currentProjectPath);
    m_currentEdit->setText(m_currentProjectPath);
}

bool BaseWizardPage::validatePage()
{
    QString errorString;
    if (m_baseEdit->text().isEmpty())
    {
        errorString.append("Base project path is empty!\n");
    }
    if (m_currentEdit->text().isEmpty())
    {
        errorString.append("Current project path is empty!\n");
    }
    if (m_mergedEdit->text().isEmpty())
    {
        errorString.append("Merged project path is empty!\n");
    }
    if (!errorString.isEmpty())
    {
        QMessageBox::critical(this,"cirtical","Threr are some path error:\n"
                              + errorString
                              + "please fill the empty");
        return false;
    }
    return true;
}

