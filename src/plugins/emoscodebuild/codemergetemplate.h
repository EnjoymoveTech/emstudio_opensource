#ifndef CODEMERGETEMPLATE_H
#define CODEMERGETEMPLATE_H
#include <emostools/api_description_parser.h>
namespace EmosTools{struct IDLStruct;}

namespace EmosCodeBuild {
class TempEngineManager;
class InjaManager;

class CodeMergeTemplate
{
public:
    CodeMergeTemplate(TempEngineManager* engineManager = nullptr);
    ~CodeMergeTemplate();
    void setIdlData(QString filePath);
    void setoldIdlJson(QString jsonPath);

    void templateHBuild(QString path);
    void templateCBuild(QString path);
    QString getIdlJson();
private:
    QString readFile(QString path);
    void writeFile(QString content, QString path);

    QString replaceConstructor(bool);
    QString replaceThread(QString, bool);
    QString replaceMethod(QString, bool);
    QString addNewThread(QString, bool);
    QString addNewMethod(QString, bool);
    QString addNewProperty(QString);
    QString addNewQuetoService(QString);
    QString replaceThread(QString, QString, bool);
    QString replaceMethod(QString, QString, bool);
    QString replacePropertyH(QString);
    QString replaceIncludeCPP(QString, QString);
    InjaManager*      m_injaManager = nullptr;
    EmosTools::IDLStruct*   m_idlData = nullptr;
    QList<EmosTools::tSrvService> m_services;
    //复写前的工程文件
    QStringList   m_oldPropertyList;
    QStringList   m_oldMethodList;
    QStringList   m_oldThreadList;
    QStringList   m_oldQuetoList;
};
}
#endif // CODEMERGETEMPLATE_H
