#ifndef INJADEBUGDIALOG_H
#define INJADEBUGDIALOG_H

#include <QDialog>

namespace Ui {
class InjaDebugDialog;
}
namespace EmosCodeBuild {

namespace Internal {
}
class InjaDebugDialog : public QDialog
{
    Q_OBJECT

public:
    explicit InjaDebugDialog(QWidget *parent = nullptr);
    ~InjaDebugDialog();

    void showMsg(const QString& msg);
public slots:
    void button_start(bool);
private:
    Ui::InjaDebugDialog *ui;
};
}
#endif // INJADEBUGDIALOG_H
