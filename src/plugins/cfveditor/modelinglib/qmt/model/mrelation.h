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

#include "melement.h"

#include "qmt/infrastructure/handle.h"

namespace qmt {

class MObject;

class QMT_EXPORT MRelation : public MElement
{
public:
    MRelation();
    MRelation(const MRelation &rhs);
    ~MRelation() override;

    MRelation &operator=(const MRelation &rhs);

    QString name() const { return m_name; }
    void setName(const QString &name);
    Uid endAUid() const { return m_endAUid; }
    void setEndAUid(const Uid &uid);
    Uid endBUid() const { return m_endBUid; }
    void setEndBUid(const Uid &uid);

    QString endAOid() const { return m_endAOid; }
    void setEndAOid(const QString &oid);
    QString endBOid() const { return m_endBOid; }
    void setEndBOid(const QString &oid);

    QString endAName() const { return m_endAName; }
    void setEndAName(const QString &name);
    QString endBName() const { return m_endBName; }
    void setEndBName(const QString &name);

    void accept(MVisitor *visitor) override;
    void accept(MConstVisitor *visitor) const override;

private:
    QString m_name;
    Uid m_endAUid;
    Uid m_endBUid;

    QString m_endAOid;
    QString m_endBOid;
    QString m_endAName;
    QString m_endBName;
};

} // namespace qmt
