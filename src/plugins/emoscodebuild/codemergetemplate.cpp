#include <QString>
#include <qbitarray.h>
#include <emostools/cfvparser.h>
#include "injamanager.h"
#include "codemergetemplate.h"
using namespace EmosTools;

namespace EmosCodeBuild {
CodeMergeTemplate::CodeMergeTemplate(TempEngineManager *engineManager)
{
    m_idlData = new EmosTools::IDLStruct;
    m_injaManager = (InjaManager*)(engineManager);
}

CodeMergeTemplate::~CodeMergeTemplate()
{
    delete m_idlData;
}

void CodeMergeTemplate::setIdlData(QString filePath)
{
    m_services.clear();
    *m_idlData = IdlParser::getIdlStructByFile(filePath);
    QList<EmosTools::tSrvService> services = EmosTools::ApiParser::getInstance()->getServices();
    for(auto i = m_idlData->service.begin() ; i != m_idlData->service.end(); i++){
        for(int j = 0; j < services.size() ;j++){
            if(i.key() == services[j].name){
                m_services.push_back(services[j]);
            }
        }
    }
}

void CodeMergeTemplate::setoldIdlJson(QString jsonPath)
{
    m_injaManager->setIdlOldJson(jsonPath);
    m_oldPropertyList = m_injaManager->getoldPropertyList();
    m_oldThreadList   = m_injaManager->getoldThreadList();
    m_oldMethodList   = m_injaManager->getoldMethodList();
    m_oldQuetoList    = m_injaManager->getoldQuetoList();
}

//path为template.h的路径
void CodeMergeTemplate::templateHBuild(QString path)
{
    QString allcontent = readFile(path);
    if(allcontent.isEmpty()){
        return;
    }
    QStringList contentList = allcontent.split("\n");
    QString name = m_idlData->name;
    for(int i = 0 ;i < contentList.size();){
        QString content = contentList[i];
        //第一个检查点 构造函数
        if(content.contains("c" + name) && content.contains("(const tChar*")){
            contentList[i] = replaceConstructor(true);
        }
        //第二个检查点 thread函数
        else if(content.contains("tResult On") || content.contains("tResult TimeTrigger_")
                || content.contains("tResult ThreadTrigger_")){
            bool flag = false;
            QString oldname;
            int begin_pos = 0;
            int end_pos = content.indexOf("(");
            if(content.contains("Trigger_")){
                begin_pos = content.indexOf("_") + 1;
            } else {
                begin_pos = content.indexOf("n") + 1;
            }
            oldname = content.mid(begin_pos,end_pos - begin_pos);
            for(int i_thread = 0; i_thread < m_oldThreadList.size(); i_thread++){
                if(oldname == (m_oldThreadList[i_thread])){
                    flag = true;
                    break;
                }
            }
            if(flag){
                QString check = replaceThread(content, oldname, true);
                //qDebug() << "thread replace is "<< endl << check << endl;
                if(!check.isEmpty()){
                    contentList[i] = check;
                }
            }
        }
        //检查新增新的thread 和 method 函数 并加入内容
        else if(content.contains("tResult Stop();") || content.contains("tVoid Stop();")){
            for(int i_method = 0; i_method < m_services.size(); i_method++){
                for(int j = 0; j < m_services[i_method].functions.size(); j++){
                    int flag = true;
                    for(int z = 0; z < m_oldMethodList.size(); z++){
                        if(m_services[i_method].functions[j].name == m_oldMethodList[z]){
                            flag = false;
                            break;
                        }
                    }
                    if(flag){
                        //qDebug() << "method insert is "<< endl << addNewMethod(m_services[i_method].functions[j].name,true) << endl;
                        contentList.insert(i+1,addNewMethod(m_services[i_method].functions[j].name,true));
                    }
                }
            }
            for(int i_thread = 0; i_thread < m_idlData->thread.size(); i_thread++){
                int flag = true;
                for(int j = 0; j < m_oldThreadList.size(); j++){
                    if(m_idlData->thread[i_thread].name == m_oldThreadList[j]){
                        flag = false;
                        break;
                    }
                }
                if(flag){
                    //qDebug() << "thread insert is "<< endl << addNewThread(m_idlData->thread[i_thread].name,true) << endl;
                    contentList.insert(i+1, addNewThread(m_idlData->thread[i_thread].name,true));
                }
            }
        }

        //第三个检查点 method函数
        else if(content.contains("(") && content.contains(");")){
            bool flag = false;
            QString oldname = content.mid(0,content.indexOf("("));
            QStringList spaceList = oldname.split(" ");
            oldname = spaceList[spaceList.size() -1];
            for(int i_method = 0; i_method < m_oldMethodList.size(); i_method++){
                if(oldname == m_oldMethodList[i_method]){
                    flag = true;
                    break;
                }
            }
            if(flag){
                QString check = replaceMethod(content, oldname, true);
                //qDebug() << "method replace is "<< endl << check << endl << "line count is " << i << endl;
                if(!check.isEmpty()){
                    contentList[i] = check;
                }
            }
        }
        //新增property至内容中
        else if(content == "private:"){
            for(auto i_property = m_idlData->property.begin() ; i_property != m_idlData->property.end(); i_property++){
                int flag = true;
                for(int j = 0; j < m_oldPropertyList.size(); j++){
                    if(i_property.key() == m_oldPropertyList[j]){
                        flag = false;
                        break;
                    }
                }
                if(flag){
                    //qDebug() << "prperty insert is "<< endl << addNewProperty(i_property.key()) << endl;
                    contentList.insert(i+1, addNewProperty(i_property.key()));
                }
            }
        }
        //第四个检查点 替换property成员变量
        else if(content.contains("const ") && content.contains("m_")){
            bool flag = false;
            //property一定是两个空格
            QString oldname = content.mid(content.indexOf("_") + 1, content.size() - 2 - content.indexOf("_"));
            for(int i_property = 0; i_property < m_oldPropertyList.size(); i_property++){
                if(oldname == m_oldPropertyList[i_property]){
                    flag = true;
                    break;
                }
            }
            if(flag){
                QString check = replacePropertyH(oldname);
                //qDebug() << "prperty replace is " << check << endl;
                if(check.isEmpty())
                {
                    contentList.removeAt(i);
                    continue;
                }else {
                    contentList[i] = check;
                }
            }
        }
        i++;
    }

    allcontent = "";
    allcontent = contentList.join("\n");
    writeFile(allcontent,path);
}

//path为template.cpp的路径 cpp只用在末尾增加新的函数即可
void CodeMergeTemplate::templateCBuild(QString path)
{
    QString allcontent = readFile(path);
    if(allcontent.isEmpty()){
        return;
    }
    QStringList contentList = allcontent.split("\n");
    QString name = m_idlData->name;
    for(int i = 0 ;i < contentList.size();){
        QString content = contentList[i];
        //首先新增头文件在#include "struct_common.h"后面
        if(content.contains("#include \"struct_common.h\"")){
            for(auto i_quetoservice = m_idlData->quoteService.begin() ; i_quetoservice != m_idlData->quoteService.end(); i_quetoservice++ ){
                int flag = true;
                for(int j = 0; j < m_oldQuetoList.size(); j++){
                    if(i_quetoservice.key() == m_oldQuetoList[j]){
                        flag = false;
                        break;
                    }
                }
                if(flag){
                    //qDebug() << "quetoservice insert is "<< endl << addNewQuetoService(i_quetoservice.key()) << endl;
                    contentList.insert(i + 1, addNewQuetoService(i_quetoservice.key()));
                }
            }
        }
        //第一个检查点头文件
        else if(content.contains("#include \"") && content.contains("_intf.h")){
            QString oldname = content.mid(content.indexOf("\"") + 1,content.indexOf("_intf") - content.indexOf("\"") - 1);
            bool flag = false;
            for(int i_quetoservice = 0 ;i_quetoservice < m_oldQuetoList.size(); i_quetoservice++){
                if(oldname == m_oldQuetoList[i_quetoservice].toLower()){
                    oldname = m_oldQuetoList[i_quetoservice];
                    flag = true;
                    break;
                }
            }
            if(flag){
                //传入进去的是大写的名字
                QString check = replaceIncludeCPP(content, oldname);
                //qDebug() << "include replace is " << check << endl;
                if(check.isEmpty()){
                    contentList.removeAt(i);
                    continue;
                } else {
                    contentList[i] = check;
                }
            }
        }//第二个检查点 构造函数
        else if(content.contains("c" + name) && content.contains("(const tChar*")){
            contentList[i] = replaceConstructor(false);
        }//第三个检查点 thread
        else if(content.contains("tResult c" + name + "::On") || content.contains("tResult c" + name + "::TimeTrigger_")
                || content.contains("tResult c" + name + "::ThreadTrigger_")){
            bool flag = false;
            QString oldname;
            int begin_pos = 0;
            int end_pos = content.indexOf("(");
            if(content.contains("Trigger_")){
                begin_pos = content.indexOf("_") + 1;
            } else {
                begin_pos = content.indexOf("::On") + 4;
            }
            oldname = content.mid(begin_pos,end_pos - begin_pos);
            for(int i_thread = 0; i_thread < m_oldThreadList.size(); i_thread++){
                if(oldname == (m_oldThreadList[i_thread])){
                    flag = true;
                    break;
                }
            }
            if(flag){
                QString check = replaceThread(content, oldname, false);
                //qDebug() << "thread replace is " << check << endl;
                if(!check.isEmpty()){
                    contentList[i] = check;
                }
            }
        }//第四个检查点 method
        else if(content.contains(" c" + name + "::") && content.contains("(") && content.contains(")")){
            bool flag = false;
            QString oldname = content.mid(content.indexOf("::") + 2,content.indexOf("(") - (content.indexOf("::") + 2));
            for(int i_method = 0; i_method < m_oldMethodList.size(); i_method++){
                if(oldname == m_oldMethodList[i_method]){
                    flag = true;
                    break;
                }
            }
            if(flag){
                QString check = replaceMethod(content, oldname, false);
                //qDebug() << "Method replace is " << check << endl;
                if(!check.isEmpty()){
                    contentList[i] = check;
                }
            }
        }
        i++;
    }

    for(int i_thread = 0; i_thread < m_idlData->thread.size(); i_thread++){
        int flag = true;
        for(int j = 0; j < m_oldThreadList.size(); j++){
            if(m_idlData->thread[i_thread].name == m_oldThreadList[j]){
                flag = false;
                break;
            }
        }
        if(flag){
            //qDebug() << "thread insert is " << addNewThread(m_idlData->thread[i_thread].name,false) << endl;
            contentList.append(addNewThread(m_idlData->thread[i_thread].name,false));
        }
    }

    for(int i_method = 0; i_method < m_services.size(); i_method++){
        for(int j = 0; j < m_services[i_method].functions.size(); j++){
            int flag = true;
            for(int z = 0; z < m_oldMethodList.size(); z++){
                if(m_services[i_method].functions[j].name == m_oldMethodList[z]){
                    flag = false;
                    break;
                }
            }
            if(flag){
                //qDebug() << "Method insert is " << addNewMethod(m_services[i_method].functions[j].name,false) << endl;
                contentList.append(addNewMethod(m_services[i_method].functions[j].name,false));
            }
        }
    }

    allcontent = "";
    allcontent = contentList.join("\n");
    writeFile(allcontent,path);
}

QString CodeMergeTemplate::getIdlJson()
{
    return m_injaManager->getidldata();
}

//读文件
QString CodeMergeTemplate::readFile(QString path)
{
    QString content;
    QFile file(path);
    if(!file.open(QIODevice::ReadWrite | QIODevice::Text)){
        return content;
    }
    QTextStream in(&file);
    content = in.readAll();
    file.close();
    return content;
}

//写文件
void CodeMergeTemplate::writeFile(QString content,QString path)
{
    QFile file(path);
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text)){
        return;
    }
    QByteArray  strBytes = content.toUtf8();
    file.write(strBytes,strBytes.length());
    file.close();
}

//构造函数进入的条件一定OK, true->.h false->.cpp
QString CodeMergeTemplate::replaceConstructor(bool type)
{
    if(type){
        return m_injaManager->renderFile("service/template/template_constructor.h", m_injaManager->getdata());
    } else {
        return m_injaManager->renderFile("service/template/template_constructor.cpp", m_injaManager->getdata());
    }

}

QString CodeMergeTemplate::replaceThread(QString name, bool type)
{
    if(type){
        return m_injaManager->renderFile("service/template/template_thread.h",
                                         m_injaManager->getThreadJson(name));
    } else {
        return m_injaManager->renderFile("service/template/template_thread.cpp",
                                         m_injaManager->getThreadJson(name));
    }
    return "";
}

QString CodeMergeTemplate::replaceMethod(QString name, bool type)
{
    if(type){
        return m_injaManager->renderFile("service/template/template_method.h",
                                         m_injaManager->getMethodJson(name));
    } else {
        return m_injaManager->renderFile("service/template/template_method.cpp",
                                         m_injaManager->getMethodJson(name));
    }
}

//根据传入进来的Thread 的名字增加新的内容, true->.h false->.cpp
QString CodeMergeTemplate::addNewThread(QString name, bool type)
{
    if(type){
        return m_injaManager->renderFile("service/template/template_thread.h",
                                         m_injaManager->getThreadJson(name));
    } else {
        return m_injaManager->renderFile("service/template/template_new_thread.cpp",
                                         m_injaManager->getThreadJson(name));
    }
    return "";
}

QString CodeMergeTemplate::addNewMethod(QString name, bool type)
{
    if(type){
        return m_injaManager->renderFile("service/template/template_method.h",
                                         m_injaManager->getMethodJson(name));
    } else {
        return m_injaManager->renderFile("service/template/template_new_method.cpp",
                                         m_injaManager->getMethodJson(name));
    }
}

QString CodeMergeTemplate::addNewProperty(QString name)
{
    return m_injaManager->renderFile("service/template/template_property.h",
                                     m_injaManager->getPropertyJson(name));
}

QString CodeMergeTemplate::addNewQuetoService(QString name)
{
    return m_injaManager->renderFile("service/template/template_include.cpp",
                                     m_injaManager->getQuetoServiceJson(name));
}

//进来后需要判断thread的名字是否本次复写还存在, 不存在则不变, true->.h false->.cpp
QString CodeMergeTemplate::replaceThread(QString content,QString oldname, bool type)
{
    for(int i = 0; i < m_idlData->thread.size(); i++){
        if(m_idlData->thread[i].name == oldname){
            if(type){
                return m_injaManager->renderFile("service/template/template_thread.h",
                                                 m_injaManager->getThreadJson(oldname));
            } else {
                return m_injaManager->renderFile("service/template/template_thread.cpp",
                                                 m_injaManager->getThreadJson(oldname));
            }
            return "";
        }
    }
    return "";
}

//同理判断Method是否存在, true->.h false->.cpp
QString CodeMergeTemplate::replaceMethod(QString content, QString oldname, bool type)
{
    for(int i = 0; i < m_services.size(); i++){
        for(int j = 0; j < m_services[i].functions.size(); j++){
            if(m_services[i].functions[j].name == oldname){
                if(type){
                    return m_injaManager->renderFile("service/template/template_method.h",
                                                     m_injaManager->getMethodJson(oldname));
                } else {
                    return m_injaManager->renderFile("service/template/template_method.cpp",
                                                     m_injaManager->getMethodJson(oldname));
                }
            }
        }
    }
    return "";
}

//判断property是否存在
QString CodeMergeTemplate::replacePropertyH(QString oldname)
{
    for(auto i = m_idlData->property.begin() ; i != m_idlData->property.end(); i++){
        if(i.key() == oldname){
            return addNewProperty(oldname);
        }
    }
    return "";
}

//判断quetoService是否存在
QString CodeMergeTemplate::replaceIncludeCPP(QString content, QString oldname)
{
    for(auto i = m_idlData->quoteService.begin() ; i != m_idlData->quoteService.end(); i++){
        if(i.key() == oldname){
            return addNewQuetoService(i.key());
        }
    }
    return "";
}
}
