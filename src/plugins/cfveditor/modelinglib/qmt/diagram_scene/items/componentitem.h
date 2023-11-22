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

#include "objectitem.h"
#include "qmt/diagram/dcomponent.h"
#include "emostools/cfvparser.h"

QT_BEGIN_NAMESPACE
class QGraphicsRectItem;
class QGraphicsSimpleTextItem;
class QGraphicsLineItem;
QT_END_NAMESPACE

namespace qmt {

class DiagramSceneModel;
class DComponent;
class CustomIconItem;
class ContextLabelItem;
class RelationStarter;
class TemplateParameterBox;

class ComponentItem : public ObjectItem
{
public:
    ComponentItem(DComponent *component, DiagramSceneModel *diagramSceneModel,
                  QGraphicsItem *parent = nullptr);
    ~ComponentItem() override;

    void update() override;

    bool intersectShapeWithLine(const QLineF &line, QPointF *intersectionPoint,
                                QLineF *intersectionLine) const override;

    QSizeF minimumSize() const override;

    //haojie.fang 增加接收连线的类型
    void relationDrawn(const QString &id, ObjectItem *targetElement,
                       const QList<QPointF> &intermediatePoints) override;

    QList<Latch> horizontalLatches(Action action, bool grabbedItem) const override;
    QList<Latch> verticalLatches(Action action, bool grabbedItem) const override;

protected:
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
    bool extendContextMenu(QMenu *menu) override;
    bool handleSelectedContextMenuAction(const QString &id) override;
    QString buildDisplayName() const override;
    void setFromDisplayName(const QString &displayName) override;
    void addRelationStarterTool(const QString &id) override;
    void addRelationStarterTool(const CustomRelation &customRelation) override;
    void addStandardRelationStarterTools() override;
private:
    DComponent::TemplateDisplay templateDisplay() const;
    bool hasPlainShape() const;
    QSizeF calcMinimumGeometry() const;
    void updateGeometry();
    void updateMembers(const Style *style);
    bool checkSameRelation(DComponent *object, DComponent *targetObject, CustomRelation* customRelation);
    bool checkQuoteService(DComponent *object, DComponent *targetObject, CustomRelation* customRelation);

    CustomIconItem *m_customIcon = nullptr;
    QGraphicsRectItem *m_shape = nullptr;
    QGraphicsSimpleTextItem *m_baseClasses = nullptr;
    QGraphicsSimpleTextItem *m_namespace = nullptr;
    ContextLabelItem *m_contextLabel = nullptr;
    QGraphicsLineItem *m_attributesSeparator = nullptr;
    QString m_attributesText;
    QGraphicsTextItem *m_attributes = nullptr;
    QGraphicsLineItem *m_methodsSeparator = nullptr;
    QString m_methodsText;
    QGraphicsTextItem *m_methods = nullptr;
    TemplateParameterBox *m_templateParameterBox = nullptr;

    bool m_hasMethod = false;
    bool m_hasEvent = false;
    bool m_hasField = false;
    bool m_hasPin = false;
    EmosTools::IDLStruct m_idl;
};

} // namespace qmt
