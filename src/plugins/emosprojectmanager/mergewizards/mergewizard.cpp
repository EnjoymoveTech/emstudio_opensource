#include "mergewizard.h"
#include "basewizardpage.h"
#include "idlwizardpage.h"
#include "cfvwizardpage.h"
#include "typewizardpage.h"
#include "apiwizardpage.h"

MergeWizard::MergeWizard(QWidget* parent) :
    QWizard(parent)
{
    setPage(PAGE_BASE,new BaseWizardPage(this));
    setPage(PAGE_TYPE,new TypeWizardPage(this));
    setPage(PAGE_API,new ApiWizardPage(this));
    setPage(PAGE_IDL,new IdlWizardPage(this));
    setMaximumSize(1400,600);
    setMinimumSize(1100,400);
}

void MergeWizard::initPages()
{
    //setPage(PAGE_CFV,new CfvWizardPage(this));

    //setAttribute(Qt::WA_DeleteOnClose);
}

void MergeWizard::setProjectPath(QString projectPath)
{
    m_projectPath = projectPath;
    dynamic_cast<BaseWizardPage*>(page(PAGE_BASE))->setProjectPath(projectPath);
    //m_mergedProjectPath = mergedProjectPath;
}

QString MergeWizard::mergedProjectPath()
{
    return m_mergedProjectPath;
}
