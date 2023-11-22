#ifndef BASEWIZARDPAGE_H
#define BASEWIZARDPAGE_H

#include <QWizardPage>
#include <QLabel>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QPushButton>

class BaseWizardPage : public QWizardPage
{
    Q_OBJECT
public:
    BaseWizardPage(QWidget* parent = Q_NULLPTR);
    ~BaseWizardPage();
    void setProjectPath(QString path);
public slots:
    void slotGetBaseProjectPath();
    void slotGetCurrentProjectPath();
    void slotGetMergedProjectPath();

    void initializePage() override;

    bool validatePage() override;
private:
    QString m_baseProjectPath;
    QString m_currentProjectPath;
    QString m_mergedProjectPath;

    QLabel* m_baseLabel;
    QLineEdit* m_baseEdit;
    QPushButton* m_baseButton;

    QLabel* m_currentLabel;
    QLineEdit* m_currentEdit;
    QPushButton* m_currentButton;

    QLabel* m_mergedLabel;
    QLineEdit* m_mergedEdit;
    QPushButton* m_mergedButton;
};

#endif // BASEWIZARDPAGE_H
