#include "injadebugdialog.h"
#include "ui_injadebugdialog.h"

#include "someipbuild.h"
#include "injamanager.h"
#include <QDomDocument>
#include <emostools/cfvparser.h>

namespace EmosCodeBuild {

namespace Internal {
}

InjaDebugDialog::InjaDebugDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::InjaDebugDialog)
{
    ui->setupUi(this);

    connect(ui->button_start, &QPushButton::clicked, this, &InjaDebugDialog::button_start);
}

InjaDebugDialog::~InjaDebugDialog()
{
    delete ui;
}

void InjaDebugDialog::showMsg(const QString &msg)
{
    ui->logEdit->appendPlainText(msg);
}

void InjaDebugDialog::button_start(bool)
{
    if(ui->edit_idl->text().isEmpty() || ui->edit_build->text().isEmpty() || ui->edit_temp->text().isEmpty())
    {
        showMsg("input path invalid");
        return;
    }
}



}
