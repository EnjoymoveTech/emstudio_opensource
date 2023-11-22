#ifndef EMOSCODEBUILDEXPORT_H
#define EMOSCODEBUILDEXPORT_H
#include <QObject>
#include <explorer/buildstep.h>
#include "emoscodebuild_global.h"
#include "configbuild/emosconfigbuild.h"

#define RETURN_IFNOTZERO(result) if(result) return -1;

namespace EmosTools{ struct CfvStruct; }
namespace EmosCodeBuild {
namespace Internal {

}

class EmosCodeBuildExportPrivate;
class EMOSCODEBUILD_EXPORT EmosCodeBuildExport : public QObject
{
    Q_OBJECT
public:
    static EmosCodeBuildExport* instance();
    //只构建xml和构建b文件
    int buildXml(XmlBuildType type, const QString& filePath, const QString& buildPath);
    int buildXml(XmlBuildType type, const EmosTools::CfvStruct& cfv, const QString& buildPath);

    //只生成idl的代码
    int selectBuild(const QString& filePath, const QString& buildPath);
    //生成单个idl的代码和b 101文件
    int build(const QString& filePath, const QString& buildPath);
    //删除生成路径
    int clear(const QString& buildPath);

signals:
    int schBuild(const QString& filePath, const QString& buildPath);
private:
    EmosCodeBuildExport();
    ~EmosCodeBuildExport();

    EmosCodeBuildExportPrivate* d_ptr;
};

}
#endif // EMOSCODEBUILDEXPORT_H
