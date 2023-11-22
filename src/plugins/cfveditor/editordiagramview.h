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

#include "qmt/diagram_widgets_ui/diagramview.h"
#include <utils/dropsupport.h>

#include "qmt/diagram_scene/items/associationitem.h"
#include "qmt/diagram/delement.h"
#include "qmt/diagram/dassociation.h"
#include "qmt/diagram/dcomponent.h"
#include "qmt/diagram/dboundary.h"
#include "qmt/diagram/ddevice.h"

namespace ModelEditor {
namespace Internal {

class PxNodeController;

class EditorDiagramView :
        public qmt::DiagramView
{
    Q_OBJECT
    class EditorDiagramViewPrivate;

public:
    explicit EditorDiagramView(QWidget *parent = nullptr);
    ~EditorDiagramView() override;

public slots:
    void showItem(int flag);
    void autoConnect();
    void beautifulConnect();
    void disconnect();
    void updateAll(const QStringList& oid);
signals:
    void zoomIn(const QPoint &zoomOrigin);
    void zoomOut(const QPoint &zoomOrigin);
public:
    void setPxNodeController(PxNodeController *pxNodeController);

protected:
    void wheelEvent(QWheelEvent *wheelEvent) override;

private:
    void connect(qmt::DComponent* A, qmt::DComponent* B, const QString &type);
    bool allowAssociation(qmt::DComponent* A, qmt::DComponent* B, const QString &type);
    void addAssociation(qmt::DComponent* A, qmt::DComponent* B, const QString &type);
    void setAssociation(qmt::DComponent* A, qmt::DComponent* B);
    bool hasRunableConnected(const qmt::MObject * elementB, qmt::MAssociationEnd::Kind kind, const QString& runableName);
    QList<QPointF> calConnectPoints(qmt::DComponent* A, qmt::DComponent* B, const QString &type);

    void dropProjectExplorerNodes(const QList<QVariant> &values, const QPoint &pos);
    void dropFiles(const QList<Utils::DropSupport::FileSpec> &files, const QPoint &pos);

    EditorDiagramViewPrivate *d;
};

} // namespace Internal
} // namespace ModelEditor
