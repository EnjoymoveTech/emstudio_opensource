#include "tempenginemanager.h"

#include <QDomDocument>

namespace EmosCodeBuild {
namespace Internal {}

TempEngineManager::TempEngineManager()
{
    m_type = Template_Somip;
}

TempEngineManager::~TempEngineManager()
{
}

void TempEngineManager::setTemplateType(TemplateType type)
{
    m_type = type;
}

TemplateType TempEngineManager::getTemplateType() const
{
    return m_type;
}
}
