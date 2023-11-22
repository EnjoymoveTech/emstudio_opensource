#ifndef MERGEWIZARDPAGE_H
#define MERGEWIZARDPAGE_H

#include <QWizardPage>
#include <QListView>
#include <QTreeView>
#include <QStringListModel>
#include <QStandardItemModel>
#include <QRadioButton>

typedef enum DataStatus
{
    STATUS_EQUAL = 0,
    STATUS_ADD,
    STATUS_MODIFY,
    STATUS_REMOVE
}DataStatus;

typedef enum Chosen
{
    NO_CHOSEN = 0,
    CURRENT,
    MERGED
}Chosen;

class MergeWizardPage : public QWizardPage
{
    Q_OBJECT

public:
    MergeWizardPage(QWidget* parent = Q_NULLPTR);
    void UIInit();
    void setModel();

    QStringListModel* StringListModel();
    QStandardItemModel* CurrentModel();
    QStandardItemModel* MergedModel();
    QListView* ConflictView();
    QRadioButton* CurrentButton();
    QRadioButton* MergedButton();
    //virtual void update();
signals:
    void sigConflictViewClicked(const QModelIndex& index);
    void sigRadioButtonClicked();

public slots:


private:
    QListView* m_conflictView;
    QTreeView* m_currentView;
    QTreeView* m_mergedView;
    QRadioButton* m_currentButton;
    QRadioButton* m_mergedButton;

    QStringListModel* m_conflictModel;
    QStandardItemModel* m_currentModel;
    QStandardItemModel* m_mergedModel;

};

#endif // MERGEWIZARDPAGE_H
