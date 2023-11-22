#ifndef DATAHANDLE_H
#define DATAHANDLE_H
#include <QStandardItemModel>
#include <QDomElement>

class Datahandle
{
public:
    Datahandle();

    static void removeChild(QDomElement* element, const QString& name);
    static QStringList getServiceList(const QDomDocument *pDoc);
    static QStringList getQuoteServiceList(const QDomDocument *pDoc);
};

#endif // DATAHANDLE_H
