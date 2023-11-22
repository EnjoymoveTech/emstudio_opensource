#ifndef DESCRIPTIONEDITORFACTORY_H
#define DESCRIPTIONEDITORFACTORY_H

#include <coreplugin/editormanager/ieditorfactory.h>
#include <generaleditor/generaleditor.h>

namespace DescriptionEditor {
namespace Internal {

class DescriptionEditorfactory : public Core::IEditorFactory
{
public:
    DescriptionEditorfactory();
};

}}

#endif // DESCRIPTIONEDITORFACTORY_H
