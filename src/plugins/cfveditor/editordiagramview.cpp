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

#include "editordiagramview.h"

#include "pxnodecontroller.h"

#include "qmt/diagram_scene/diagramscenemodel.h"
#include "qmt/tasks/diagramscenecontroller.h"
#include "qmt/stereotype/customrelation.h"
#include "qmt/stereotype/stereotypecontroller.h"
#include "qmt/infrastructure/uid.h"
#include "qmt/model/massociation.h"
#include "qmt/model/mcomponent.h"
#include "qmt/model/mdiagram.h"
#include "qmt/model_controller/modelcontroller.h"
#include "qmt/diagram_controller/dselection.h"
#include "qmt/diagram_controller/diagramcontroller.h"
#include "qmt/diagram_scene/items/componentitem.h"
#include <explorer/projectnodes.h>
#include <QWheelEvent>

#include <utils/algorithm.h>
#include <utils/stringutils.h>
#include <emostools/cfvparser.h>
#include <emostools/api_description_parser.h>

using namespace qmt;

namespace ModelEditor {
namespace Internal {

MAssociationEnd::Kind kind(const QString& name)
{
    if(name == "Method")
        return MAssociationEnd::Method;
    else if(name == "Event")
        return MAssociationEnd::Event;
    else if(name == "Field")
        return MAssociationEnd::Field;
    else if(name == "Pin")
        return MAssociationEnd::Pin;
    return MAssociationEnd::Method;
}


class EditorDiagramView::EditorDiagramViewPrivate {
public:
    PxNodeController *pxNodeController = nullptr;
    QList<qmt::DAssociation*> currentDAssociations;
    QList<qmt::MAssociation*> currentMAssociations;
    qmt::DAssociation* addDAssociation;
    qmt::MAssociation* addMAssociation;
    EmosTools::IDLStruct idlA;
    EmosTools::IDLStruct idlB;
};

EditorDiagramView::EditorDiagramView(QWidget *parent)
    : qmt::DiagramView(parent),
      d(new EditorDiagramViewPrivate)
{
}

EditorDiagramView::~EditorDiagramView()
{
    delete d;
}

void EditorDiagramView::showItem(int flag)
{
    qmt::DiagramSceneModel * sceneModel = diagramSceneModel();
    qmt::MDiagram * diagram = sceneModel->diagram();
    QSet<qmt::DElement *> component;//要显示的item

    //item相关筛选显示
    if(flag & 16)
    {
        QSet<QGraphicsItem *> seletcItems = sceneModel->selectedItems();
        if(seletcItems.size() == 0)
            return;

        QSet<qmt::Uid> seletcUid;//记录选中item Uid
        for(QGraphicsItem * item : seletcItems)
        {
            qmt::DElement * elem = sceneModel->element(item);
            if(dynamic_cast<qmt::DComponent*>(elem))
            {
                component << elem;
                seletcUid << elem->uid();
            }
        }

        for(qmt::DElement * elem : diagram->diagramElements())
        {
            if(component.contains(elem))
            {
                continue;
            }

            qmt::DAssociation * ass = dynamic_cast<qmt::DAssociation*>(elem);
            if(!ass)
                continue;

            if(seletcUid.contains(ass->endAUid()))
            {
                component << ass;
                component << diagram->findDiagramElement(ass->endBUid());
            }
            else if(seletcUid.contains(ass->endBUid()))
            {
                component << ass;
                component << diagram->findDiagramElement(ass->endAUid());
            }
        }
    }

    //连线筛选显示
    for(qmt::DElement * elem : diagram->diagramElements())
    {
        if(dynamic_cast<qmt::DBoundary*>(elem) || dynamic_cast<qmt::DDevice*>(elem))
            continue;
        else if(dynamic_cast<qmt::DComponent*>(elem))
        {
            if(flag & 16)
            {
                if(component.contains(elem))
                    sceneModel->graphicsItem(elem)->setVisible(true);
                else
                    sceneModel->graphicsItem(elem)->setVisible(false);
            }
            else
                sceneModel->graphicsItem(elem)->setVisible(true);
        }
        else if(dynamic_cast<qmt::DAssociation*>(elem))
        {
            qmt::DAssociation* association = dynamic_cast<qmt::DAssociation*>(elem);
            bool show = true;
            if((flag & 16) && !component.contains(elem))
                show = false;

            if(association->endA().kind() ==  qmt::MAssociationEnd::Method)
            {
                if(!(flag & 1))
                    show = false;
            }
            else if(association->endA().kind() ==  qmt::MAssociationEnd::Event)
            {
                if(!(flag & 2))
                    show = false;
            }
            else if(association->endA().kind() ==  qmt::MAssociationEnd::Field)
            {
                if(!(flag & 4))
                    show = false;
            }
            else if(association->endA().kind() ==  qmt::MAssociationEnd::Pin)
            {
                if(!(flag & 8))
                    show = false;
            }
            sceneModel->graphicsItem(elem)->setVisible(show);
        }
    }
}

void EditorDiagramView::autoConnect()
{
    qmt::DiagramSceneModel * sceneModel = diagramSceneModel();
    qmt::MDiagram * diagram = sceneModel->diagram();
    qmt::ModelController* modelController = diagramSceneModel()->diagramSceneController()->modelController();

    QList<qmt::DComponent*> components;
    d->currentDAssociations.clear();
    d->currentMAssociations.clear();

    for(qmt::DElement * elem : diagram->diagramElements())
    {
        if(dynamic_cast<qmt::DComponent*>(elem))
            components << dynamic_cast<qmt::DComponent*>(elem);
        if(dynamic_cast<qmt::DAssociation*>(elem))
        {
            d->currentDAssociations << dynamic_cast<qmt::DAssociation*>(elem);
            d->currentMAssociations << modelController->findObject<qmt::MAssociation>(elem->modelUid());
        }
    }

    for(qmt::DComponent * componentB : components)
    {
        EmosTools::IDLStruct idl = EmosTools::IdlParser::getIdlStruct(componentB->oid());
        if(!idl.isValid)
            continue;
        if(idl.quoteService.size() == 0)
            continue;

        QMapIterator<QString, int> service(idl.quoteService);
        while (service.hasNext()) {
            service.next();

            for(qmt::DComponent * componentA : components)
            {
                if(componentA == componentB)
                    continue;

                if(!componentA->serviceInfo().contains(service.key()))
                    continue;

                EmosTools::tSrvService srv = EmosTools::ApiParser::getInstance()->getService(service.key());
                if (srv.id == -1)
                    continue;

                if(!srv.events.isEmpty())
                    connect(componentA, componentB, "Event");
                if(!srv.fields.isEmpty())
                    connect(componentA, componentB, "Field");
                if(!srv.pins.isEmpty())
                    connect(componentA, componentB, "Pin");
                if(!srv.functions.isEmpty())
                    connect(componentA, componentB, "Method");
            }
        }
    }
}

void EditorDiagramView::beautifulConnect()
{
    qmt::DiagramSceneModel * sceneModel = diagramSceneModel();
    qmt::MDiagram * diagram = sceneModel->diagram();
    qmt::DiagramController* diagramController = diagramSceneModel()->diagramSceneController()->diagramController();

    for(qmt::DElement * elem : diagram->diagramElements())
    {
        qmt::DAssociation* ass = dynamic_cast<qmt::DAssociation*>(elem);
        if(ass)
        {
            qmt::DComponent* A = diagramController->findElement<qmt::DComponent>(ass->endAUid(), diagram);
            qmt::DComponent* B = diagramController->findElement<qmt::DComponent>(ass->endBUid(), diagram);

            if(A && B)
            {
                QList<DRelation::IntermediatePoint> points;
                for(auto p : DiagramSceneController::calConnectPoints(A, B, ass->endA().kind()))
                {
                    points << DRelation::IntermediatePoint(p);
                }
                diagramController->startUpdateElement(ass, diagram, DiagramController::UpdateMajor);
                ass->setIntermediatePoints(points);
                diagramController->finishUpdateElement(ass, diagram, false);
            }
        }
    }
}

void EditorDiagramView::disconnect()
{
    qmt::DiagramSceneModel * sceneModel = diagramSceneModel();
    qmt::MDiagram * diagram = sceneModel->diagram();

    DSelection selection;
    for(qmt::DElement * elem : diagram->diagramElements())
    {
        if(dynamic_cast<qmt::DAssociation*>(elem))
        {
            selection.append(elem->uid(), diagram->uid());
        }
    }
    sceneModel->diagramSceneController()->deleteFromDiagram(selection, diagram);
}

void EditorDiagramView::updateAll(const QStringList& oid)
{
    for(QGraphicsItem * item : diagramSceneModel()->graphicsItems())
    {
        ComponentItem* comp = dynamic_cast<ComponentItem*>(item);
        if(comp)
        {
            comp->setExist(oid.contains(comp->object()->oid()));
            comp->update();
        }
    }
    update();
}

void EditorDiagramView::setPxNodeController(PxNodeController *pxNodeController)
{
    d->pxNodeController = pxNodeController;
}

void EditorDiagramView::wheelEvent(QWheelEvent *wheelEvent)
{
    if (wheelEvent->modifiers() == Qt::ControlModifier) {
        int degree = wheelEvent->angleDelta().y() / 8;
#if QT_VERSION < QT_VERSION_CHECK(5, 14, 0)
        QPoint zoomOrigin = wheelEvent->pos();
#else
        QPoint zoomOrigin = wheelEvent->position().toPoint();
#endif
        if (degree > 0)
            emit zoomIn(zoomOrigin);
        else if (degree < 0)
            emit zoomOut(zoomOrigin);
    }
}

void EditorDiagramView::connect(qmt::DComponent* A, qmt::DComponent* B, const QString &type)
{
    for(qmt::DAssociation* ass : d->currentDAssociations)
    {
        //相同的连线不能连多次
        if(ass->endAUid() == A->uid() && ass->endBUid() == B->uid() && ass->endA().kindName() == type)
            return;
        //相同oid,不同uid的线不能连多次
        if(ass->endAOid() == A->oid() && ass->endBOid() == B->oid() && ass->endA().kindName() == type &&
             ((ass->endAUid() != A->uid() && ass->endBUid() == B->uid()) || (ass->endAUid() == A->uid() && ass->endBUid() != B->uid())))
            return;
    } 

    if(!allowAssociation(A, B, type))
        return;

    addAssociation(A, B, type);

    setAssociation(A, B);
}

bool EditorDiagramView::allowAssociation(qmt::DComponent *A, qmt::DComponent *B, const QString &type)
{
    qmt::ModelController* modelController = diagramSceneModel()->diagramSceneController()->modelController();
    qmt::MComponent* mB = modelController->findObject<qmt::MComponent>(B->modelUid());
    qmt::MComponent* mA = modelController->findObject<qmt::MComponent>(A->modelUid());
    if(!mB || !mA)
        return false;

    d->idlA = EmosTools::IdlParser::getIdlStruct(A->oid());
    d->idlB = EmosTools::IdlParser::getIdlStruct(B->oid());
    if(!d->idlA.isValid || !d->idlB.isValid)
        return false;

    bool canConnect = false;
    if(type == "Method")
    {
        canConnect = true;
    }
    else if(type == "Event")
    {
        for(auto runnable : d->idlB.thread)
        {
            if(runnable.type != "Event")
                continue;

            for(const QString& srvName : d->idlB.quoteService.keys())
            {
                if(!d->idlA.service.contains(srvName))//A没有实现此服务
                    continue;

                EmosTools::tSrvService srv = EmosTools::ApiParser::getInstance()->getService(srvName);
                if (srv.id == -1)
                {
                    continue;
                }

                for(const EmosTools::tSrvEvent& event : srv.events)
                {
                    if(runnable.data.trimmed() != event.data.trimmed())//数据类型一直才能被选择
                        continue;

                    if(hasRunableConnected(mB, kind(type), event.name))//查看是否有使用过的runnable
                        continue;

                    canConnect = true;
                }
            }
        }
    }
    else if(type == "Field")
    {
        for(auto runnable : d->idlB.thread)
        {
            if(runnable.type != "Field")
                continue;

            for(const QString& srvName : d->idlB.quoteService.keys())
            {
                if(!d->idlA.service.contains(srvName))//A没有实现此服务
                    continue;

                EmosTools::tSrvService srv = EmosTools::ApiParser::getInstance()->getService(srvName);
                if (srv.id == -1)
                {
                    continue;
                }

                for(const EmosTools::tSrvField& field : srv.fields)
                {
                    if(runnable.data.trimmed() != field.data.trimmed())//数据类型一直才能被选择
                        continue;

                    if(hasRunableConnected(mB, kind(type), field.name))//查看是否有使用过的runnable
                        continue;

                    canConnect = true;
                }
            }
        }
    }
    else if(type == "Pin")
    {
        for(auto runnable : d->idlB.thread)
        {
            if(runnable.type != "Data")
                continue;

            for(const QString& srvName : d->idlB.quoteService.keys())
            {
                if(!d->idlA.service.contains(srvName))//A没有实现此服务
                    continue;

                EmosTools::tSrvService srv = EmosTools::ApiParser::getInstance()->getService(srvName);
                if (srv.id == -1)
                {
                    continue;
                }

                for(const EmosTools::tSrvPin& pin : srv.pins)
                {
                    if(runnable.data.trimmed() != pin.data.trimmed())//数据类型一直才能被选择
                        continue;

                    if(hasRunableConnected(mB, kind(type), pin.name))//查看是否有使用过的runnable
                        continue;

                    canConnect = true;
                }
            }
        }
    }

    return canConnect;
}

void EditorDiagramView::addAssociation(qmt::DComponent *A, qmt::DComponent *B, const QString &type)
{
    QList<QPointF> intermediatePoints = DiagramSceneController::calConnectPoints(A, B, kind(type));

    DiagramSceneController *diagramSceneController = diagramSceneModel()->diagramSceneController();

    StereotypeController *stereotypeController = diagramSceneModel()->stereotypeController();
    CustomRelation customRelation = stereotypeController->findCustomRelation(type);

    auto assoziatedClass = dynamic_cast<DComponent *>(B);
    auto derivedClass = dynamic_cast<DComponent *>(A);
    diagramSceneController->createComAssociation(derivedClass, assoziatedClass, intermediatePoints, diagramSceneModel()->diagram(),
                [=] (MAssociation *mAssociation, DAssociation *dAssociation) {
        if (mAssociation && dAssociation) {
            static const QHash<CustomRelation::Relationship, MAssociationEnd::Kind> relationship2KindMap = {
                { CustomRelation::Relationship::Association, MAssociationEnd::Association },
                { CustomRelation::Relationship::Aggregation, MAssociationEnd::Aggregation },
                { CustomRelation::Relationship::Composition, MAssociationEnd::Composition },
                { CustomRelation::Relationship::Method, MAssociationEnd::Method },
                { CustomRelation::Relationship::Event, MAssociationEnd::Event },
                { CustomRelation::Relationship::Field, MAssociationEnd::Field },
                { CustomRelation::Relationship::Pin, MAssociationEnd::Pin }};
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

            d->currentDAssociations << dAssociation;
            d->currentMAssociations << mAssociation;
            d->addDAssociation = dAssociation;
            d->addMAssociation = mAssociation;
        }
    });
}

void EditorDiagramView::setAssociation(qmt::DComponent *A, qmt::DComponent *B)
{
    qmt::ModelController* modelController = diagramSceneModel()->diagramSceneController()->modelController();
    qmt::MComponent* mB = modelController->findObject<qmt::MComponent>(B->modelUid());
    qmt::MComponent* mA = modelController->findObject<qmt::MComponent>(A->modelUid());
    if(!mB || !mA)
        return;

    if(d->addDAssociation->endA().kind() == qmt::MAssociationEnd::Event)
    {
        QHash<QString,QString> eventMap = d->addDAssociation->getEventMap();
        for(auto runnable : d->idlB.thread)
        {
            if(runnable.type != "Event")
                continue;

            for(const QString& srvName : d->idlB.quoteService.keys())
            {
                if(!d->idlA.service.contains(srvName))//A没有实现此服务
                    continue;

                EmosTools::tSrvService srv = EmosTools::ApiParser::getInstance()->getService(srvName);
                if (srv.id == -1)
                {
                    continue;
                }

                for(const EmosTools::tSrvEvent& event : srv.events)
                {
                    if(runnable.data.trimmed() != event.data.trimmed())//数据类型一直才能被选择
                        continue;

                    if(hasRunableConnected(mB, d->addDAssociation->endA().kind(), event.name))//查看是否有使用过的runnable
                        continue;

                    eventMap.insert(runnable.name, srvName + " : " + event.name);
                    d->addDAssociation->setEventMap(eventMap);
                    d->addMAssociation->setEventMap(eventMap);
                }
            }
        }
    }
    else if(d->addDAssociation->endA().kind() == qmt::MAssociationEnd::Field)
    {
        QHash<QString,QString> fieldMap = d->addDAssociation->getFieldMap();
        for(auto runnable : d->idlB.thread)
        {
            if(runnable.type != "Field")
                continue;

            for(const QString& srvName : d->idlB.quoteService.keys())
            {
                if(!d->idlA.service.contains(srvName))//A没有实现此服务
                    continue;

                EmosTools::tSrvService srv = EmosTools::ApiParser::getInstance()->getService(srvName);
                if (srv.id == -1)
                {
                    continue;
                }

                for(const EmosTools::tSrvField& field : srv.fields)
                {
                    if(runnable.data.trimmed() != field.data.trimmed())//数据类型一直才能被选择
                        continue;

                    if(hasRunableConnected(mB, d->addDAssociation->endA().kind(), field.name))//查看是否有使用过的runnable
                        continue;

                    fieldMap.insert(runnable.name, srvName + " : " + field.name);
                    d->addDAssociation->setFieldMap(fieldMap);
                    d->addMAssociation->setFieldMap(fieldMap);
                }
            }
        }
    }
    else if(d->addDAssociation->endA().kind() == qmt::MAssociationEnd::Pin)
    {
        QHash<QString,QString> pinMap = d->addDAssociation->getPinMap();
        for(auto runnable : d->idlB.thread)
        {
            if(runnable.type != "Data")
                continue;

            for(const QString& srvName : d->idlB.quoteService.keys())
            {
                if(!d->idlA.service.contains(srvName))//A没有实现此服务
                    continue;

                EmosTools::tSrvService srv = EmosTools::ApiParser::getInstance()->getService(srvName);
                if (srv.id == -1)
                {
                    continue;
                }

                for(const EmosTools::tSrvPin& pin : srv.pins)
                {
                    if(runnable.data.trimmed() != pin.data.trimmed())//数据类型一直才能被选择
                        continue;

                    if(hasRunableConnected(mB, d->addDAssociation->endA().kind(), pin.name))//查看是否有使用过的runnable
                        continue;

                    pinMap.insert(runnable.name, srvName + " : " + pin.name);
                    d->addDAssociation->setPinMap(pinMap);
                    d->addMAssociation->setPinMap(pinMap);

                    int port = Utils::makeUniquelyId(diagramSceneModel()->diagramSceneController()->getAllPort(), 1, COMPONENT_START_PORT);
                    A->addPinInfo({ pin.name, QString::number(port)});
                    mA->addPinInfo({ pin.name, QString::number(port)});
                }
            }
        }
    }
}

bool EditorDiagramView::hasRunableConnected(const qmt::MObject * elementB, qmt::MAssociationEnd::Kind kind, const QString& runableName)
{
    QList<MRelation *> relationList = diagramSceneModel()->diagramSceneController()->modelController()->findRelationsOfObject(elementB);
    for(MRelation * relation : relationList)
    {
        MAssociation * ass = dynamic_cast<MAssociation *>(relation);
        if(ass->endB().kind() == kind)
        {
            QHash<QString,QString> eventMap = ass->getEventMap();
            if(!eventMap.value(runableName).isEmpty())
                return true;
            QHash<QString,QString> fieldMap = ass->getFieldMap();
            if(!fieldMap.value(runableName).isEmpty())
                return true;
            QHash<QString,QString> pinMap = ass->getPinMap();
            if(!pinMap.value(runableName).isEmpty())
                return true;
        }
    }

    return false;
}

void EditorDiagramView::dropProjectExplorerNodes(const QList<QVariant> &values, const QPoint &pos)
{
    for (const auto &value : values) {
        if (value.canConvert<ProjectExplorer::Node *>()) {
            auto node = value.value<ProjectExplorer::Node *>();
            QPointF scenePos = mapToScene(pos);
            auto folderNode = dynamic_cast<ProjectExplorer::FolderNode *>(node);
            if (folderNode) {
                d->pxNodeController->addFileSystemEntry(
                            folderNode->filePath().toString(), -1, -1,
                            diagramSceneModel()->findTopmostElement(scenePos),
                            scenePos, diagramSceneModel()->diagram());
            }
        }
    }
}

void EditorDiagramView::dropFiles(const QList<Utils::DropSupport::FileSpec> &files, const QPoint &pos)
{
    for (const auto &fileSpec : files) {
        QPointF scenePos = mapToScene(pos);
        d->pxNodeController->addFileSystemEntry(
                    fileSpec.filePath, fileSpec.line, fileSpec.column,
                    diagramSceneModel()->findTopmostElement(scenePos),
                    scenePos, diagramSceneModel()->diagram());
    }
}

} // namespace Internal
} // namespace ModelEditor
