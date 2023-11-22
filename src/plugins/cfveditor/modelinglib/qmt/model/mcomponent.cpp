/****************************************************************************
**
** Copyright (C) 2016 Jochen Becher
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

#include "mcomponent.h"
#include "emostools/api_description_parser.h"

#include "mvisitor.h"
#include "mconstvisitor.h"

namespace qmt {

MComponent::MComponent()
    : MObject()
{
}

MComponent::MComponent(const MComponent &rhs)
    : MObject(rhs),
      m_umlNamespace(rhs.m_umlNamespace),
      m_templateParameters(rhs.m_templateParameters),
      m_members(rhs.m_members),
      m_serviceInfo(rhs.m_serviceInfo),
      m_propertyInfo(rhs.m_propertyInfo),
      m_pinInfo(rhs.m_pinInfo)
{
}

MComponent::~MComponent()
{
}

MComponent &MComponent::operator=(const MComponent &rhs)
{
    if (this != &rhs) {
        MObject::operator =(rhs);
        m_umlNamespace = rhs.m_umlNamespace;
        m_templateParameters = rhs.m_templateParameters;
        m_members = rhs.m_members;
    }
    return *this;
}

void MComponent::setUmlNamespace(const QString &umlNamespace)
{
    m_umlNamespace = umlNamespace;
}

void MComponent::setTemplateParameters(const QList<QString> &templateParameters)
{
    m_templateParameters = templateParameters;
}

void MComponent::setMembers(const QList<MClassMember> &members)
{
    m_members = members;
}

void MComponent::addMember(const MClassMember &member)
{
    m_members.append(member);
}

void MComponent::insertMember(int beforeIndex, const MClassMember &member)
{
    m_members.insert(beforeIndex, member);
}

void MComponent::removeMember(const Uid &uid)
{
    QMT_CHECK(uid.isValid());
    for (int i = 0; i < m_members.count(); ++i) {
        if (m_members.at(i).uid() == uid) {
            m_members.removeAt(i);
            return;
        }
    }
    QMT_CHECK(false);
}

void MComponent::removeMember(const MClassMember &member)
{
    removeMember(member.uid());
}

void MComponent::accept(MVisitor *visitor)
{
    visitor->visitMComponent(this);
}

void MComponent::accept(MConstVisitor *visitor) const
{
    visitor->visitMComponent(this);
}

void MComponent::setServiceInfo(const QHash<QString, QString> &serviceInfo)
{
    m_serviceInfo = serviceInfo;
}

void MComponent::setPropertyInfo(const QHash<QString, QString> &propertyInfo)
{
    m_propertyInfo = propertyInfo;
}

void MComponent::setPinInfo(QHash <QString,QString> pinInfo)
{
    m_pinInfo = pinInfo;
}

void MComponent::addPinInfo(QPair<QString, QString> pair)
{
    if (m_pinInfo.contains(pair.first))
    {
        m_pinInfo.remove(pair.first);
    }
    m_pinInfo.insert(pair.first,pair.second);
}

void MComponent::removePinInfo(QPair<QString, QString> pair)
{
    if (m_pinInfo.contains(pair.first))
    {
        m_pinInfo.remove(pair.first);
    }
}

QPair<QString, QString> MComponent::findPinInfo(QString name)
{
    QPair<QString,QString> pair = {"",""};
    foreach (const QString& pinName,m_pinInfo.keys())
    {
        if (pinName == name)
        {
            pair.first = pinName;
            pair.second = m_pinInfo.value(pinName);
            break;
        }
    }
    return pair;
}

} // namespace qmt
