#ifndef MERGEWIZARD_H
#define MERGEWIZARD_H

#include <QWizard>

class MergeWizard : public QWizard
{
    Q_OBJECT
public:
    MergeWizard(QWidget* parent = Q_NULLPTR);
    void initPages();
    void setProjectPath(QString projectPath);
    QString mergedProjectPath();

    typedef enum PageID
    {
        PAGE_BASE = 0,
        PAGE_TYPE,
        PAGE_API,
        PAGE_IDL,
        PAGE_CFV
    }PageID;

    //void initializePage(int id) override;
    //void cleanupPage(int id) override;
    //bool validateCurrentPage() override;
    //int nextId() override;

private:
    QString m_projectPath;
    QString m_mergedProjectPath;
};

#endif // MERGEWIZARD_H
