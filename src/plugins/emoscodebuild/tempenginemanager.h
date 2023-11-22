#ifndef TEMPENGINEMANAGER_H
#define TEMPENGINEMANAGER_H

#include <coreplugin/icore.h>
#include <emostools/idlparser.h>

namespace EmosCodeBuild {

enum TemplateType
{
    Template_Somip,
};

namespace Internal {
}

class TempEngineManager
{
public:
    TempEngineManager();
    virtual ~TempEngineManager();

    virtual void setTemplateType(TemplateType type);
    virtual TemplateType getTemplateType() const;

    virtual void setCodeRootDir(const QString& dir) = 0;
    virtual QString getTemplateDir() = 0;

    virtual void setTypeData() = 0;
    virtual void setApiData() = 0;
    virtual void setIdlData(const EmosTools::IDLStruct& idlStruct) = 0;
    virtual void setSingleData(const QString& key, const QVariant& value) = 0;
    virtual QString getdata() = 0;
    virtual QString renderFile(const QString& filename, const QString& data) = 0;
protected:
    TemplateType m_type;

protected:
    const QString m_staticDir = Core::ICore::resourcePath() + QLatin1String("/emoscodebuild/");
};

}
#endif // TEMPENGINEMANAGER_H
