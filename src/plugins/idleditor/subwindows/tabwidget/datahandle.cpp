#include "datahandle.h"

Datahandle::Datahandle()
{
}

void Datahandle::removeChild(QDomElement *element, const QString &name)
{
    while(!element->firstChildElement(name).isNull())
    {
        element->removeChild(element->firstChildElement(name));
    }
}
//提炼出所有的service 的 name List
QStringList Datahandle::getServiceList(const QDomDocument *pDoc)
{
    QStringList ans;
    QDomNodeList NodeList = pDoc->documentElement().elementsByTagName("Service");
    for(int i = 0;i < NodeList.size();i++)
    {
        ans.push_back(NodeList.item(i).toElement().attribute("name"));
    }
    return ans;
}
//提炼出所有的QuoteService 的 name List
QStringList Datahandle::Datahandle::getQuoteServiceList(const QDomDocument *pDoc)
{
    QStringList ans;
    QDomNodeList NodeList = pDoc->documentElement().elementsByTagName("QuoteService");
    for(int i = 0;i < NodeList.size();i++)
    {
        ans.push_back(NodeList.item(i).toElement().attribute("name"));
    }
    return ans;
}
