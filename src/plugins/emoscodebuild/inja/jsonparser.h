#ifndef JSONPARSER_H
#define JSONPARSER_H
#include <inja/inja.hpp>
#include <QString>

namespace EmosCodeBuild {
using Json = inja::json;
enum DATALISTTYPE{
    QUETOLIST,
    PROPERTYLIST,
    THREADLIST,
    METHODLIST
};


//lingfei.yu 解析Json数据便于对比和拿取
class JsonParser
{
public:
    JsonParser();
    ~JsonParser();
    void setJsonData(Json oldData, Json newData);
    QString getJsonData();
    QStringList getDataList(DATALISTTYPE datakind, bool new_or_old);
    QString getSingleJsonData(DATALISTTYPE dataname, QString name);
private:
    QStringList getItemSecondName(Json obj);
    Json m_oldData;
    Json m_newData;

};
}
#endif // JSONPARSER_H
