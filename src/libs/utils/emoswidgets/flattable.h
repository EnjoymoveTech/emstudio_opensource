#ifndef FLATTABLE_H
#define FLATTABLE_H

#include <utils/utils_global.h>
#include <QToolTip>
#include <QTableWidget>

namespace EmosWidgets
{

class FlatTablePrivate;
class EMSTUDIO_UTILS_EXPORT FlatTable : public QTableWidget
{
    Q_OBJECT
public:
    FlatTable(QWidget* parent = nullptr);
    ~FlatTable();
    void setHeaderSize(const QVector<int>& w);
    void appendRowItems(QList<QTableWidgetItem*> items);
    void appendRowItems(QStringList items);
    //lingfei.yu show text in line row
    void setItemShowtext(QModelIndex index, const QString text);
signals:
    void sigEdit(const QModelIndex &index);
public slots:
    void slot_tooltip(QModelIndex index);
private:
    QScopedPointer<FlatTablePrivate> d;
    QMap<QModelIndex,QString> m_showtext;
    virtual bool edit(const QModelIndex &index, EditTrigger trigger, QEvent *event) override;
    void setTableStyle();
};

}

#endif // FLATTABLE_H
