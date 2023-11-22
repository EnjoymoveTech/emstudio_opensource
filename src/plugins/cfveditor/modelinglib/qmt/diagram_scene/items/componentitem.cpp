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

#include "componentitem.h"
#include "qmt/controller/namecontroller.h"
#include "qmt/diagram/dclass.h"
#include "qmt/diagram_controller/diagramcontroller.h"
#include "qmt/diagram/dcomponent.h"
#include "qmt/diagram/dinheritance.h"
#include "qmt/diagram_scene/diagramsceneconstants.h"
#include "qmt/diagram_scene/diagramscenemodel.h"
#include "qmt/diagram_scene/parts/contextlabelitem.h"
#include "qmt/diagram_scene/parts/customiconitem.h"
#include "qmt/diagram_scene/parts/editabletextitem.h"
#include "qmt/diagram_scene/parts/stereotypesitem.h"
#include "qmt/infrastructure/contextmenuaction.h"
#include "qmt/infrastructure/geometryutilities.h"
#include "qmt/infrastructure/qmtassert.h"
#include "qmt/stereotype/stereotypecontroller.h"
#include "qmt/stereotype/stereotypeicon.h"
#include "qmt/stereotype/customrelation.h"
#include "qmt/model/mcomponent.h"
#include "qmt/model/mcomponentmember.h"
#include "qmt/model/massociation.h"
#include "qmt/model/minheritance.h"
#include "qmt/model_controller/modelcontroller.h"
#include "qmt/style/style.h"
#include "qmt/tasks/diagramscenecontroller.h"
#include <utils/algorithm.h>
#include "qmt/tasks/ielementtasks.h"
#include "qmt/diagram_scene/parts/contextlabelitem.h"
#include "qmt/diagram_scene/parts/customiconitem.h"
#include "qmt/diagram_scene/parts/editabletextitem.h"
#include "qmt/diagram_scene/parts/relationstarter.h"
#include "qmt/diagram_scene/parts/stereotypesitem.h"
#include "qmt/diagram_scene/parts/templateparameterbox.h"

#include <coreplugin/editormanager/editormanager.h>
#include <emostools/cfvparser.h>
#include <utils/emoswidgets/messagetips.h>
#include "emostools/api_description_parser.h"

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QGraphicsSimpleTextItem>
#include <QBrush>
#include <QPen>
#include <QFont>
#include <QMenu>
#include <algorithm>

namespace qmt {

static const char ASSOCIATION[] = "association";
static const char INHERITANCE[] = "inheritance";

static const qreal MINIMUM_AUTO_WIDTH = 100.0;
static const qreal MINIMUM_AUTO_HEIGHT = 40.0;
static const qreal BODY_VERT_BORDER = 1.0;
static const qreal BODY_HORIZ_BORDER = 1.0;
static const qreal BODY_HORIZ_DISTANCE = 1.0;

ComponentItem::ComponentItem(DComponent *component, DiagramSceneModel *diagramSceneModel, QGraphicsItem *parent)
    : ObjectItem("component", component, diagramSceneModel, parent)
{
}

ComponentItem::~ComponentItem()
{
}

void ComponentItem::update()
{
    prepareGeometryChange();

    updateStereotypeIconDisplay();

    auto diagramClass = dynamic_cast<DComponent *>(object());
    QMT_ASSERT(diagramClass, return);

    const Style *style = adaptedStyle(stereotypeIconId());

    if (diagramClass->showAllMembers()) {
        updateMembers(style);
    } else {
        m_attributesText.clear();
        m_methodsText.clear();
    }

    // custom icon
    if (stereotypeIconDisplay() == StereotypeIcon::DisplayIcon) {
        if (!m_customIcon)
            m_customIcon = new CustomIconItem(diagramSceneModel(), this);
        m_customIcon->setStereotypeIconId(stereotypeIconId());
        m_customIcon->setBaseSize(stereotypeIconMinimumSize(m_customIcon->stereotypeIcon(), CUSTOM_ICON_MINIMUM_AUTO_WIDTH, CUSTOM_ICON_MINIMUM_AUTO_HEIGHT));
        m_customIcon->setBrush(style->fillBrush());
        m_customIcon->setPen(style->outerLinePen());
        m_customIcon->setZValue(SHAPE_ZVALUE);
    } else if (m_customIcon) {
        m_customIcon->scene()->removeItem(m_customIcon);
        delete m_customIcon;
        m_customIcon = nullptr;
    }

    // shape
    if (!m_customIcon) {
        if (!m_shape)
            m_shape = new QGraphicsRectItem(this);
        m_shape->setBrush(style->fillBrush());
        if(!exist())
            m_shape->setBrush(QColor(0, 0, 0, 60));
        m_shape->setPen(style->outerLinePen());
        m_shape->setZValue(SHAPE_ZVALUE);
    } else if (m_shape){
        m_shape->scene()->removeItem(m_shape);
        delete m_shape;
        m_shape = nullptr;
    }

    // stereotypes
    updateStereotypes(stereotypeIconId(), stereotypeIconDisplay(), style);

    // base classes
    DiagramController *diagramController = diagramSceneModel()->diagramController();
    ModelController *modelController = diagramController->modelController();
    MComponent *klass = modelController->findElement<MComponent>(diagramClass->modelUid());
    if (klass) {
        // TODO cache relation's Uids and check for change
        QList<QString> baseClasses;
        for (const auto &handle : klass->relations()) {
            if (MInheritance *mInheritance = dynamic_cast<MInheritance *>(handle.target())) {
                if (mInheritance->base() != klass->uid()) {
                    DInheritance *dInheritance = diagramController->findDelegate<DInheritance>(mInheritance, diagramSceneModel()->diagram());
                    if (dInheritance) {
                        DComponent *dBaseClass = diagramController->findElement<DComponent>(dInheritance->base(), diagramSceneModel()->diagram());
                        if (dBaseClass)
                            continue;
                    }
                    MComponent *mBaseClass = diagramController->modelController()->findElement<MComponent>(mInheritance->base());
                    if (mBaseClass) {
                        QString baseClassName;
                        baseClassName += mBaseClass->name();
                        baseClasses.append(baseClassName);
                    }
                }
            }
        }
        if (!baseClasses.isEmpty()) {
            if (!m_baseClasses)
                m_baseClasses = new QGraphicsSimpleTextItem(this);
            QFont font = style->smallFont();
            font.setItalic(true);
            m_baseClasses->setFont(font);
            m_baseClasses->setBrush(style->textBrush());
            m_baseClasses->setText(baseClasses.join('\n'));
        } else if (m_baseClasses) {
            if (m_baseClasses->scene())
                m_baseClasses->scene()->removeItem(m_baseClasses);
            delete m_baseClasses;
            m_baseClasses = nullptr;
        }
    }

    // namespace
    if (!suppressTextDisplay() && !diagramClass->umlNamespace().isEmpty()) {
        if (!m_namespace)
            m_namespace = new QGraphicsSimpleTextItem(this);
        m_namespace->setFont(style->smallFont());
        m_namespace->setBrush(style->textBrush());
        m_namespace->setText(diagramClass->umlNamespace());
    } else if (m_namespace) {
        m_namespace->scene()->removeItem(m_namespace);
        delete m_namespace;
        m_namespace = nullptr;
    }

    // class name
    updateNameItem(style);
    updateOidItem(style);

    // context
    if (!suppressTextDisplay() && showContext()) {
        if (!m_contextLabel)
            m_contextLabel = new ContextLabelItem(this);
        m_contextLabel->setFont(style->smallFont());
        m_contextLabel->setBrush(style->textBrush());
        m_contextLabel->setContext(object()->context());
    } else if (m_contextLabel) {
        m_contextLabel->scene()->removeItem(m_contextLabel);
        delete m_contextLabel;
        m_contextLabel = nullptr;
    }

    // attributes separator
    if (m_shape || (!suppressTextDisplay() && (!m_attributesText.isEmpty() || !m_methodsText.isEmpty()))) {
        if (!m_attributesSeparator)
            m_attributesSeparator = new QGraphicsLineItem(this);
        m_attributesSeparator->setPen(style->innerLinePen());
        m_attributesSeparator->setZValue(SHAPE_DETAILS_ZVALUE);
    } else if (m_attributesSeparator) {
        m_attributesSeparator->scene()->removeItem(m_attributesSeparator);
        delete m_attributesSeparator;
        m_attributesSeparator = nullptr;
    }

    // attributes
    if (!suppressTextDisplay() && !m_attributesText.isEmpty()) {
        if (!m_attributes)
            m_attributes = new QGraphicsTextItem(this);
        m_attributes->setFont(style->smallFont());
        //m_attributes->setBrush(style->textBrush());
        m_attributes->setDefaultTextColor(style->textBrush().color());
        m_attributes->setHtml(m_attributesText);
    } else if (m_attributes) {
        m_attributes->scene()->removeItem(m_attributes);
        delete m_attributes;
        m_attributes = nullptr;
    }

    // methods separator
    if (m_shape || (!suppressTextDisplay() && (!m_attributesText.isEmpty() || !m_methodsText.isEmpty()))) {
        if (!m_methodsSeparator)
            m_methodsSeparator = new QGraphicsLineItem(this);
        m_methodsSeparator->setPen(style->innerLinePen());
        m_methodsSeparator->setZValue(SHAPE_DETAILS_ZVALUE);
        m_methodsSeparator->setVisible(false);
    } else if (m_methodsSeparator) {
        m_methodsSeparator->scene()->removeItem(m_methodsSeparator);
        delete m_methodsSeparator;
        m_methodsSeparator = nullptr;
    }

    // methods
    if (!suppressTextDisplay() && !m_methodsText.isEmpty()) {
        if (!m_methods)
            m_methods = new QGraphicsTextItem(this);
        m_methods->setFont(style->smallFont());
        //m_methods->setBrush(style->textBrush());
        m_methods->setDefaultTextColor(style->textBrush().color());
        m_methods->setHtml(m_methodsText);
    } else if (m_methods) {
        m_methods->scene()->removeItem(m_methods);
        delete m_methods;
        m_methods = nullptr;
    }

    // template parameters
    if (templateDisplay() == DComponent::TemplateBox && !diagramClass->templateParameters().isEmpty()) {
        // TODO due to a bug in Qt the m_nameItem may get focus back when this item is newly created
        // 1. Select name item of class without template
        // 2. Click into template property (item name loses focus) and enter a letter
        // 3. Template box is created which gives surprisingly focus back to item name
        if (!m_templateParameterBox)
            m_templateParameterBox = new TemplateParameterBox(this);
        QPen pen = style->outerLinePen();
        pen.setStyle(Qt::DashLine);
        m_templateParameterBox->setPen(pen);
        m_templateParameterBox->setBrush(QBrush(Qt::white));
        m_templateParameterBox->setFont(style->smallFont());
        m_templateParameterBox->setTextBrush(style->textBrush());
        m_templateParameterBox->setTemplateParameters(diagramClass->templateParameters());
    } else if (m_templateParameterBox) {
        m_templateParameterBox->scene()->removeItem(m_templateParameterBox);
        delete m_templateParameterBox;
        m_templateParameterBox = nullptr;
    }

    updateSelectionMarker(m_customIcon);
    updateRelationStarter();
    updateAlignmentButtons();
    updateGeometry();
}

bool ComponentItem::intersectShapeWithLine(const QLineF &line, QPointF *intersectionPoint, QLineF *intersectionLine) const
{
    if (m_customIcon) {
        QList<QPolygonF> polygons = m_customIcon->outline();
        for (int i = 0; i < polygons.size(); ++i)
            polygons[i].translate(object()->pos() + object()->rect().topLeft());
        if (shapeIcon().textAlignment() == qmt::StereotypeIcon::TextalignBelow) {
            if (nameItem()) {
                QPolygonF polygon(nameItem()->boundingRect());
                polygon.translate(object()->pos() + nameItem()->pos());
                polygons.append(polygon);
            }
            if (oidItem()) {
                QPolygonF polygon(oidItem()->boundingRect());
                polygon.translate(object()->pos() + oidItem()->pos());
                polygons.append(polygon);
            }
            if (m_contextLabel) {
                QPolygonF polygon(m_contextLabel->boundingRect());
                polygon.translate(object()->pos() + m_contextLabel->pos());
                polygons.append(polygon);
            }
        }
        return GeometryUtilities::intersect(polygons, line, nullptr, intersectionPoint, intersectionLine);
    }
    QPolygonF polygon;
    QRectF rect = object()->rect();
    rect.translate(object()->pos());
    polygon << rect.topLeft() << rect.topRight() << rect.bottomRight() << rect.bottomLeft() << rect.topLeft();
    return GeometryUtilities::intersect(polygon, line, intersectionPoint, intersectionLine);
}

QSizeF ComponentItem::minimumSize() const
{
    return calcMinimumGeometry();
}

void ComponentItem::relationDrawn(const QString &id, ObjectItem *targetItem, const QList<QPointF> &points)
{
    QList<QPointF> intermediatePoints = points;

    DiagramSceneController *diagramSceneController = diagramSceneModel()->diagramSceneController();
    if (id == INHERITANCE) {
        auto baseClass = dynamic_cast<DComponent *>(targetItem->object());
        if (baseClass) {
            auto derivedClass = dynamic_cast<DComponent *>(object());
            QMT_ASSERT(derivedClass, return);
            diagramSceneController->createComInheritance(derivedClass, baseClass, intermediatePoints, diagramSceneModel()->diagram());
        }
        return;
    } else if (id == ASSOCIATION) {
        auto associatedClass = dynamic_cast<DComponent *>(targetItem->object());
        if (associatedClass) {
            auto derivedClass = dynamic_cast<DComponent *>(object());
            QMT_ASSERT(derivedClass, return);
            diagramSceneController->createComAssociation(derivedClass, associatedClass, intermediatePoints, diagramSceneModel()->diagram());
        }
        return;
    } else {
        StereotypeController *stereotypeController = diagramSceneModel()->stereotypeController();
        CustomRelation customRelation = stereotypeController->findCustomRelation(id);
        if (!customRelation.isNull()) {
            switch (customRelation.element()) {
            case CustomRelation::Element::Inheritance:
            {
                auto baseClass = dynamic_cast<DComponent *>(targetItem->object());
                if (baseClass) {
                    auto derivedClass = dynamic_cast<DComponent *>(object());
                    QMT_ASSERT(derivedClass, return);
                    diagramSceneController->createComInheritance(derivedClass, baseClass, intermediatePoints, diagramSceneModel()->diagram());
                }
                return;
            }
            case CustomRelation::Element::Association:
            {
                auto assoziatedClass = dynamic_cast<DComponent *>(targetItem->object());
                if (assoziatedClass) {
                    auto derivedClass = dynamic_cast<DComponent *>(object());
                    QMT_ASSERT(derivedClass, return);

                    if(checkSameRelation(derivedClass, assoziatedClass, &customRelation))//haojie.fang 是否已经有相同连线，相同oid的comp只能有一次连线
                        return;

                    if(!checkQuoteService(derivedClass, assoziatedClass, &customRelation))//haojie.fang 是否有引用此服务
                        return;

                    static const QHash<CustomRelation::Relationship, MAssociationEnd::Kind> relationship2KindMap = {
                        { CustomRelation::Relationship::Association, MAssociationEnd::Association },
                        { CustomRelation::Relationship::Aggregation, MAssociationEnd::Aggregation },
                        { CustomRelation::Relationship::Composition, MAssociationEnd::Composition },
                        { CustomRelation::Relationship::Method, MAssociationEnd::Method },
                        { CustomRelation::Relationship::Event, MAssociationEnd::Event },
                        { CustomRelation::Relationship::Field, MAssociationEnd::Field },
                        { CustomRelation::Relationship::Pin, MAssociationEnd::Pin }};

                    if(intermediatePoints.size() == 0)
                        intermediatePoints = DiagramSceneController::calConnectPoints(derivedClass, assoziatedClass, relationship2KindMap.value(customRelation.endA().relationship()));

                    diagramSceneController->createComAssociation(
                                derivedClass, assoziatedClass, intermediatePoints, diagramSceneModel()->diagram(),
                                [=] (MAssociation *mAssociation, DAssociation *dAssociation) {
                        if (mAssociation && dAssociation) {
                            diagramSceneController->modelController()->startUpdateRelation(mAssociation);
                            mAssociation->setStereotypes(Utils::toList(customRelation.stereotypes()));
                            mAssociation->setName(customRelation.name());
                            MAssociationEnd endA;
                            endA.setCardinality(customRelation.endA().cardinality());
                            endA.setKind(relationship2KindMap.value(customRelation.endA().relationship()));
                            endA.setName(customRelation.endA().role());
                            endA.setNavigable(customRelation.endA().navigable());
                            mAssociation->setEndA(endA);
                            MAssociationEnd endB;
                            endB.setCardinality(customRelation.endB().cardinality());
                            endB.setKind(relationship2KindMap.value(customRelation.endB().relationship()));
                            endB.setName(customRelation.endB().role());
                            endB.setNavigable(customRelation.endB().navigable());
                            mAssociation->setEndB(endB);
                            diagramSceneController->modelController()->finishUpdateRelation(mAssociation, false);
                        }
                    });
                }
                return;
            }
            case CustomRelation::Element::Dependency:
            case CustomRelation::Element::Relation:
                // fall thru
                break;
            }
        }
    }
    ObjectItem::relationDrawn(id, targetItem, intermediatePoints);
}

bool ComponentItem::extendContextMenu(QMenu *menu)
{
    bool extended = false;
    if (diagramSceneModel()->diagramSceneController()->elementTasks()->hasClassDefinition(object(), diagramSceneModel()->diagram())) {
        menu->addAction(new ContextMenuAction(tr("Show Definition"), "showDefinition", menu));
        extended = true;
    }
    return extended;
}

bool ComponentItem::handleSelectedContextMenuAction(const QString &id)
{
    if (id == "showDefinition") {
        diagramSceneModel()->diagramSceneController()->elementTasks()->openClassDefinition(object(), diagramSceneModel()->diagram());
        return true;
    }
    return false;
}

QString ComponentItem::buildDisplayName() const
{
    auto diagramClass = dynamic_cast<DComponent *>(object());
    QMT_ASSERT(diagramClass, return QString());

    QString name;
    if (templateDisplay() == DComponent::TemplateName && !diagramClass->templateParameters().isEmpty()) {
        name = object()->name();
        name += QLatin1Char('<');
        bool first = true;
        foreach (const QString &p, diagramClass->templateParameters()) {
            if (!first)
                name += QLatin1Char(',');
            name += p;
            first = false;
        }
        name += QLatin1Char('>');
    } else {
        name = object()->name();
    }
    return name;
}

void ComponentItem::setFromDisplayName(const QString &displayName)
{
    if (templateDisplay() == DComponent::TemplateName) {
        QString name;
        QStringList templateParameters;
        // NOTE namespace is ignored because it has its own edit field
        if (NameController::parseClassName(displayName, nullptr, &name, &templateParameters)) {
            auto diagramClass = dynamic_cast<DComponent *>(object());
            QMT_ASSERT(diagramClass, return);
            ModelController *modelController = diagramSceneModel()->diagramSceneController()->modelController();
            MComponent *mklass = modelController->findObject<MComponent>(diagramClass->modelUid());
            if (mklass && (name != mklass->name() || templateParameters != mklass->templateParameters())) {
                modelController->startUpdateObject(mklass);
                mklass->setName(name);
                mklass->setTemplateParameters(templateParameters);
                modelController->finishUpdateObject(mklass, false);
            }
        }
    } else {
        ObjectItem::setFromDisplayName(displayName);
    }
}

QList<ILatchable::Latch> ComponentItem::horizontalLatches(ILatchable::Action action, bool grabbedItem) const
{
    return ObjectItem::horizontalLatches(action, grabbedItem);
}

QList<ILatchable::Latch> ComponentItem::verticalLatches(ILatchable::Action action, bool grabbedItem) const
{
    return ObjectItem::verticalLatches(action, grabbedItem);
}

void ComponentItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    if(m_idl.isValid)
    {
        QString path = EmosTools::IdlParser::getIdlFullPath(m_idl.oid);
        Core::EditorManager::openEditor(path);
    }
}

void ComponentItem::addRelationStarterTool(const QString &id)
{
    ObjectItem::addRelationStarterTool(id);
}

void ComponentItem::addRelationStarterTool(const CustomRelation &customRelation)
{//haojie.fang 添加箭头类型

    //haojie.fang 检测是否需要加箭头工具
    if(customRelation.id() == "Method" && !m_hasMethod)
        return;
    if(customRelation.id() == "Event" && !m_hasEvent)
        return;
    if(customRelation.id() == "Field" && !m_hasField)
        return;
    if(customRelation.id() == "Pin" && !m_hasPin)
        return;

    ArrowItem::Shaft shaft = ArrowItem::ShaftSolid;
    ArrowItem::Head headStart = ArrowItem::HeadNone;
    ArrowItem::Head headEnd = ArrowItem::HeadNone;

    //CustomRelation::Element::Association

    switch (customRelation.endB().relationship())
    {
    case CustomRelation::Relationship::Association:
    case CustomRelation::Relationship::Aggregation:
    case CustomRelation::Relationship::Composition:
        break;
        //haojie.fang add custom type
    case CustomRelation::Relationship::Method:
        shaft = ArrowItem::ShaftSolid;
        headStart = ArrowItem::HeadTriangle;
        break;
    case CustomRelation::Relationship::Event:
        shaft = ArrowItem::ShaftSolid;
        headEnd = ArrowItem::HeadTriangle;
        break;
    case CustomRelation::Relationship::Field:
        shaft = ArrowItem::ShaftSolid;
        headEnd = ArrowItem::HeadTriangle;
        break;
    case CustomRelation::Relationship::Pin:
        shaft = ArrowItem::ShaftSolid;
        headEnd = ArrowItem::HeadTriangle;
        break;
    }

    relationStarter()->addArrow(customRelation.id(), shaft, headStart, headEnd,
                                customRelation.title());
}

void ComponentItem::addStandardRelationStarterTools()
{
    ObjectItem::addStandardRelationStarterTools();
    addRelationStarterTool(INHERITANCE);
    addRelationStarterTool(ASSOCIATION);
}

bool ComponentItem::hasPlainShape() const
{
    auto diagramComponent = dynamic_cast<DComponent *>(object());
    QMT_ASSERT(diagramComponent, return false);
    return diagramComponent->isPlainShape();
}

DComponent::TemplateDisplay ComponentItem::templateDisplay() const
{
    auto diagramClass = dynamic_cast<DComponent *>(object());
    QMT_ASSERT(diagramClass, return DComponent::TemplateSmart);

    DComponent::TemplateDisplay templateDisplay = diagramClass->templateDisplay();
    if (templateDisplay == DComponent::TemplateSmart) {
        if (m_customIcon)
            templateDisplay = DComponent::TemplateName;
        else
            templateDisplay = DComponent::TemplateBox;
    }
    return templateDisplay;
}

QSizeF ComponentItem::calcMinimumGeometry() const
{
    double width = 0.0;
    double height = 0.0;

    if (m_customIcon) {
        QSizeF sz = stereotypeIconMinimumSize(m_customIcon->stereotypeIcon(),
                                              CUSTOM_ICON_MINIMUM_AUTO_WIDTH, CUSTOM_ICON_MINIMUM_AUTO_HEIGHT);
        if (shapeIcon().textAlignment() != qmt::StereotypeIcon::TextalignTop
                && shapeIcon().textAlignment() != qmt::StereotypeIcon::TextalignCenter)
            return sz;
        width = sz.width();
    }

    height += BODY_VERT_BORDER;
    double top_row_width = BODY_HORIZ_DISTANCE;
    double top_row_height = 0;
    if (CustomIconItem *stereotypeIconItem = this->stereotypeIconItem()) {
        top_row_width += stereotypeIconItem->boundingRect().width();
        top_row_height = stereotypeIconItem->boundingRect().height();
    }
    if (m_baseClasses) {
        top_row_width += m_baseClasses->boundingRect().width();
        top_row_height = std::max(top_row_height, m_baseClasses->boundingRect().height());
    }
    width = std::max(width, top_row_width);
    height += top_row_height;
    if (StereotypesItem *stereotypesItem = this->stereotypesItem()) {
        width = std::max(width, stereotypesItem->boundingRect().width());
        height += stereotypesItem->boundingRect().height();
    }
    if (m_namespace) {
        width = std::max(width, m_namespace->boundingRect().width());
        height += m_namespace->boundingRect().height();
    }
    if (nameItem()) {
        width = std::max(width, nameItem()->boundingRect().width());
        height += nameItem()->boundingRect().height();
    }
    if (oidItem()) {
        width = std::max(width, oidItem()->boundingRect().width());
        height += oidItem()->boundingRect().height();
    }
    if (m_contextLabel)
        height += m_contextLabel->height();
    if (m_attributesSeparator)
        height += 2.0;
    if (m_attributes) {
        width = std::max(width, m_attributes->boundingRect().width());
        height += m_attributes->boundingRect().height();
    }
    if (m_methodsSeparator)
        height += 2.0;
    if (m_methods) {
        width = std::max(width, m_methods->boundingRect().width());
        height += m_methods->boundingRect().height();
    }
    height += BODY_VERT_BORDER;

    width = BODY_HORIZ_BORDER + width + BODY_HORIZ_BORDER;

    return GeometryUtilities::ensureMinimumRasterSize(QSizeF(width, height), 2 * RASTER_WIDTH, 2 * RASTER_HEIGHT);
}

void ComponentItem::updateGeometry()
{
    prepareGeometryChange();

    // calc width and height
    double width = 0.0;
    double height = 0.0;

    QSizeF geometry = calcMinimumGeometry();
    width = geometry.width();
    height = geometry.height();

    if (object()->isAutoSized()) {
        if (!m_customIcon) {
            if (width < MINIMUM_AUTO_WIDTH)
                width = MINIMUM_AUTO_WIDTH;
            if (height < MINIMUM_AUTO_HEIGHT)
                height = MINIMUM_AUTO_HEIGHT;
        }
    } else {
        QRectF rect = object()->rect();
        if (rect.width() > width)
            width = rect.width();
        if (rect.height() > height)
            height = rect.height();
    }

    // update sizes and positions
    double left = -width / 2.0;
    double right = width / 2.0;
    double top = -height / 2.0;
    //double bottom = height / 2.0;
    double y = top;

    setPos(object()->pos());

    QRectF rect(left, top, width, height);

    // the object is updated without calling DiagramController intentionally.
    // attribute rect is not a real attribute stored on DObject but
    // a backup for the graphics item used for manual resized and persistency.
    object()->setRect(rect);

    if (m_customIcon) {
        m_customIcon->setPos(left, top);
        m_customIcon->setActualSize(QSizeF(width, height));
    }

    if (m_shape)
        m_shape->setRect(rect);

    if (m_customIcon) {
        switch (shapeIcon().textAlignment()) {
        case qmt::StereotypeIcon::TextalignBelow:
            y += height + BODY_VERT_BORDER;
            break;
        case qmt::StereotypeIcon::TextalignCenter:
        {
            double h = 0.0;
            if (CustomIconItem *stereotypeIconItem = this->stereotypeIconItem())
                h += stereotypeIconItem->boundingRect().height();
            if (StereotypesItem *stereotypesItem = this->stereotypesItem())
                h += stereotypesItem->boundingRect().height();
            if (nameItem())
                h += nameItem()->boundingRect().height();
            if (oidItem())
                h += oidItem()->boundingRect().height();
            if (m_contextLabel)
                h += m_contextLabel->height();
            y = top + (height - h) / 2.0;
            break;
        }
        case qmt::StereotypeIcon::TextalignNone:
            // nothing to do
            break;
        case qmt::StereotypeIcon::TextalignTop:
            y += BODY_VERT_BORDER;
            break;
        }
    } else {
        y += BODY_VERT_BORDER;
    }
    double first_row_height = 0;
    if (m_baseClasses) {
        m_baseClasses->setPos(left + BODY_HORIZ_BORDER, y);
        first_row_height = m_baseClasses->boundingRect().height();
    }
    if (CustomIconItem *stereotypeIconItem = this->stereotypeIconItem()) {
        stereotypeIconItem->setPos(right - stereotypeIconItem->boundingRect().width() - BODY_HORIZ_BORDER, y);
        first_row_height = std::max(first_row_height, stereotypeIconItem->boundingRect().height());
    }
    y += first_row_height;
    if (StereotypesItem *stereotypesItem = this->stereotypesItem()) {
        stereotypesItem->setPos(-stereotypesItem->boundingRect().width() / 2.0, y);
        y += stereotypesItem->boundingRect().height();
    }
    if (m_namespace) {
        m_namespace->setPos(-m_namespace->boundingRect().width() / 2.0, y);
        y += m_namespace->boundingRect().height();
    }
    if (nameItem()) {
        nameItem()->setPos(-nameItem()->boundingRect().width() / 2.0, y);
        y += nameItem()->boundingRect().height();
    }
    if (oidItem()) {
        oidItem()->setPos(-oidItem()->boundingRect().width() / 2.0, y);
        y += oidItem()->boundingRect().height();
    }
    if (m_contextLabel) {
        if (m_customIcon)
            m_contextLabel->resetMaxWidth();
        else
            m_contextLabel->setMaxWidth(width - 2 * BODY_HORIZ_BORDER);
        m_contextLabel->setPos(-m_contextLabel->boundingRect().width() / 2.0, y);
        y += m_contextLabel->boundingRect().height();
    }
    if (m_attributesSeparator) {
        m_attributesSeparator->setLine(left, 4.0, right, 4.0);
        m_attributesSeparator->setPos(0, y);
        y += 2.0;
    }
    if (m_attributes) {
        if (m_customIcon)
            m_attributes->setPos(-m_attributes->boundingRect().width() / 2.0, y);
        else
            m_attributes->setPos(left + BODY_HORIZ_BORDER, y);
        y += m_attributes->boundingRect().height();
    }

    if (m_methodsSeparator) {
        m_methodsSeparator->setLine(left, 4.0, right, 4.0);
        m_methodsSeparator->setPos(0, y);
        y += 2.0;
    }
    if (m_methods) {
        if (m_customIcon)
            m_methods->setPos(-m_methods->boundingRect().width() / 2.0, y);
        else
            m_methods->setPos(left + BODY_HORIZ_BORDER, y);
        y += m_methods->boundingRect().height();
    }

    if (m_templateParameterBox) {
        m_templateParameterBox->setBreakLines(false);
        double x = right - m_templateParameterBox->boundingRect().width() * 0.8;
        if (x < 0) {
            m_templateParameterBox->setBreakLines(true);
            x = right - m_templateParameterBox->boundingRect().width() * 0.8;
        }
        if (x < 0)
            x = 0;
        m_templateParameterBox->setPos(x, top - m_templateParameterBox->boundingRect().height() + BODY_VERT_BORDER);
    }

    updateSelectionMarkerGeometry(rect);
    updateRelationStarterGeometry(rect);
    updateAlignmentButtonsGeometry(rect);
    updateDepth();
}


void ComponentItem::updateMembers(const Style *style)
{
    Q_UNUSED(style)

    m_attributesText.clear();
    m_methodsText.clear();

    //haojie.fang 先占位
    //m_attributesText= "<ul><li>method</li><li>event</li><li>field</li></ul>";

    m_hasMethod = false;
    m_hasEvent = false;
    m_hasField = false;
    m_hasPin = false;

    m_idl = EmosTools::IdlParser::getIdlStruct(this->object()->oid());
    if(!m_idl.service.isEmpty())
    {
        QMap<QString, int> servicemap = m_idl.service;
        foreach (const QString& key,servicemap.keys())
        {
            QString sName = key;
            EmosTools::tSrvService srv = EmosTools::ApiParser::getInstance()->getService(sName);
            if (srv.id != -1)
            {
                if(!srv.functions.isEmpty())
                    m_hasMethod = true;
                if(!srv.events.isEmpty())
                    m_hasEvent = true;
                if(!srv.fields.isEmpty())
                    m_hasField = true;
                if(!srv.pins.isEmpty())
                    m_hasPin = true;
            }
        }
    }
    if(m_hasMethod)
        m_attributesText += "📕 method<br>";
    if(m_hasEvent)
        m_attributesText += "📘 event<br>";
    if(m_hasField)
        m_attributesText += "📗 field<br>";
    if(m_hasPin)
        m_attributesText += "📔 pin<br>";
    m_attributesText = m_attributesText.left(m_attributesText.length() - 4);

    MComponentMember::Visibility attributesVisibility = MComponentMember::VisibilityUndefined;
    MComponentMember::Visibility methodsVisibility = MComponentMember::VisibilityUndefined;
    QString attributesGroup;
    QString methodsGroup;

    MComponentMember::Visibility *currentVisibility = nullptr;
    QString *currentGroup = nullptr;
    QString *text = nullptr;

    auto dclass = dynamic_cast<DComponent *>(object());
    QMT_ASSERT(dclass, return);

    foreach (const MComponentMember &member, dclass->members()) {
        switch (member.memberType()) {
        case MComponentMember::MemberUndefined:
            QMT_ASSERT(false, return);
            break;
        case MComponentMember::MemberAttribute:
            currentVisibility = &attributesVisibility;
            currentGroup = &attributesGroup;
            text = &m_attributesText;
            break;
        case MComponentMember::MemberMethod:
            currentVisibility = &methodsVisibility;
            currentGroup = &methodsGroup;
            text = &m_methodsText;
            break;
        }

        if (text && !text->isEmpty())
            *text += "<br/>";

        bool addNewline = false;
        bool addSpace = false;
        if (currentVisibility)
            *currentVisibility = member.visibility();
        if (currentGroup && member.group() != *currentGroup) {
            *text += QString("[%1]").arg(member.group());
            addNewline = true;
            *currentGroup = member.group();
        }
        if (addNewline)
            *text += "<br/>";

        bool haveSignal = false;
        bool haveSlot = false;
        if (member.visibility() != MComponentMember::VisibilityUndefined) {
            QString vis;
            switch (member.visibility()) {
            case MComponentMember::VisibilityUndefined:
                break;
            case MComponentMember::VisibilityPublic:
                vis = "+";
                addSpace = true;
                break;
            case MComponentMember::VisibilityProtected:
                vis = "#";
                addSpace = true;
                break;
            case MComponentMember::VisibilityPrivate:
                vis = "-";
                addSpace = true;
                break;
            case MComponentMember::VisibilitySignals:
                vis = "&gt;";
                haveSignal = true;
                addSpace = true;
                break;
            case MComponentMember::VisibilityPrivateSlots:
                vis = "-$";
                haveSlot = true;
                addSpace = true;
                break;
            case MComponentMember::VisibilityProtectedSlots:
                vis = "#$";
                haveSlot = true;
                addSpace = true;
                break;
            case MComponentMember::VisibilityPublicSlots:
                vis = "+$";
                haveSlot = true;
                addSpace = true;
                break;
            }
            *text += vis;
        }

        if (member.properties() & MComponentMember::PropertyQsignal && !haveSignal) {
            *text += "&gt;";
            addSpace = true;
        }
        if (member.properties() & MComponentMember::PropertyQslot && !haveSlot) {
            *text += "$";
            addSpace = true;
        }
        if (addSpace)
            *text += " ";
        if (member.properties() & MComponentMember::PropertyQinvokable)
            *text += "invokable ";
        if (!member.stereotypes().isEmpty()) {
            *text += StereotypesItem::format(member.stereotypes());
            *text += " ";
        }
        if (member.properties() & MComponentMember::PropertyStatic)
            *text += "static ";
        if (member.properties() & MComponentMember::PropertyVirtual)
            *text += "virtual ";
        *text += member.declaration().toHtmlEscaped();
        if (member.properties() & MComponentMember::PropertyConst)
            *text += " const";
        if (member.properties() & MComponentMember::PropertyOverride)
            *text += " override";
        if (member.properties() & MComponentMember::PropertyFinal)
            *text += " final";
        if (member.properties() & MComponentMember::PropertyAbstract)
            *text += " = 0";
    }
}

bool ComponentItem::checkSameRelation(DComponent *object, DComponent *targetObject, CustomRelation* customRelation)
{
    ModelController *modelController = diagramSceneModel()->diagramSceneController()->modelController();
    MComponent *objectModel = modelController->findObject<MComponent>(object->modelUid());
    MComponent *targetObjectModel = modelController->findObject<MComponent>(targetObject->modelUid());

    //当前item的连线检测
    for(MRelation * relation : modelController->findRelationsOfObject(dynamic_cast<MObject *>(objectModel)))
    {
        if(dynamic_cast<MAssociation *>(relation)->endBUid() == targetObjectModel->uid() &&//相同uid同种类的线不能连多次
            (int)dynamic_cast<MAssociation *>(relation)->endA().kind() == (int)customRelation->endA().relationship())
            return true;

        if(dynamic_cast<MAssociation *>(relation)->endBOid() == targetObjectModel->oid() &&//相同oid,不同uid的线不能连多次
           dynamic_cast<MAssociation *>(relation)->endBUid() != targetObjectModel->uid())
            return true;
    }

    //目标item的连线检测
    for(MRelation * relation : modelController->findRelationsOfObject(dynamic_cast<MObject *>(targetObjectModel)))
    {
        if(dynamic_cast<MAssociation *>(relation)->endAOid() == objectModel->oid() &&//相同oid,不同uid的线不能连多次
           dynamic_cast<MAssociation *>(relation)->endAUid() != objectModel->uid())
            return true;
    }

    return false;
}


bool ComponentItem::checkQuoteService(DComponent *object, DComponent *targetObject, CustomRelation *customRelation)
{
    if(targetObject->oid() == "Studio")
        return true;

    EmosTools::IDLStruct idlStruct = EmosTools::IdlParser::getIdlStruct(targetObject->oid());

    QHashIterator<QString, QString> i(object->serviceInfo());
    while (i.hasNext()) {
        i.next();

        if(idlStruct.quoteService.contains(i.key()))
            return true;
    }
    QWidget* widget = scene()->views().first()->viewport();
    EmosWidgets::MessageTips::showText(widget, "not quote service", EmosWidgets::MessageTips::SmallText,
                                       EmosWidgets::MessageTips::PositionBottom, EmosWidgets::MessageTips::Critical,1000);

    return false;
}

} // namespace qmt
