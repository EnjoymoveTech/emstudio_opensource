#include "injacallback.h"
#include "injamanager.h"
#include <QString>
#include <inja/inja.hpp>
using Json = inja::json;

namespace EmosCodeBuild {

namespace Internal {
}
injaCallBack::injaCallBack(InjaManager* mgr)
{
    inja::Environment* env = static_cast<inja::Environment*>(mgr->getEnv());
    env->add_callback("toHex", 2, [](inja::Arguments& args)
    {
        int number =args.at(0)->get<int>();
        QString t_num = "0000" + QString::number(number, 16);
        int x = args.at(1)->get<int>();
        if(x == 2)
        {
            t_num = t_num.right(2);
        }
        else
        {
            t_num = t_num.right(4);
        }
        return t_num.toStdString();
    });

    env->add_callback("CanRecvNoti", 1, [](inja::Arguments& args)
    {
        Json fieldList = *args.at(0);
        for(int i = 0; i < fieldList.size(); i++)
        {
            int type = fieldList[i]["type"].get<int>();
            if(type % 2)
            {
                return true;
            }
        }
        return false;
    });

    env->add_callback("inclusion", 1, [](inja::Arguments& args)
    {
        QString include = QString::fromStdString(args.at(0)->get<std::string>());
        QStringList includeList = include.split("/");
        includeList.removeAll("");
        QString res = "";
        for(int i = 0; i < includeList.size(); i++)
        {
            res += "#include \"" + includeList.at(i) + ".h\"\n";
        }
        return res.toStdString();
    });

    env->add_callback("commaToBracket", 1, [](inja::Arguments& args)
    {
        QString include = QString::fromStdString(args.at(0)->get<std::string>());
        QString res = "";
        if(include == "0" || include == "1" || include == "")
        {
            ;
        }
        else
        {
            res = "[" + include + "]";
            res.replace(",", "][");
        }
        return res.toStdString();
    });

    //计算类型*的数量
    env->add_callback("pointerCount", 1, [](inja::Arguments& args)
    {
        QString type = QString::fromStdString(args.at(0)->get<std::string>());
        return type.count("*");
    });

    //计算类型*的数量
    env->add_callback("calInParaSize", 1, [](inja::Arguments& args)
    {
        int64_t size = 0;
        Json paraList = *args.at(0);
        for(int i = 0; i < paraList.size(); i++)
        {
            QString direction = QString::fromStdString(paraList[i]["direction"].get<std::string>());
            if(direction != "out")
                size += paraList[i]["size"].get<int>();
        }
        return size;
    });

    //移除type里 ***
    env->add_callback("removePointer", 1, [](inja::Arguments& args)
    {
        QString type = QString::fromStdString(args.at(0)->get<std::string>());
        return type.replace("*","").toStdString();
    });

    //计算类型&的数量
    env->add_callback("refCount", 1, [](inja::Arguments& args)
    {
        QString type = QString::fromStdString(args.at(0)->get<std::string>());
        return type.count("&");
    });

    //移除&
    env->add_callback("removeRef", 1, [](inja::Arguments& args)
    {
        QString type = QString::fromStdString(args.at(0)->get<std::string>());
        return type.replace("&","").toStdString();
    });
}
}
