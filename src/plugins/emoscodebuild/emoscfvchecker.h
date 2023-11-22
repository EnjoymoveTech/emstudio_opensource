#ifndef EMOSCFVCHECKER_H
#define EMOSCFVCHECKER_H

#include <QtXml/QDomDocument>
#include <explorer/buildstep.h>
#include <emostools/cfvparser.h>

#define RETURN_IFFALSE(result) if(result == false) return false;

namespace EmosCodeBuild {
namespace Internal {

}
class EmosCfvChecker
{
public:
    EmosCfvChecker();
    ~EmosCfvChecker();
    bool setData(const EmosTools::CfvStruct &cfv);

private:
    bool componentCheck(const EmosTools::CfvStruct &cfv);
    bool connectCheck(const EmosTools::CfvStruct &cfv);
    bool processCheck(const EmosTools::CfvStruct &cfv);
    bool deviceCheck(const EmosTools::CfvStruct &cfv);
    bool nameRepeatCheck(const EmosTools::CfvStruct &cfv);
    bool propertyCheck(const EmosTools::CfvStruct &cfv);
    bool portRepeat(const EmosTools::CfvStruct &cfv);
    bool multiCheck(const EmosTools::CfvStruct &cfv);
};
}

#endif // EMOSCFVCHECKER_H
