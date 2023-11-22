#include "flattable.h"

#include "widgetsstyle.h"
#include "widgetstheme.h"

#include <QHeaderView>
#include <QStyledItemDelegate>

namespace EmosWidgets
{

class FlatTabDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    FlatTabDelegate(const QVector<int>& w, QWidget* parent = 0);
    void setHeaderSize(const QVector<int>& w);
protected:
    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;
private:
    QVector<int> m_width;
};

FlatTabDelegate::FlatTabDelegate(const QVector<int>& w, QWidget* parent) : QStyledItemDelegate(parent)
{
    m_width = w;
}

void FlatTabDelegate::setHeaderSize(const QVector<int>& w)
{
    m_width = w;
}

void FlatTabDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if(m_width.size() > index.column())
    {
        int sum = 0;
        for(auto i : m_width)
            sum += i;

        QTableView* pTable =  static_cast<QTableView*> (this->parent());
        if (pTable != nullptr) {

            for(int i = 0; i < m_width.size(); ++i)
            {
                if (index.column() == i) {  //列标为0
                    int width = int(pTable->width() * m_width[i] * 1.0 / sum)-5;    //按比例设置列宽
                    pTable->setColumnWidth(i, width);   //设置列宽
                    //pTable->setRowHeight(index.row(), 20);  //设置行高
                }
            }
        }
    }

    QStyledItemDelegate::paint(painter, option, index);
}

class FlatTablePrivate
{
public:
    int textSize = 12;
    int headerTextSize = 14;
};


FlatTable::FlatTable(QWidget* parent) :
    QTableWidget(parent),
    d(new FlatTablePrivate)
{
    QFont font("Roboto");
    font.setPixelSize(d->textSize);
    setFont(font);
    horizontalHeader()->setStretchLastSection(true);//横向最后一列充满
    horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    QFont headerFont("Roboto");
    headerFont.setPixelSize(d->headerTextSize);
    horizontalHeader()->setFont(headerFont);
    verticalHeader()->setVisible(false);//去掉纵向表头
    setItemDelegate(new FlatTabDelegate(QVector<int>(), this));
    setMouseTracking(true);
    setTableStyle();
    connect(this, SIGNAL(entered(QModelIndex)), this, SLOT(slot_tooltip(QModelIndex)));
    //    setStyleSheet(R"(
    //设置表头样式（无内容部分）
    //QHeaderView
    //{
    //    background:transparent;     	//表头背景颜色
    //}
    //设置表头部分样式（有内容部分）
    //QHeaderView::section
    //{
    //    font-size:17px;             //字体大小
    //    font-family:"Microsoft YaHei";	//字体样式
    //    color:#FFFFFF;                 //字体颜色
    //    background-color:#ADADAD;     //背景颜色
    //    border:none;				//边框 none无边框
    //    text-align:center;              //对齐方式
    //    min-height:35px;
    //    max-height:35px;
    //    margin-left:0px;
    //    padding-left:0px;
    //}
    //表格样式
    //QTableWidget{
    //alternate-background-color:rgb(192,192,192);	/*背景交错颜色*/
    //***************注意**************
    //ui->tableWidget->setAlternatingRowColors(true);		//允许绘制背景色交替 必须写这句否则交替背景色没有效果
    //*****************************************/
    //background:rgb(255,255,255);
    //color:#DCDCDC;
    //border:1px solid #242424;
    //gridline-color:#242424;
    //selection-color:#242424;	/*选中行的字体颜色*/
    //selection-background-color:#242424;	/*选中行的背景颜色*/
    //}
    //单元格样式
    //QTableWidget::item
    //{
    //    border-bottom:1px solid #EEF1F7 ; 	//item每个单元格只显示下边线
    //}
    //单元格被选中时的样式
    //QTableWidget::item::selected
    //{
    //    color:rgb(51,103,204);
    //    background:#FFFFFF;
    //}
    //滚动条样式
    //QScrollBar::handle:vertical
    //{
    //    background: rgba(51,103,204,25%);
    //    border: 0px solid grey;
    //    border-radius:3px;
    //    width: 8px;
    //}
    //QScrollBar::handle:vertical:hover
    //{
    //    background: rgba(51,103,204,50%);
    //    border: 0px solid grey;
    //    border-radius:3px;
    //    width: 8px;
    //}
    //QScrollBar::vertical
    //{
    //    border-width:1px;
    //    border-style: solid;
    //    border-color: rgba(255, 255, 255, 10%);
    //    width: 8px;
    //    margin:0px 0px 0px 0px;
    //    border-radius:3px;
    //}
    //QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical
    //{
    //    background:rgba(255,255,255,10%);
    //}

    //QScrollBar::add-line:vertical {
    //    height:6px;width:6px;
    //    border-image:url(:/images/a/3.png);
    //    subcontrol-position:bottom;
    //}
    //QScrollBar::sub-line:vertical {
    //    height:6px;width:6px;
    //    border-image:url(:/images/a/1.png);
    //    subcontrol-position:top;
    //}
    //    )");
}

FlatTable::~FlatTable()
{

}

void FlatTable::setHeaderSize(const QVector<int> &w)
{
    horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    qobject_cast<FlatTabDelegate*>(this->itemDelegate())->setHeaderSize(w);
}

// lingfei.yu 末尾新增一行的所有items至每一列
void FlatTable::appendRowItems(QList<QTableWidgetItem *> items)
{
    this->insertRow(this->rowCount());
    for(int i = 0;i < this->columnCount();i++)
    {
        if(items.size() > i)
        {
            this->setItem(this->rowCount() -1,i,items.at(i));
        }
    }
}

// lingfei.yu 末尾新增一行的所有items字符串至每一列
void FlatTable::appendRowItems(QStringList items)
{
    this->insertRow(this->rowCount());
    for(int i = 0;i < this->columnCount();i++)
    {
        if(items.size() > i)
        {
            QTableWidgetItem *item = new QTableWidgetItem(items.at(i));
            this->setItem(this->rowCount() -1,i,item);
        }
    }
}
//lingfei.yu 设置默认显示的内容，若无，则显示框内内容
void FlatTable::setItemShowtext(QModelIndex index, const QString text)
{
    m_showtext.insert(index,text);
}
//lingfei.yu 接受到显示的信号
void FlatTable::slot_tooltip(QModelIndex index)
{
    if(m_showtext.contains(index))
    {
        QToolTip::showText(QCursor::pos(),m_showtext.value(index));
    }else {
        QToolTip::showText(QCursor::pos(),index.data().toString());
    }
    return ;
}

bool FlatTable::edit(const QModelIndex &index, EditTrigger trigger, QEvent *event)
{
    emit sigEdit(index);
    return QAbstractItemView::edit(index, trigger, event);
}

void FlatTable::setTableStyle()
{
    QStringList list;

    //设置表头样式（无内容部分）
    //QHeaderView
    //{
    //    background:transparent;     	//表头背景颜色
    //}
    //设置表头部分样式（有内容部分）
    //QHeaderView::section
    //{
    //    font-size:17px;             //字体大小
    //    font-family:"Microsoft YaHei";	//字体样式
    //    color:#FFFFFF;                 //字体颜色
    //    background-color:#ADADAD;     //背景颜色
    //    border:none;				//边框 none无边框
    //    text-align:center;              //对齐方式
    //    min-height:35px;
    //    max-height:35px;
    //    margin-left:0px;
    //    padding-left:0px;
    //}

    //    list.append(QString(R"(
    //                        QHeaderView
    //                        {
    //                            background:transparent;
    //                        }
    //                        )"));
    list.append(QString(R"(
                        QHeaderView:section
                        {
                            border:1px groove gray;
                            background-color:#1296db;
                            color:#ffffff;
                        }
                        )"));

    list.append(QString(R"(
    QTableWidget::item::selected
    {
        color:rgb(51,103,204);
        background:#FFFFFF;
    }
                        )"));



    QString qss = list.join("");
    this->setStyleSheet(qss);
}

}

#include "flattable.moc"
