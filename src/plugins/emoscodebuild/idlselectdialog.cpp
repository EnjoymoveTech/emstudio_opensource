#include "idlselectdialog.h"
#include "ui_idlselectdialog.h"
#include <emostools/cfvparser.h>
#include <QListWidgetItem>

IdlSelectDialog::IdlSelectDialog(const QString& filePath, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::IdlSelectDialog)
{
    ui->setupUi(this);

    setWindowTitle("Build Select");

    connect(ui->buttonOK, &QPushButton::clicked, [&](bool){
       QDialog::accept();
    });
    connect(ui->buttonCancel, &QPushButton::clicked, [&](bool){
       QDialog::reject();
    });

    QList<QString> oidList = EmosTools::CfvParser::getOidListFromCfv(filePath);
    for(const auto& i : oidList)
    {
        QListWidgetItem* item = new QListWidgetItem(i);
        item->setCheckState(Qt::Checked);
        ui->listWidget->addItem(item);
    }
}

IdlSelectDialog::~IdlSelectDialog()
{
    delete ui;
}

QList<QString> IdlSelectDialog::getNoSelectList()
{
    QList<QString> noSelectList;
    for(int i = 0; i < ui->listWidget->count(); ++i)
    {
        if(ui->listWidget->item(i)->checkState() == Qt::Unchecked)
            noSelectList.push_back(ui->listWidget->item(i)->text());
    }
    return noSelectList;
}
