/****************************************************************************
**
** Copyright (C) 2017 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of Qt Creator.
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 as published by the Free Software
** Foundation with exceptions as appearing in the file LICENSE.GPL3-EXCEPT
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
****************************************************************************/

#include "url.h"
#include "temporaryfile.h"

#include <QHostAddress>
#include <QTcpServer>
#include <QtNetwork>

namespace Utils {

QUrl urlFromLocalHostAndFreePort()
{
    QUrl serverUrl;
    QTcpServer server;
    serverUrl.setScheme(urlTcpScheme());
    if (server.listen(QHostAddress::LocalHost) || server.listen(QHostAddress::LocalHostIPv6)) {
        serverUrl.setHost(server.serverAddress().toString());
        serverUrl.setPort(server.serverPort());
    }
    return serverUrl;
}

QUrl urlFromLocalSocket()
{
    QUrl serverUrl;
    serverUrl.setScheme(urlSocketScheme());
    TemporaryFile file("emstudio-freesocket");
    if (file.open())
        serverUrl.setPath(file.fileName());
    return serverUrl;
}

QString urlSocketScheme()
{
    return QString("socket");
}

QString urlTcpScheme()
{
    return QString("tcp");
}

//判断是否是IP地址
bool isIP(QString ip)
{
    QRegExp RegExp("((2[0-4]\\d|25[0-5]|[01]?\\d\\d?)\\.){3}(2[0-4]\\d|25[0-5]|[01]?\\d\\d?)");
    return RegExp.exactMatch(ip);
}

//获取本机IP
QStringList getLocalIP()
{
    QStringList strIpAddress;

    QList<QNetworkInterface> interfaceList = QNetworkInterface::allInterfaces();
    foreach(QNetworkInterface interfaceItem, interfaceList)
    {
        if(interfaceItem.flags().testFlag(QNetworkInterface::IsUp)
                &&interfaceItem.flags().testFlag(QNetworkInterface::IsRunning)
                &&interfaceItem.flags().testFlag(QNetworkInterface::CanBroadcast)
                &&interfaceItem.flags().testFlag(QNetworkInterface::CanMulticast)
                &&!interfaceItem.flags().testFlag(QNetworkInterface::IsLoopBack)
                &&interfaceItem.hardwareAddress()!="00:50:56:C0:00:01"
                &&interfaceItem.hardwareAddress()!="00:50:56:C0:00:08"
                &&interfaceItem.hardwareAddress()!="0A:00:27:00:00:27"
                &&interfaceItem.name().indexOf("VMware") < 0
                &&interfaceItem.name().indexOf("VirtualBox") < 0
                &&interfaceItem.humanReadableName().indexOf("Default Switch") < 0
                )
        {
            QList<QNetworkAddressEntry> addressEntryList=interfaceItem.addressEntries();
            foreach(QNetworkAddressEntry addressEntryItem, addressEntryList)
            {
                if(addressEntryItem.ip().protocol()==QAbstractSocket::IPv4Protocol)
                {
                      qDebug()<<"------------------------------------------------------------";
                      qDebug()<<"Adapter Name:"<<interfaceItem.name();
                      qDebug()<<"Adapter Name:"<<interfaceItem.index();
                      qDebug()<<"Adapter Name:"<<interfaceItem.humanReadableName();
                      qDebug()<<"Adapter Address:"<<interfaceItem.hardwareAddress();
                      qDebug()<<"IP Address:"<<addressEntryItem.ip().toString();
                      qDebug()<<"IP Mask:"<<addressEntryItem.netmask().toString();
                      qDebug()<<"IP interfaceItem.hardwareAddress():"<<interfaceItem.hardwareAddress();

                    strIpAddress << addressEntryItem.ip().toString();
                }
            }
        }
    }

    return strIpAddress;
}
}
