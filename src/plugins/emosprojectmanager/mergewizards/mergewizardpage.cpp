#include "mergewizardpage.h"

#include <QVBoxLayout>
#include <QHBoxLayout>

MergeWizardPage::MergeWizardPage(QWidget* parent) :
    QWizardPage(parent)
{
    m_conflictView = new QListView(parent);
    m_currentView = new QTreeView(parent);
    m_mergedView = new QTreeView(parent);
    m_currentButton = new QRadioButton("current:",parent);
    m_mergedButton = new QRadioButton("merged:",parent);

    m_conflictModel = new QStringListModel(m_conflictView);


    m_conflictView->setModel(m_conflictModel);


    m_conflictView->setMaximumWidth(150);
    m_conflictView->setMinimumWidth(150);

    m_currentButton->setMaximumWidth(100);
    m_currentButton->setMinimumHeight(20);
    m_mergedButton->setMaximumWidth(100);
    m_mergedButton->setMinimumHeight(20);
    m_currentView->hide();
    m_mergedView->hide();
    m_currentButton->hide();
    m_mergedButton->hide();

    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->addWidget(m_conflictView);
    layout->addStretch(2);
    //layout->addWidget(m_currentView);
    //layout->addStretch(1);
    //layout->addWidget(m_mergedView);
    setLayout(layout);
    //connect(m_conflictView,&QListView::clicked,this,&MergeWizardPage::slotConflictViewClicked);

}

void MergeWizardPage::UIInit()
{
    m_currentModel = new QStandardItemModel(m_currentView);
    m_mergedModel= new QStandardItemModel(m_mergedView);

    m_currentView->setModel(m_currentModel);
    m_mergedView->setModel(m_mergedModel);

    m_currentModel->setColumnCount(2);
    m_mergedModel->setColumnCount(2);
    m_currentModel->setHorizontalHeaderLabels(QStringList()<<"object"<<"detail");
    m_mergedModel->setHorizontalHeaderLabels(QStringList()<<"object"<<"detail");

    m_currentView->setMaximumWidth(500);
    m_currentView->setMinimumWidth(500);
    m_mergedView->setMaximumWidth(500);
    m_mergedView->setMinimumWidth(500);
    m_currentButton->setMaximumWidth(100);
    m_currentButton->setMinimumHeight(20);
    m_mergedButton->setMaximumWidth(100);
    m_mergedButton->setMinimumHeight(20);

    //m_currentView->setEnabled(true);
    //m_mergedView->setEnabled(true);

    //m_currentView->setColumnWidth(0,200);
    //m_mergedView->setColumnWidth(0,200);

    QVBoxLayout* currentlayout = new QVBoxLayout;
    currentlayout->addWidget(m_currentButton);
    currentlayout->addWidget(m_currentView);
    ((QHBoxLayout*)layout())->addLayout(currentlayout);

    ((QHBoxLayout*)layout())->addStretch(1);

    QVBoxLayout* mergedlayout = new QVBoxLayout;
    mergedlayout->addWidget(m_mergedButton);
    mergedlayout->addWidget(m_mergedView);
    ((QHBoxLayout*)layout())->addLayout(mergedlayout);

    m_currentView->show();
    m_mergedView->show();
    m_currentButton->show();
    m_mergedButton->show();

    m_currentView->expandAll();
    m_mergedView->expandAll();

    connect(ConflictView(),&QListView::clicked,this,&MergeWizardPage::sigConflictViewClicked);
    connect(CurrentButton(),&QRadioButton::clicked,this,&MergeWizardPage::sigRadioButtonClicked);
    connect(MergedButton(),&QRadioButton::clicked,this,&MergeWizardPage::sigRadioButtonClicked);
}

void MergeWizardPage::setModel()
{
    m_currentView->setModel(m_currentModel);
    m_mergedView->setModel(m_mergedModel);
}

QStringListModel* MergeWizardPage::StringListModel()
{
    return m_conflictModel;
}

QStandardItemModel* MergeWizardPage::CurrentModel()
{
    return m_currentModel;
}

QStandardItemModel* MergeWizardPage::MergedModel()
{
    return m_mergedModel;
}

QListView* MergeWizardPage::ConflictView()
{
    return m_conflictView;
}

QRadioButton* MergeWizardPage::CurrentButton()
{
    return m_currentButton;
}

QRadioButton* MergeWizardPage::MergedButton()
{
    return m_mergedButton;
}



