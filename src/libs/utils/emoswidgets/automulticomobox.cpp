#include "automulticomobox.h"

#include "widgetsstyle.h"
#include "widgetstheme.h"

#include <QAbstractItemView>
#include <QStringListModel>
#include <QSortFilterProxyModel>
#include <QCompleter>
#include <QLineEdit>
#include <QKeyEvent>

namespace EmosWidgets
{

class AutoMultiComoBoxFilterModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    AutoMultiComoBoxFilterModel(QObject *parent = nullptr) : QSortFilterProxyModel(parent) {}

    void setCurValue(const QString text)
    {
        m_curValue = text;
    }
    void setSeparator(const QString c)
    {
        m_sepStr = c;
    }
protected:
    virtual bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override
    {
        if (!source_parent.isValid()) {
            const QModelIndex &categoryIndex = sourceModel()->index(source_row, 0, source_parent);
            const QRegExp &regexp = filterRegExp();


            if(regexp.pattern().split(m_sepStr).size() > 1)
            {
                const QRegExp regexp2(regexp.pattern().split(m_sepStr).last(), regexp.caseSensitivity());
                if (regexp2.indexIn(sourceModel()->data(categoryIndex, filterRole()).toString()) != -1)
                    return true;

                return false;
            }

            if (regexp.indexIn(sourceModel()->data(categoryIndex, filterRole()).toString()) != -1)
                return true;

            return false;
        }
        return QSortFilterProxyModel::filterAcceptsRow(source_row, source_parent);
    }

private:

    QString m_curValue;
    QString m_sepStr {","};
};

class AutoMultiComoBoxPrivate : public QObject
{
    Q_OBJECT
public:
    AutoMultiComoBoxPrivate(AutoMultiComoBox* _q) : q(_q){}

    AutoMultiComoBoxFilterModel* m_filterModel = nullptr;
    QStringListModel* m_model = nullptr;
    QCompleter* m_completer = nullptr;
    QLineEdit* m_editor = nullptr;
    AutoMultiComoBox* q = nullptr;
    QString m_sepStr {","};

public slots:
    void slotTextChanged(const QString &);
    void slotCompActivated(const QString &text);
};

void AutoMultiComoBoxPrivate::slotTextChanged(const QString & text)
{
    m_filterModel->setFilterRegExp(text);
}

void AutoMultiComoBoxPrivate::slotCompActivated(const QString & text)
{
    QString textStr;
    QStringList strList = m_editor->text().split(m_sepStr);
    for(int i = 0; i < strList.size() - 1; i++)
    {
        textStr += strList[i] + m_sepStr;
    }
    textStr += text + m_sepStr;
    m_editor->setText(textStr);
    m_completer->complete(q->rect()); // popup it up!
}

AutoMultiComoBox::AutoMultiComoBox(QWidget* parent) :
    QComboBox(parent),
    d(new AutoMultiComoBoxPrivate(this))
{
    setEditable(true);

    d->m_completer = new QCompleter(this);
    d->m_completer->setCompletionMode(QCompleter::UnfilteredPopupCompletion);
    d->m_completer->setWidget(this);

    d->m_model = new QStringListModel(d->m_completer);
    d->m_filterModel = new AutoMultiComoBoxFilterModel(d->m_completer);
    d->m_filterModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    d->m_filterModel->setFilterKeyColumn(0);
    d->m_filterModel->setSourceModel(d->m_model);

    d->m_completer->setModel(d->m_filterModel);

    d->m_editor = new QLineEdit(this);

    setLineEdit(d->m_editor);

    connect(d->m_editor, &QLineEdit::textChanged, d.get(), &AutoMultiComoBoxPrivate::slotTextChanged);
    connect(d->m_completer, QOverload<const QString &>::of(&QCompleter::activated), d.get(), &AutoMultiComoBoxPrivate::slotCompActivated);
}

AutoMultiComoBox::~AutoMultiComoBox()
{

}

void AutoMultiComoBox::setCompleteList(const QStringList &list)
{
    d->m_model->setStringList(list);
}

QStringList AutoMultiComoBox::getCompleteList() const
{
    return d->m_model->stringList();
}

void AutoMultiComoBox::setSeparator(const QString &c)
{
    d->m_sepStr = c;
    d->m_filterModel->setSeparator(c);
}

void AutoMultiComoBox::showPopup()
{
    d->m_completer->complete(rect()); // popup it up!
}

void AutoMultiComoBox::hidePopup()
{
    d->m_completer->popup()->hide();
}

void AutoMultiComoBox::keyPressEvent(QKeyEvent *e)
{
    if (d->m_completer && d->m_completer->popup()->isVisible()) {
        // The following keys are forwarded by the completer to the widget
       switch (e->key()) {
       //case Qt::Key_Enter:
       //case Qt::Key_Return:
       case Qt::Key_Escape:
       case Qt::Key_Tab:
       case Qt::Key_Backtab:
            e->ignore();
            return; // let the completer do default behavior
       default:
           break;
       }
    }
    d->m_completer->complete(rect()); // popup it up!

    QComboBox::keyPressEvent(e);
}

}

#include "automulticomobox.moc"
