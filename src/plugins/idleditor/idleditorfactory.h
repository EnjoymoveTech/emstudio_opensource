#ifndef IDLEDITORFACTORY
#define IDLEDITORFACTORY
#include <coreplugin/editormanager/ieditorfactory.h>

namespace Idl {
namespace Internal {

class IdlEditorFactory : public Core::IEditorFactory
{
public:
    explicit IdlEditorFactory();
};

} // namespace Internal
} // namespace Idl

#endif
