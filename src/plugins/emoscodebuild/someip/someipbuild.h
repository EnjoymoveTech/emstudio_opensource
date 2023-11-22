#ifndef EMOSSOMEIPBUILD_H
#define EMOSSOMEIPBUILD_H

#include "emosbuildbase.h"
#include <QtXml/QDomDocument>

namespace EmosTools{
struct tStructParsingData;
struct tEnumParsingData;
}

namespace EmosCodeBuild {
namespace Internal {

}
class CodeMergeTemplate;
enum buildtype{
    Client = 0,
    Proxy,
    Service
};

//lingfei.yu 生成代码的详细实现类
class SomeipBuild : public EmosBuildBase
{
public:
    SomeipBuild();
    ~SomeipBuild();
    int buildType(const QString &buildPath);
    int buildApi(const QString &buildPath);
    int buildIdl(const EmosTools::IDLStruct &idlStruct, const QString &buildPath);

private:
    bool proxyBuild();
    bool skelBuild();
    bool TemplateHBuild(QString,int);
    bool TemplateCBuild(QString,int);
    bool ComCBuild(QString,int);
    bool ComHBuild(QString,int);
    bool IntfBuild(QString,int);
    bool CmakeBuild(QString,int);
    bool LittleCmakeBuild(QString,int);
    bool MethodBuild(QString,int);
    bool EventBuild(QString,int);
    bool FieldBuild(QString,int);
    bool PinBuild(QString,int);
    bool StubBuild(QString);
    bool StructBuild(QString);
    bool EnumBuild(QString);
    bool MacroBuild(QString);
    bool TemplateABCBuild(QString);
    bool TemplateHeaderBuild(QString);
    bool TemplateUserBuild(QString);
    void CodeMerge(QString templatepath);
    QString m_buildpath;
    QString m_srcBuildPath;
    QString m_name;
    bool codeMergeFlag = true;
    EmosTools::IDLStruct m_idlData;
    CodeMergeTemplate* m_CodeMergeTemplate = nullptr;
    QList<EmosTools::tStructParsingData> m_structInfo;
    QList<EmosTools::tEnumParsingData> m_enumInfo;
};
}
#endif // EMOSSOMEIPBUILD_H
