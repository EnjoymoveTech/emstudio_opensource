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

#include "ddevice.h"

#include "dvisitor.h"
#include "dconstvisitor.h"

namespace qmt {

DDevice::DDevice()
    : DElement()
{
}

DDevice::DDevice(const DDevice &rhs)
    : DElement(rhs),
      m_text(rhs.m_text),
      m_pos(rhs.m_pos),
      m_rect(rhs.m_rect),
      m_ip(rhs.m_ip),
      m_ecuID(rhs.m_ecuID)
{
}

DDevice::~DDevice()
{
}

DDevice &DDevice::operator=(const DDevice &rhs)
{
    if (this != &rhs) {
        DElement::operator=(rhs);
        m_text = rhs.m_text;
        m_pos = rhs.m_pos;
        m_rect = rhs.m_rect;
    }
    return *this;
}

void DDevice::setText(const QString &text)
{
    m_text = text;
}

void DDevice::setPos(const QPointF &pos)
{
    m_pos = pos;
}

void DDevice::setRect(const QRectF &rect)
{
    m_rect = rect;
}

void DDevice::accept(DVisitor *visitor)
{
    visitor->visitDDevice(this);
}

void DDevice::accept(DConstVisitor *visitor) const
{
    visitor->visitDDevice(this);
}

void DDevice::setIp(const QString &ip)
{
    m_ip = ip;
}

void DDevice::setEcuID(int ecuID)
{
    m_ecuID = ecuID;
}

} // namespace qmt
