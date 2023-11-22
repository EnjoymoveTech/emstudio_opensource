#pragma once
#include <QTreeView>

namespace Idl {
namespace Internal {
class IdlEditorW;


class InspectorTreeView
  : public QTreeView
{

public:
    InspectorTreeView(QObject *parent = nullptr)
    {
        setHeaderHidden(false);
        expandAll();
    }


};


} // namespace Internal
} // namespace ResourceEditor
