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

#include "diagramview.h"

#include "qmt/diagram_ui/diagram_mime_types.h"

#include "qmt/infrastructure/uid.h"
#include "qmt/diagram_scene/diagramscenemodel.h"
#include "qmt/tasks/diagramscenecontroller.h"

#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDragLeaveEvent>
#include <QDropEvent>
#include <QMimeData>
#include <utils/dropsupport.h>

#define DROP_VALUE_SIZE 4

namespace qmt {

DiagramView::DiagramView(QWidget *parent)
    : QGraphicsView(parent)
{
    setAcceptDrops(true);
    setAlignment(Qt::AlignLeft | Qt::AlignTop);
    setDragMode(QGraphicsView::RubberBandDrag);
    setFrameShape(QFrame::NoFrame);
    setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
}

DiagramView::~DiagramView()
{
}

DiagramSceneModel *DiagramView::diagramSceneModel() const
{
    return m_diagramSceneModel;
}

void DiagramView::setDiagramSceneModel(DiagramSceneModel *diagramSceneModel)
{
    setScene(nullptr);
    m_diagramSceneModel = diagramSceneModel;
    if (diagramSceneModel) {
        setScene(m_diagramSceneModel->graphicsScene());
        connect(m_diagramSceneModel, &DiagramSceneModel::sceneRectChanged,
                this, &DiagramView::onSceneRectChanged, Qt::QueuedConnection);
        // Signal is connected after diagram is updated. Enforce setting of scene rect.
        onSceneRectChanged(m_diagramSceneModel->sceneRect());
    }
}

void DiagramView::dragEnterEvent(QDragEnterEvent *event)
{
    const Utils::DropMimeData * internalData = qobject_cast<const Utils::DropMimeData *>(event->mimeData());
    if (internalData) {
        if(internalData->values().size() != DROP_VALUE_SIZE)
        {
            event->ignore();
            return;
        }
        bool accept = false;
        QString newElementId = internalData->values()[0].toString();
        if (!newElementId.isEmpty()) {
            accept = true;
        }
        event->setAccepted(accept);
    }
    else
    {
        event->ignore();
    }
}

void DiagramView::dragLeaveEvent(QDragLeaveEvent *event)
{
    event->accept();
}

void DiagramView::dragMoveEvent(QDragMoveEvent *event)
{
    event->accept();
}

void DiagramView::dropEvent(QDropEvent *event)
{
    event->setDropAction(Qt::MoveAction);
    const Utils::DropMimeData * internalData = qobject_cast<const Utils::DropMimeData *>(event->mimeData());
    if (internalData) {
        if(internalData->values().size() != DROP_VALUE_SIZE)
        {
            event->ignore();
            return;
        }
        DiagramSceneController *diagramSceneController = m_diagramSceneModel->diagramSceneController();

        event->accept();
        QString newElementId = internalData->values()[0].toString();
        QString name = internalData->values()[1].toString();
        QString stereotype = internalData->values()[2].toString();
        QString oid = internalData->values()[3].toString();
        if (!newElementId.isEmpty()) {
            QPointF pos = mapToScene(event->pos());
            diagramSceneController->dropNewElement(//haojie.fang 添加元素
                        newElementId, name, stereotype, oid, m_diagramSceneModel->findTopmostElement(pos),
                        pos, m_diagramSceneModel->diagram(), event->pos(), size());
        }
    }
    else
    {
        event->ignore();
    }
}

void DiagramView::onSceneRectChanged(const QRectF &sceneRect)
{
    // add some adjustment to all 4 sides
    static const qreal ADJUSTMENT = 80;
    QRectF rect = sceneRect.adjusted(-ADJUSTMENT, -ADJUSTMENT, ADJUSTMENT, ADJUSTMENT);
    setSceneRect(rect);
}

} // namespace qmt
