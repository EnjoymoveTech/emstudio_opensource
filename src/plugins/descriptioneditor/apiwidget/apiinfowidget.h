#ifndef APIINFOWIDGET_H
#define APIINFOWIDGET_H

#include <utils/treemodel.h>
#include <QDomDocument>
#include <QApplication>
#include <QPalette>

namespace EmosWidgets {class FlatTable;}
class ApiTypeNode : public Utils::TreeItem
{
public:
    ApiTypeNode(const QString Class, const QString Object)
        : m_Class(Class),m_Object(Object)
    {}

    QVariant data(int colum, int role) const override
    {
        if (role == Qt::DisplayRole)
        {
            if(0 == colum)
            {
                return m_Object;
            }
            else
            {
                return m_Class;
            }
        }
        return QVariant();
    }

    QString m_Class;
    QString m_Object;
};

class GroupNode : public Utils::StaticTreeItem
{
public:
    GroupNode(const QString &text)
        : Utils::StaticTreeItem(text)
    {}

    Qt::ItemFlags flags(int) const { return {}; }
    QVariant data(int column, int role) const
    {
        if (role == Qt::ForegroundRole) {
            return QApplication::palette().color(QPalette::ColorGroup::Normal,
                                                 QPalette::ColorRole::Text);
        }
        return Utils::StaticTreeItem::data(column, role);
    }
};

class InspectorModel : public Utils::TreeModel<Utils::TreeItem,GroupNode, ApiTypeNode>
{
    Q_OBJECT

public:
    InspectorModel(QHash<QString, EmosWidgets::FlatTable*> tableHash, QObject *parent);
    bool LoadWidget(QString serviceName);
private:
    void LoadService();
    Utils::TreeItem *m_ServiceRoot = nullptr;
    QHash<QString, EmosWidgets::FlatTable*> m_tableHash;
};

#endif
