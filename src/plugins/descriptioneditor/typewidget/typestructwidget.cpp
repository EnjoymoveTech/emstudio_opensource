#include "typestructwidget.h"

#include <utils/emoswidgets/flattable.h>
#include <utils/emoswidgets/messagetips.h>
#include <fileparser/headerparser.h>
#include <coreplugin/minisplitter.h>
#include <explorer/projecttree.h>
#include <explorer/project.h>

#include <QVBoxLayout>

using namespace EmosWidgets;
using namespace EmosTools;

TypeStructWidget::TypeStructWidget(QWidget *parent)
{
    structSplitter = new Core::MiniSplitter(Qt::Vertical);

    includeWidget = new TextEditor::TextEditorWidget(this);
    connect(includeWidget, &TextEditor::TextEditorWidget::textChanged, this, &TypeStructWidget::modify);
    textWidget = new TextEditor::TextEditorWidget(this);
    connect(textWidget, &TextEditor::TextEditorWidget::textChanged, this, &TypeStructWidget::modify);
    QLabel* includeLabel = new QLabel("User Add Area");

    QLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0,0,0,0);
    layout->setSpacing(0);
    layout->addWidget(structSplitter);
    this->setLayout(layout);

    structSplitter->addWidget(includeLabel);
    structSplitter->addWidget(includeWidget);
    structSplitter->addWidget(textWidget);
    structSplitter->setStretchFactor(0, 1);
    structSplitter->setStretchFactor(1, 3);
    structSplitter->setStretchFactor(2, 30);

    m_includeDoc.reset(new TextEditor::TextDocument());
    includeWidget->setTextDocument(m_includeDoc);
    m_doc.reset(new TextEditor::TextDocument());
    textWidget->setTextDocument(m_doc);
}

TypeStructWidget::~TypeStructWidget()
{
}

QDomDocument TypeStructWidget::saveWidget()
{
    QString includeText = includeWidget->toPlainText();
    writeIncludeText(includeText);
    QString headerText = textWidget->toPlainText();
    headerWriter(headerText);
    HeaderParser* headerParser = new HeaderParser();
    headerParser->getFileNameList({m_headerPath});
    if(headerParser->syntaxCheck(headerText))
    {
        headerParser->getInfoFromHeader(m_xml, "Cover");
    }
    delete headerParser;
    includeWidget->restoreState(includeWidget->saveState());
    textWidget->restoreState(textWidget->saveState());
    return m_xml;
}

void TypeStructWidget::getPath(QString headerPath)
{
    m_headerPath = headerPath;
}

void TypeStructWidget::getDoc(QDomDocument &doc)
{
    m_xml = doc;
}

void TypeStructWidget::showWidget()
{
    QFile headerfile(m_headerPath);
    if(headerfile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QString showText = QString::fromLatin1(headerfile.readAll()).remove(QRegExp("^[\\n| ]+\\s*"));
        m_doc->setPlainText(showText);
        headerfile.close();
    }
}

void TypeStructWidget::loadWidget()
{
    TypeParser* typeParser = TypeParser::getInstance();
    typeParser->reloadXml();
    m_includeDoc->setPlainText(typeParser->getIncludes());
    m_typedefList = typeParser->getTypedefs();
    m_structList = typeParser->getStructs();
    m_enumList = typeParser->getEnums();
    m_macroList = typeParser->getMacros();
    QString ContentT = "";
    ContentT += macroWriter() + "\n";
    ContentT += enumWriter() + "\n";
    ContentT += structWriter();
    headerWriter(ContentT);
    showWidget();
}

QString TypeStructWidget::structWriter()
{
    QString ContentT = "";
    QString defaultfilename = "";
    for(int i = 0; i < m_structList.size(); i++)
    {
        tStructData structItem = m_structList.at(i);
        QString filename = structItem.filename;
        if(filename != defaultfilename)
        {
            ContentT += "// file " + filename + ".h\n";
            defaultfilename = filename;
        }
        ContentT += "#pragma pack(" + QString::number(structItem.alignment) + ")\n";
        ContentT += "typedef struct _" + structItem.name + "\n{\n";
        for(int j = 0; j < structItem.structInfo.size(); j++)
        {
            tStructInfo element = structItem.structInfo.at(j);
            ContentT += "    " + element.type + " " + element.name;
            QString arraysize = element.arraysize;
            if(element.arraysize != "0" && element.arraysize != "1")
            {
                if(arraysize.contains(","))  // Multidimensional Array
                {
                    QStringList sizeList = arraysize.split(",");
                    sizeList.removeAll("");
                    for(int sizeIdx = 0; sizeIdx < sizeList.size(); sizeIdx++)
                    {
                        ContentT += "[" + sizeList.at(sizeIdx) + "]";
                    }
                }
                else
                {
                    ContentT += "[" + element.arraysize + "]";
                }
            }
            ContentT += ";";
            if(element.description != "")
            {
                ContentT += "  // " + element.description;
            }
            ContentT += "\n";
        }
        ContentT += "} " + structItem.name + ";\n#pragma pack()\n\n";
    }
    return ContentT;
}

QString TypeStructWidget::enumWriter()
{
    QString ContentT = "";
    QString defaultfilename = "";
    for(int i = 0; i < m_enumList.size(); i++)
    {
        tEnumData enumItem = m_enumList.at(i);
        QString filename = enumItem.filename;
        if(filename != defaultfilename)
        {
            ContentT += "// file " + filename + ".h\n";
            defaultfilename = filename;
        }
        ContentT += "enum " + enumItem.name + "\n{\n";
        for(int j = 0; j < enumItem.enuminfo.size(); j++)
        {
            tEnumInfo element = enumItem.enuminfo.at(j);
            ContentT += "    " + element.name;
            if(element.value != "")
            {
                ContentT += " = " + element.value;
            }
            ContentT += ",";
            if(element.description != "")
            {
                ContentT += "  // " + element.description;
            }
            ContentT += "\n";
        }
        ContentT += "};\n\n";
    }
    return ContentT;
}

QString TypeStructWidget::macroWriter()
{
    QString ContentT = "";
    for(int i = 0; i < m_macroList.size(); i++)
    {
        tMacroData macroItem = m_macroList.at(i);
        ContentT += "#define " + macroItem.name + " " + macroItem.value;
        if(macroItem.description != "")
        {
            ContentT += "  // " + macroItem.description;
        }
        ContentT += "\n";
    }
    ContentT += "\n";
    for(int i = 0; i < m_typedefList.size(); i++)
    {
        tTypedefData typedefItem = m_typedefList.at(i);
        ContentT += "typedef " + typedefItem.value + " " + typedefItem.name + ";\n";
    }
    return ContentT;
}

void TypeStructWidget::headerWriter(QString &ContentT)
{
    if(QFile::exists(m_headerPath))
    {
        QFile::remove(m_headerPath);
    }
    QFile header(m_headerPath);
    if(header.open(QIODevice::ReadWrite))
    {
        QByteArray strBytes = ContentT.toUtf8();
        header.write(strBytes, strBytes.length());
        header.close();
    }
}

void TypeStructWidget::writeIncludeText(QString &includeText)
{
    QString filePath = ProjectExplorer::ProjectTree::currentProject()->projectDirectory().toString() + "/1_interface_design/type.description";
    QDomElement includeNode = m_xml.firstChildElement("emos:ddl").firstChildElement("includes");
    if(includeNode.isNull())
    {
        includeNode = m_xml.createElement("includes");
        m_xml.firstChildElement("emos:ddl").appendChild(includeNode);
    }
    includeNode.setAttribute("text", includeText);
    if(QFile::exists(filePath))
    {
        QFile::remove(filePath);
    }
    QFile xml(filePath);
    if(!xml.open(QIODevice::ReadWrite))
    {
        QString errLog = "FileError: Can't open file " + filePath;
        return;
    }
    QTextStream stream(&xml);
    m_xml.save(stream, 4, QDomNode::EncodingFromTextStream);
    xml.close();
}
