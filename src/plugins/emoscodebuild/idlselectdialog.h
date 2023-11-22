#ifndef IDLSELECTDIALOG_H
#define IDLSELECTDIALOG_H

#include <QDialog>

namespace Ui {
class IdlSelectDialog;
}

class IdlSelectDialog : public QDialog
{
    Q_OBJECT

public:
    explicit IdlSelectDialog(const QString& filePath, QWidget *parent = nullptr);
    ~IdlSelectDialog();

    QList<QString> getNoSelectList();
private:
    Ui::IdlSelectDialog *ui;
};

#endif // IDLSELECTDIALOG_H
