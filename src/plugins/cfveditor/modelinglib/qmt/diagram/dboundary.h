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

#include "delement.h"

#include <QPointF>
#include <QRectF>
#include <QHash>

namespace qmt {

class QMT_EXPORT DBoundary : public DElement
{
public:
    DBoundary();
    DBoundary(const DBoundary &rhs);
    ~DBoundary() override;

    DBoundary &operator=(const DBoundary &rhs);

    Uid modelUid() const override { return Uid::invalidUid(); }
    QString text() const { return m_text; }
    void setText(const QString &text);
    QPointF pos() const { return m_pos; }
    void setPos(const QPointF &pos);
    QRectF rect() const { return m_rect; }
    void setRect(const QRectF &rect);

    QHash <QString,QString> staticInfo() const { return m_staticInfo; }
    void setStaticInfo(const QHash <QString,QString> &staticInfo);
    bool staticSelected() const { return m_staticSelected; }
    void setStaticSelected(bool staticSelected);
    QHash <QString,QString> baseSrvInfo() const { return m_baseSrvInfo; }
    void setBaseSrvInfo(const QHash <QString,QString> &baseSrvInfo);
    QList<QString> baseSrvSelect() const { return m_baseSrvSelect; }
    void setBaseSrvSelect(const QList<QString> &baseSrvSelect);

    void accept(DVisitor *visitor) override;
    void accept(DConstVisitor *visitor) const override;
private:
    QString m_text;
    QPointF m_pos;
    QRectF m_rect;

    QHash <QString,QString> m_staticInfo;// processName, proxyPort
    bool m_staticSelected = false;
    QHash <QString,QString> m_baseSrvInfo;// service;property  : value
    QList<QString> m_baseSrvSelect;
};

} // namespace qmt
