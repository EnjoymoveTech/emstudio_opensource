#ifndef TYPESTRUCTWIDGET_H
#define TYPESTRUCTWIDGET_H

#include "typewidgetbase.h"

#include <generaleditor/textdocument.h>
#include <generaleditor/generaleditor.h>
#include <QDomDocument>
#include <emostools/type_description_parser.h>

namespace Core {class MiniSplitter;}
class TypeStructWidget: public QWidget
{
    Q_OBJECT
public:
    explicit TypeStructWidget(QWidget *parent = nullptr);
    ~TypeStructWidget();
    QDomDocument saveWidget();
    void getPath(QString headerPath);
    void getDoc(QDomDocument &doc);
    void showWidget();
    void loadWidget();

signals:
    void modify();
public slots:
    //void slotAddRow();
    //void slotchange();

private:
    QString structWriter();
    QString enumWriter();
    QString macroWriter();
    void headerWriter(QString &);
    void writeIncludeText(QString&);

    Core::MiniSplitter *structSplitter = nullptr;

    TextEditor::TextEditorWidget* textWidget = nullptr;
    TextEditor::TextEditorWidget* includeWidget = nullptr;
    QSharedPointer<TextEditor::TextDocument> m_doc = nullptr;
    QSharedPointer<TextEditor::TextDocument> m_includeDoc = nullptr;
    QDomDocument m_xml;
    QString m_headerPath;
    QList<EmosTools::tTypedefData> m_typedefList;
    QList<EmosTools::tStructData> m_structList;
    QList<EmosTools::tEnumData> m_enumList;
    QList<EmosTools::tMacroData> m_macroList;
};

#endif // TYPESTRUCTWIDGET_H
