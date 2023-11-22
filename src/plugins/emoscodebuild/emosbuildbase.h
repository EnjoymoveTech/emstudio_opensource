#ifndef EMOSBUILDBASE_H
#define EMOSBUILDBASE_H

#include <emostools/cfvparser.h>
#include <emostools/api_description_parser.h>
#include <emostools/type_description_parser.h>

#define RETURN_IFFALSE(result) if(result == false) return false;

namespace EmosCodeBuild {
namespace Internal {

}
class EmosBuildBase
{
public:
    EmosBuildBase();
    virtual ~EmosBuildBase();

    virtual void setCodeRootDir(const QString &dir);

    virtual int buildType(const QString &buildPath) = 0;
    virtual int buildApi(const QString &buildPath) = 0;
    virtual int buildIdl(const EmosTools::IDLStruct &idlStruct, const QString &buildPath) = 0;

protected:
    
    bool codeWriter(const QString &,const QString &);
    bool emptyLogPrint(const QString &,const QString &);

    class TempEngineManager* m_engineManager;

};
}
#endif // EMOSSOMEIPBUILD_H
