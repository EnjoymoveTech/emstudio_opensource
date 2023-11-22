#include "descriptioneditorwidget.h"
#include <QHBoxLayout>

#include <generaleditor/textdocument.h>
#include <generaleditor/generaleditor.h>
#include <explorer/project.h>
#include <emostools/api_description_parser.h>

#include "descriptioneditor.h"
#include <QPushButton>
using namespace Core;

namespace DescriptionEditor {
namespace Internal {

class DescriptionEditorWidgetPrivate
{
public:
    QStackedWidget *widgetStack = nullptr;
    TextEditor::TextEditorWidget* textWidget = nullptr;
    QSharedPointer<TextEditor::TextDocument> doc = nullptr;
    DescriptionEditor* editor = nullptr;
};

DescriptionEditorWidget::DescriptionEditorWidget(DescriptionEditor* editor, QWidget *parent):
    Utils::FancyMainWindow(parent),
    d(new DescriptionEditorWidgetPrivate)
{
    d->editor = editor;

    d->doc.reset(new TextEditor::TextDocument());
    d->textWidget = new TextEditor::TextEditorWidget(this);
    d->textWidget->setTextDocument(d->doc);

    d->widgetStack = new QStackedWidget(this);
    mainWidget = new QWidget(this);

    d->widgetStack->addWidget(mainWidget);
    d->widgetStack->addWidget(d->textWidget);
    d->widgetStack->setCurrentWidget(mainWidget);

    setCentralWidget(d->widgetStack);
    connect(EditorManager::instance(), &EditorManager::autoCreateNewFile,
            this, &DescriptionEditorWidget::slotCreateNewFile);
}

DescriptionEditorWidget::~DescriptionEditorWidget()
{
    delete d;
}

DescriptionEditor *DescriptionEditorWidget::editor()
{
    return d->editor;
}

void DescriptionEditorWidget::slotText(bool checked)
{
    if(checked)
    {
        d->widgetStack->setCurrentWidget(d->textWidget);

        QFile xmlfile(d->editor->document()->filePath().toString());
        if(xmlfile.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            d->doc->setPlainText(QString::fromLatin1(xmlfile.readAll()));
            xmlfile.close();
        }
    }
    else
        d->widgetStack->setCurrentWidget(mainWidget);
}

void DescriptionEditorWidget::slotModify()
{
    qobject_cast<TextEditor::TextDocument*>(d->editor->document())->document()->setModified(true);
}
}}
