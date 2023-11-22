
HEADERS += \
    $$PWD/config/configcontroller.h \
    $$PWD/config/sourcepos.h \
    $$PWD/config/stereotypedefinitionparser.h \
    $$PWD/config/stringtextsource.h \
    $$PWD/config/textscanner.h \
    $$PWD/config/textsource.h \
    $$PWD/config/token.h \
    $$PWD/controller/container.h \
    $$PWD/controller/namecontroller.h \
    $$PWD/controller/references.h \
    $$PWD/controller/selection.h \
    $$PWD/controller/undocommand.h \
    $$PWD/controller/undocontroller.h \
    $$PWD/diagram_controller/dclonevisitor.h \
    $$PWD/diagram_controller/dcontainer.h \
    $$PWD/diagram_controller/dfactory.h \
    $$PWD/diagram_controller/dflatassignmentvisitor.h \
    $$PWD/diagram_controller/diagramcontroller.h \
    $$PWD/diagram_controller/dreferences.h \
    $$PWD/diagram_controller/dselection.h \
    $$PWD/diagram_controller/dupdatevisitor.h \
    $$PWD/diagram_controller/dvoidvisitor.h \
    $$PWD/diagram/dannotation.h \
    $$PWD/diagram/dassociation.h \
    $$PWD/diagram/dboundary.h \
    $$PWD/diagram/ddevice.h \
    $$PWD/diagram/dclass.h \
    $$PWD/diagram/dcomponent.h \
    $$PWD/diagram/dconnection.h \
    $$PWD/diagram/dconstvisitor.h \
    $$PWD/diagram/ddependency.h \
    $$PWD/diagram/ddiagram.h \
    $$PWD/diagram/delement.h \
    $$PWD/diagram/dinheritance.h \
    $$PWD/diagram/ditem.h \
    $$PWD/diagram/dobject.h \
    $$PWD/diagram/dpackage.h \
    $$PWD/diagram/drelation.h \
    $$PWD/diagram/dvisitor.h \
    $$PWD/diagram_scene/capabilities/alignable.h \
    $$PWD/diagram_scene/capabilities/editable.h \
    $$PWD/diagram_scene/capabilities/intersectionable.h \
    $$PWD/diagram_scene/capabilities/latchable.h \
    $$PWD/diagram_scene/capabilities/moveable.h \
    $$PWD/diagram_scene/capabilities/relationable.h \
    $$PWD/diagram_scene/capabilities/resizable.h \
    $$PWD/diagram_scene/capabilities/selectable.h \
    $$PWD/diagram_scene/capabilities/windable.h \
    $$PWD/diagram_scene/diagramgraphicsscene.h \
    $$PWD/diagram_scene/diagramsceneconstants.h \
    $$PWD/diagram_scene/diagramscenemodel.h \
    $$PWD/diagram_scene/diagramscenemodelitemvisitors.h \
    $$PWD/diagram_scene/items/annotationitem.h \
    $$PWD/diagram_scene/items/associationitem.h \
    $$PWD/diagram_scene/items/boundaryitem.h \
    $$PWD/diagram_scene/items/deviceitem.h \
    $$PWD/diagram_scene/items/classitem.h \
    $$PWD/diagram_scene/items/componentitem.h \
    $$PWD/diagram_scene/items/connectionitem.h \
    $$PWD/diagram_scene/items/diagramitem.h \
    $$PWD/diagram_scene/items/itemitem.h \
    $$PWD/diagram_scene/items/objectitem.h \
    $$PWD/diagram_scene/items/packageitem.h \
    $$PWD/diagram_scene/items/relationitem.h \
    $$PWD/diagram_scene/items/stereotypedisplayvisitor.h \
    $$PWD/diagram_scene/latchcontroller.h \
    $$PWD/diagram_scene/parts/alignbuttonsitem.h \
    $$PWD/diagram_scene/parts/alignlineitem.h \
    $$PWD/diagram_scene/parts/arrowitem.h \
    $$PWD/diagram_scene/parts/contextlabelitem.h \
    $$PWD/diagram_scene/parts/customiconitem.h \
    $$PWD/diagram_scene/parts/editabletextitem.h \
    $$PWD/diagram_scene/parts/pathselectionitem.h \
    $$PWD/diagram_scene/parts/rectangularselectionitem.h \
    $$PWD/diagram_scene/parts/relationstarter.h \
    $$PWD/diagram_scene/parts/stereotypesitem.h \
    $$PWD/diagram_scene/parts/templateparameterbox.h \
    $$PWD/diagram_ui/diagram_mime_types.h \
    $$PWD/diagram_ui/diagramsmanager.h \
    $$PWD/diagram_ui/diagramsviewinterface.h \
    $$PWD/diagram_ui/sceneinspector.h \
    $$PWD/diagram_widgets_ui/diagramsview.h \
    $$PWD/diagram_widgets_ui/diagramview.h \
    $$PWD/diagram_widgets_ui/stackeddiagramsview.h \
    $$PWD/document_controller/documentcontroller.h \
    $$PWD/infrastructure/contextmenuaction.h \
    $$PWD/infrastructure/exceptions.h \
    $$PWD/infrastructure/geometryutilities.h \
    $$PWD/infrastructure/handle.h \
    $$PWD/infrastructure/handles.h \
    $$PWD/infrastructure/ioexceptions.h \
    $$PWD/infrastructure/qcompressedfile.h \
    $$PWD/infrastructure/qmt_global.h \
    $$PWD/infrastructure/uid.h \
    $$PWD/model/mcomponentmember.h \
    $$PWD/model_controller/mchildrenvisitor.h \
    $$PWD/model_controller/mclonevisitor.h \
    $$PWD/model_controller/mcontainer.h \
    $$PWD/model_controller/mflatassignmentvisitor.h \
    $$PWD/model_controller/modelcontroller.h \
    $$PWD/model_controller/mreferences.h \
    $$PWD/model_controller/mselection.h \
    $$PWD/model_controller/mvoidvisitor.h \
    $$PWD/model/massociation.h \
    $$PWD/model/mcanvasdiagram.h \
    $$PWD/model/mclass.h \
    $$PWD/model/mclassmember.h \
    $$PWD/model/mcomponent.h \
    $$PWD/model/mconnection.h \
    $$PWD/model/mconstvisitor.h \
    $$PWD/model/mdependency.h \
    $$PWD/model/mdiagram.h \
    $$PWD/model/melement.h \
    $$PWD/model/minheritance.h \
    $$PWD/model/mitem.h \
    $$PWD/model/mobject.h \
    $$PWD/model/mpackage.h \
    $$PWD/model/mrelation.h \
    $$PWD/model/msourceexpansion.h \
    $$PWD/model/mvisitor.h \
    $$PWD/model_ui/modeltreeviewinterface.h \
    $$PWD/model_ui/sortedtreemodel.h \
    $$PWD/model_ui/stereotypescontroller.h \
    $$PWD/model_ui/treemodel.h \
    $$PWD/model_ui/treemodelmanager.h \
    $$PWD/model_widgets_ui/classmembersedit.h \
    $$PWD/model_widgets_ui/modeltreeview.h \
    $$PWD/model_widgets_ui/palettebox.h \
    $$PWD/model_widgets_ui/processstaticwidget.h \
    $$PWD/model_widgets_ui/propertiesview.h \
    $$PWD/model_widgets_ui/propertiesviewmview.h \
    $$PWD/model_widgets_ui/basesrvwidget.h \
    $$PWD/project_controller/projectcontroller.h \
    $$PWD/project/project.h \
    $$PWD/serializer/diagramserializer.h \
    $$PWD/serializer/infrastructureserializer.h \
    $$PWD/serializer/modelserializer.h \
    $$PWD/serializer/projectserializer.h \
    $$PWD/stereotype/customrelation.h \
    $$PWD/stereotype/iconshape.h \
    $$PWD/stereotype/shape.h \
    $$PWD/stereotype/shapepaintvisitor.h \
    $$PWD/stereotype/shapes.h \
    $$PWD/stereotype/shapevalue.h \
    $$PWD/stereotype/shapevisitor.h \
    $$PWD/stereotype/stereotypecontroller.h \
    $$PWD/stereotype/stereotypeicon.h \
    $$PWD/stereotype/toolbar.h \
    $$PWD/style/defaultstyleengine.h \
    $$PWD/style/defaultstyle.h \
    $$PWD/style/eventstarterstyle.h \
    $$PWD/style/fieldstarterstyle.h \
    $$PWD/style/methodstarterstyle.h \
    $$PWD/style/pinstarterstyle.h \
    $$PWD/style/objectvisuals.h \
    $$PWD/style/relationstarterstyle.h \
    $$PWD/style/stylecontroller.h \
    $$PWD/style/styledobject.h \
    $$PWD/style/styledrelation.h \
    $$PWD/style/styleengine.h \
    $$PWD/style/style.h \
    $$PWD/tasks/alignonrastervisitor.h \
    $$PWD/tasks/diagramscenecontroller.h \
    $$PWD/tasks/finddiagramvisitor.h \
    $$PWD/tasks/findrootdiagramvisitor.h \
    $$PWD/tasks/ielementtasks.h \
    $$PWD/tasks/isceneinspector.h \
    $$PWD/tasks/voidelementtasks.h \
    $$PWD/infrastructure/qmtassert.h \
    $$PWD/diagram_scene/items/swimlaneitem.h \
    $$PWD/diagram/dswimlane.h

SOURCES += \
    $$PWD/config/configcontroller.cpp \
    $$PWD/config/sourcepos.cpp \
    $$PWD/config/stereotypedefinitionparser.cpp \
    $$PWD/config/stringtextsource.cpp \
    $$PWD/config/textscanner.cpp \
    $$PWD/config/token.cpp \
    $$PWD/controller/namecontroller.cpp \
    $$PWD/controller/selection.cpp \
    $$PWD/controller/undocommand.cpp \
    $$PWD/controller/undocontroller.cpp \
    $$PWD/diagram_controller/dclonevisitor.cpp \
    $$PWD/diagram_controller/dfactory.cpp \
    $$PWD/diagram_controller/dflatassignmentvisitor.cpp \
    $$PWD/diagram_controller/diagramcontroller.cpp \
    $$PWD/diagram_controller/dupdatevisitor.cpp \
    $$PWD/diagram_controller/dvoidvisitor.cpp \
    $$PWD/diagram/dannotation.cpp \
    $$PWD/diagram/dassociation.cpp \
    $$PWD/diagram/dboundary.cpp \
    $$PWD/diagram/ddevice.cpp \
    $$PWD/diagram/dclass.cpp \
    $$PWD/diagram/dcomponent.cpp \
    $$PWD/diagram/dconnection.cpp \
    $$PWD/diagram/ddependency.cpp \
    $$PWD/diagram/ddiagram.cpp \
    $$PWD/diagram/delement.cpp \
    $$PWD/diagram/dinheritance.cpp \
    $$PWD/diagram/ditem.cpp \
    $$PWD/diagram/dobject.cpp \
    $$PWD/diagram/dpackage.cpp \
    $$PWD/diagram/drelation.cpp \
    $$PWD/diagram_scene/diagramgraphicsscene.cpp \
    $$PWD/diagram_scene/diagramscenemodel.cpp \
    $$PWD/diagram_scene/diagramscenemodelitemvisitors.cpp \
    $$PWD/diagram_scene/items/annotationitem.cpp \
    $$PWD/diagram_scene/items/associationitem.cpp \
    $$PWD/diagram_scene/items/boundaryitem.cpp \
    $$PWD/diagram_scene/items/deviceitem.cpp \
    $$PWD/diagram_scene/items/classitem.cpp \
    $$PWD/diagram_scene/items/componentitem.cpp \
    $$PWD/diagram_scene/items/connectionitem.cpp \
    $$PWD/diagram_scene/items/diagramitem.cpp \
    $$PWD/diagram_scene/items/itemitem.cpp \
    $$PWD/diagram_scene/items/objectitem.cpp \
    $$PWD/diagram_scene/items/packageitem.cpp \
    $$PWD/diagram_scene/items/relationitem.cpp \
    $$PWD/diagram_scene/items/stereotypedisplayvisitor.cpp \
    $$PWD/diagram_scene/latchcontroller.cpp \
    $$PWD/diagram_scene/parts/alignbuttonsitem.cpp \
    $$PWD/diagram_scene/parts/alignlineitem.cpp \
    $$PWD/diagram_scene/parts/arrowitem.cpp \
    $$PWD/diagram_scene/parts/contextlabelitem.cpp \
    $$PWD/diagram_scene/parts/customiconitem.cpp \
    $$PWD/diagram_scene/parts/editabletextitem.cpp \
    $$PWD/diagram_scene/parts/pathselectionitem.cpp \
    $$PWD/diagram_scene/parts/rectangularselectionitem.cpp \
    $$PWD/diagram_scene/parts/relationstarter.cpp \
    $$PWD/diagram_scene/parts/stereotypesitem.cpp \
    $$PWD/diagram_scene/parts/templateparameterbox.cpp \
    $$PWD/diagram_ui/diagramsmanager.cpp \
    $$PWD/diagram_ui/sceneinspector.cpp \
    $$PWD/diagram_widgets_ui/diagramsview.cpp \
    $$PWD/diagram_widgets_ui/diagramview.cpp \
    $$PWD/diagram_widgets_ui/stackeddiagramsview.cpp \
    $$PWD/document_controller/documentcontroller.cpp \
    $$PWD/infrastructure/contextmenuaction.cpp \
    $$PWD/infrastructure/exceptions.cpp \
    $$PWD/infrastructure/geometryutilities.cpp \
    $$PWD/infrastructure/ioexceptions.cpp \
    $$PWD/infrastructure/qcompressedfile.cpp \
    $$PWD/model/mcomponentmember.cpp \
    $$PWD/model_controller/mchildrenvisitor.cpp \
    $$PWD/model_controller/mclonevisitor.cpp \
    $$PWD/model_controller/mflatassignmentvisitor.cpp \
    $$PWD/model_controller/modelcontroller.cpp \
    $$PWD/model_controller/mvoidvisitor.cpp \
    $$PWD/model/massociation.cpp \
    $$PWD/model/mcanvasdiagram.cpp \
    $$PWD/model/mclass.cpp \
    $$PWD/model/mclassmember.cpp \
    $$PWD/model/mcomponent.cpp \
    $$PWD/model/mconnection.cpp \
    $$PWD/model/mdependency.cpp \
    $$PWD/model/mdiagram.cpp \
    $$PWD/model/melement.cpp \
    $$PWD/model/minheritance.cpp \
    $$PWD/model/mitem.cpp \
    $$PWD/model/mobject.cpp \
    $$PWD/model/mpackage.cpp \
    $$PWD/model/mrelation.cpp \
    $$PWD/model/msourceexpansion.cpp \
    $$PWD/model_ui/sortedtreemodel.cpp \
    $$PWD/model_ui/stereotypescontroller.cpp \
    $$PWD/model_ui/treemodel.cpp \
    $$PWD/model_ui/treemodelmanager.cpp \
    $$PWD/model_widgets_ui/classmembersedit.cpp \
    $$PWD/model_widgets_ui/modeltreeview.cpp \
    $$PWD/model_widgets_ui/palettebox.cpp \
    $$PWD/model_widgets_ui/processstaticwidget.cpp \
    $$PWD/model_widgets_ui/propertiesview.cpp \
    $$PWD/model_widgets_ui/propertiesviewmview.cpp \
    $$PWD/model_widgets_ui/basesrvwidget.cpp \
    $$PWD/project_controller/projectcontroller.cpp \
    $$PWD/project/project.cpp \
    $$PWD/serializer/diagramserializer.cpp \
    $$PWD/serializer/infrastructureserializer.cpp \
    $$PWD/serializer/modelserializer.cpp \
    $$PWD/serializer/projectserializer.cpp \
    $$PWD/stereotype/customrelation.cpp \
    $$PWD/stereotype/iconshape.cpp \
    $$PWD/stereotype/shapepaintvisitor.cpp \
    $$PWD/stereotype/shapes.cpp \
    $$PWD/stereotype/shapevalue.cpp \
    $$PWD/stereotype/stereotypecontroller.cpp \
    $$PWD/stereotype/stereotypeicon.cpp \
    $$PWD/stereotype/toolbar.cpp \
    $$PWD/style/defaultstyle.cpp \
    $$PWD/style/defaultstyleengine.cpp \
    $$PWD/style/eventstarterstyle.cpp \
    $$PWD/style/fieldstarterstyle.cpp \
    $$PWD/style/methodstarterstyle.cpp \
    $$PWD/style/pinstarterstyle.cpp \
    $$PWD/style/objectvisuals.cpp \
    $$PWD/style/relationstarterstyle.cpp \
    $$PWD/style/stylecontroller.cpp \
    $$PWD/style/style.cpp \
    $$PWD/style/styledobject.cpp \
    $$PWD/style/styledrelation.cpp \
    $$PWD/tasks/alignonrastervisitor.cpp \
    $$PWD/tasks/diagramscenecontroller.cpp \
    $$PWD/tasks/finddiagramvisitor.cpp \
    $$PWD/tasks/findrootdiagramvisitor.cpp \
    $$PWD/tasks/voidelementtasks.cpp \
    $$PWD/diagram_scene/items/swimlaneitem.cpp \
    $$PWD/diagram/dswimlane.cpp

RESOURCES += \
    $$PWD/resources/resources.qrc
