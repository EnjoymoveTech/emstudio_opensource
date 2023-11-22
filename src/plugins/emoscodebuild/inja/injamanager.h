#ifndef INJAMANAGER_H
#define INJAMANAGER_H
#include <QScopedPointer>
#include "../tempenginemanager.h"

namespace EmosCodeBuild {

namespace Internal {
class InjaManagerPrivate;
}

class InjaManager : public TempEngineManager
{
public:
    InjaManager();
    virtual ~InjaManager();

    static void showDebugDialog();

    void setTemplateType(TemplateType type) override;
    TemplateType getTemplateType() const override;

    void setCodeRootDir(const QString& dir) override;
    QString getTemplateDir() override;

    void setTypeData() override;
    void setApiData() override;
    void setIdlData(const EmosTools::IDLStruct& idlStruct) override;
    void setIdlOldJson(const QString& JsonPath);
    QString getPropertyJson(QString name);
    QString getThreadJson(QString name);
    QString getMethodJson(QString name);
    QString getQuetoServiceJson(QString name);
    QStringList getoldPropertyList();
    QStringList getoldThreadList();
    QStringList getoldMethodList();
    QStringList getoldQuetoList();
    void setSingleData(const QString& key, const QVariant& value) override;
    QString getdata() override;
    QString getidldata();
    QString renderFile(const QString& filename, const QString& data) override;

    void* getEnv();
private:

    QScopedPointer<Internal::InjaManagerPrivate> d;
};

}
#endif // INJAMANAGER_H
