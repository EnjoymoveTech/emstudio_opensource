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

#include "qmt/infrastructure/uid.h"

#include <QString>

namespace qmt {

class DVisitor;
class DConstVisitor;

class QMT_EXPORT DElement
{
public:
    DElement();
    virtual ~DElement();

    Uid uid() const { return m_uid; }
    void setUid(const Uid &uid);
    QString oid() const { return m_oid; }
    void setOid(const QString &oid);
    void renewUid();
    virtual Uid modelUid() const = 0;

    virtual void accept(DVisitor *visitor);
    virtual void accept(DConstVisitor *visitor) const;

private:
    Uid m_uid;
    QString m_oid;
};

} // namespace qmt
