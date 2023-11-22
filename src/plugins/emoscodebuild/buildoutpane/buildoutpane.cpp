#include "buildoutpane.h"
#include "coreplugin/icontext.h"
#include "coreplugin/coreconstants.h"
#include "coreplugin/find/basetextfind.h"

#include <aggregation/aggregate.h>
#include <coreplugin/icore.h>
#include <utils/utilsicons.h>

#include <QFont>
#include <QToolButton>

namespace EmosCodeBuild {
namespace Internal {

const char zoomSettingsKey[] = "Core/BuildoutOutput/Zoom";

BuildoutPane::BuildoutPane(QObject *parent)
    : MessageOutputWindow()
{

}

QString BuildoutPane::displayName() const
{
    return tr("Buildout Info");
}

int BuildoutPane::priorityInStatusBar() const
{
    return 40;
}

} // namespace Internal
} // namespace Core
