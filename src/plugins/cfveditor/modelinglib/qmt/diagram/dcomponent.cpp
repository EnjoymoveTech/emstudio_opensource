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

#include "dcomponent.h"

#include "dvisitor.h"
#include "dconstvisitor.h"

namespace qmt {

DComponent::DComponent()
{
}

DComponent::~DComponent()
{
}

void DComponent::setUmlNamespace(const QString &umlNamespace)
{
    m_umlNamespace = umlNamespace;
}

void DComponent::setTemplateParameters(const QList<QString> &templateParameters)
{
    m_templateParameters = templateParameters;
}

void DComponent::setMembers(const QList<MComponentMember> &members)
{
    m_members = members;
}

void DComponent::setVisibleMembers(const QSet<Uid> &visibleMembers)
{
    m_visibleMembers = visibleMembers;
}

void DComponent::setTemplateDisplay(DComponent::TemplateDisplay templateDisplay)
{
    m_templateDisplay = templateDisplay;
}

void DComponent::setShowAllMembers(bool showAllMembers)
{
    m_showAllMembers = showAllMembers;
}

void DComponent::setPlainShape(bool planeShape)
{
    m_isPlainShape = planeShape;
}

void DComponent::accept(DVisitor *visitor)
{
    visitor->visitDComponent(this);
}

void DComponent::accept(DConstVisitor *visitor) const
{
    visitor->visitDComponent(this);
}

void DComponent::setServiceInfo(const QHash<QString, QString> &serviceInfo)
{
    m_serviceInfo = serviceInfo;
}

void DComponent::setPropertyInfo(const QHash<QString, QString> &propertyInfo)
{
    m_propertyInfo = propertyInfo;
}

void DComponent::setPinInfo(const QHash<QString, QString> &pinInfo)
{
    m_pinInfo = pinInfo;
}

void DComponent::addPinInfo(QPair<QString, QString> pair)
{
    if (m_pinInfo.contains(pair.first))
    {
        m_pinInfo.remove(pair.first);
    }
    m_pinInfo.insert(pair.first,pair.second);
}

void DComponent::removePinInfo(QPair<QString, QString> pair)
{
    if (m_pinInfo.contains(pair.first))
    {
        m_pinInfo.remove(pair.first);
    }
}

QPair<QString, QString> DComponent::findPinInfo(QString name)
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
