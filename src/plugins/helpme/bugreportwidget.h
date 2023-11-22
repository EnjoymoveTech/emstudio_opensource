#ifndef BugReportWidget_H
#define BugReportWidget_H

#include <QWidget>
#include <QStringListModel>
#include <QDragEnterEvent>
#include "QtGui/private/qzipwriter_p.h"

namespace Ui {
class BugReportWidget;
}

namespace Helpme {
namespace Internal {

class BugReportWidget : public QWidget
{
    Q_OBJECT

public:
    explicit BugReportWidget(QWidget *parent = nullptr);
    ~BugReportWidget();
protected:
    void dragEnterEvent(QDragEnterEvent* event);
    void dropEvent(QDropEvent* event);

    void closeEvent(QCloseEvent* event);
public slots:
    void slotCancelButtonClicked();
    void slotClearEnclosureButtonClicked();
    void slotReportButtonClicked();

private:
    void startReport();
    void endReport();
    void addEnclosure(QString fileName);
    void addDictionary(QZipWriter* writer,QString dirPath,QString subDirName);

    Ui::BugReportWidget *ui;
    QStringListModel* m_listModel;
    class PostMessager* m_manager = nullptr;
};
}
}

#endif // BugReportWidget_H
