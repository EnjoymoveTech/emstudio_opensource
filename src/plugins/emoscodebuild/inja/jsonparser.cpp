#include "jsonparser.h"
#include <QStringList>

namespace EmosCodeBuild {
JsonParser::JsonParser()
{

}

JsonParser::~JsonParser()
{

}
//传递进来的Json只有Idl的数据
void JsonParser::setJsonData(Json oldData, Json newData)
{
    m_oldData = oldData;
    m_newData = newData;
}

QString JsonParser::getJsonData()
{
    return QString::fromStdString(m_newData.dump());
}

//true->new  false->old
QStringList JsonParser::getDataList(DATALISTTYPE datakind, bool new_or_old)
{
    QStringList ans;
    Json data;
    std::string nodename;
    if(new_or_old){
        data = m_newData;
    } else {
        data = m_oldData;
    }
    //lingfei.yu 查找节点对应的Json
    if(datakind == THREADLIST){
        nodename = "threadList";
    } else if(datakind == PROPERTYLIST){
        nodename = "propertyList";
    } else if(datakind == QUETOLIST){
        nodename = "QuoteServiceList";
    }else {
        nodename = "methodList";
        nlohmann::json object = data["serviceList"];
        for(auto &iter : object.items()){
            nlohmann::json servicenode = iter.value();
            ans += getItemSecondName(servicenode["methodList"]);
        }
        return ans;
    }
    if(data.contains(nodename)){
        return getItemSecondName(data[nodename]);
    }
    return ans;
}

QString JsonParser::getSingleJsonData(DATALISTTYPE datakind, QString name)
{
    Json data;
    std::string dataname = name.toStdString();
    if(datakind == THREADLIST){
        if(!m_newData.contains("threadList")){
            return "";
        }
        for(auto &iter : (m_newData["threadList"].items())){
            if(iter.value()["name"] == dataname){
                data["name"] = m_newData["name"];
                data["thread"] = iter.value();
                break;
            }
        }
    } else if(datakind == PROPERTYLIST){
        if(!m_newData.contains("propertyList")){
            return "";
        }
        for(auto &iter : m_newData["propertyList"].items()){
            if(iter.value()["name"] == dataname){
                data["property"] = iter.value();
                break;
            }
        }
    } else if(datakind == QUETOLIST){
        if(!m_newData.contains("QuoteServiceList")){
            return "";
        }
        for(auto &iter : m_newData["QuoteServiceList"].items()){
            if(iter.value()["name"] == dataname){
                data["quoteservice"] = iter.value();
                break;
            }
        }
    } else {
        if(!m_newData.contains("serviceList")){
            return "";
        }
        for(auto &iter : m_newData["serviceList"].items()){
            for(auto &iter_method : iter.value()["methodList"].items()){
                if(iter_method.value()["name"] == dataname){
                    data["name"] = m_newData["name"];
                    data["method"] = iter_method.value();
                    break;
                }
            }
        }
    }
    return QString::fromStdString(data.dump());
}

QStringList JsonParser::getItemSecondName(Json obj)
{
    QStringList ans;
    for(auto &iter : obj.items()){
        ans.push_back(QString::fromStdString(iter.value()["name"]));
    }
    return ans;
}


}
