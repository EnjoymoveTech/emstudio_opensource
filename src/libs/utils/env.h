#ifndef ENV_H
#define ENV_H

#include <QString>
#include "utils_global.h"

namespace Utils {

class EMSTUDIO_UTILS_EXPORT Env
{
public:
    Env();
    ~Env();

    static void setLibrarySearchPath(const QString& strPath);
    static void removeLibrarySearchPath(const QString& strPath);

private:
    static void lockLibrarySearchPathMutex();
    static void unlockLibrarySearchPathMutex();

};

}
#endif // ENV_H
