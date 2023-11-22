#include "autocomobox.h"

#include "widgetsstyle.h"
#include "widgetstheme.h"

#include <QStringListModel>
#include <QCompleter>

namespace EmosWidgets
{

class AutoComoBoxPrivate
{
public:
    QStringListModel* m_model;
    QCompleter* m_completer;
};


AutoComoBox::AutoComoBox(QWidget* parent) :
    QComboBox(parent),
    d(new AutoComoBoxPrivate)
{
    setEditable(true);

    d->m_model = new QStringListModel(this);
    d->m_completer = new QCompleter(d->m_model, this);
    d->m_completer->setFilterMode(Qt::MatchContains);//将匹配方式设置为 内容匹配
    d->m_completer->setCaseSensitivity(Qt::CaseInsensitive);//忽略大小写

    setCompleter(d->m_completer);;
}

AutoComoBox::~AutoComoBox()
{

}

void AutoComoBox::setCompleteList(const QStringList &list)
{
    d->m_model->setStringList(list);

    clear();
    addItems(list);
}

QStringList AutoComoBox::getCompleteList() const
{
    return d->m_model->stringList();
}

}
