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

#pragma once

#include "dobject.h"
#include "qmt/model/mcomponentmember.h"
namespace qmt {

class QMT_EXPORT DComponent : public DObject
{
public:
    enum TemplateDisplay {
        TemplateSmart,
        TemplateBox,
        TemplateName
    };

    DComponent();
    ~DComponent() override;

    bool isPlainShape() const { return m_isPlainShape; }
    void setPlainShape(bool planeShape);
    QString umlNamespace() const { return m_umlNamespace; }
    void setUmlNamespace(const QString &umlNamespace);
    QList<QString> templateParameters() const { return m_templateParameters; }
    void setTemplateParameters(const QList<QString> &templateParameters);
    QList<MComponentMember> members() const { return m_members; }
    void setMembers(const QList<MComponentMember> &members);
    QSet<Uid> visibleMembers() const { return m_visibleMembers; }
    void setVisibleMembers(const QSet<Uid> &visibleMembers);
    TemplateDisplay templateDisplay() const { return m_templateDisplay; }
    void setTemplateDisplay(TemplateDisplay templateDisplay);
    bool showAllMembers() const { return m_showAllMembers; }
    void setShowAllMembers(bool showAllMembers);

    void accept(DVisitor *visitor) override;
    void accept(DConstVisitor *visitor) const override;

    //haojie.fang add service
    void setServiceInfo(const QHash <QString,QString>& serviceInfo);
    QHash <QString,QString> serviceInfo() const{ return m_serviceInfo; }

    //haojie.fang add property
    void setPropertyInfo(const QHash <QString,QString>& propertyInfo);
    QHash <QString,QString> propertyInfo() const{ return m_propertyInfo; }

    //zhecheng.zhou add pin
    void setPinInfo(const QHash<QString, QString> &pinInfo);
    void addPinInfo(QPair<QString,QString> pair);
    void removePinInfo(QPair<QString,QString> pair);
    QPair<QString, QString> findPinInfo(QString name);
    QHash <QString,QString> pinInfo() const{ return m_pinInfo; }
private:
    bool m_isPlainShape = false;
    QString m_umlNamespace;
    QList<QString> m_templateParameters;
    QList<MComponentMember> m_members;
    QSet<Uid> m_visibleMembers;
    TemplateDisplay m_templateDisplay = TemplateSmart;
    bool m_showAllMembers = true;

    QHash <QString,QString> m_serviceInfo;//name : address;port;protocol;type
    QHash <QString,QString> m_propertyInfo;//name : type;value
    QHash <QString,QString> m_pinInfo;//name : value
};

} // namespace qmt
