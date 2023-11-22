/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
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

#include "qmakenodetreebuilder.h"

#include "qmakeproject.h"
#include "qmakeprojectmanagerconstants.h"

#include <coreplugin/fileiconprovider.h>
#include <explorer/projectexplorerconstants.h>
#include <explorer/target.h>

#include <utils/algorithm.h>
#include <utils/qtcassert.h>

#include <QMap>

using namespace Core;
using namespace ProjectExplorer;
using namespace Utils;

using namespace QmakeProjectManager::Internal;

namespace {

// Static cached data in struct QmakeStaticData providing information and icons
// for file types and the project. Do some magic via qAddPostRoutine()
// to make sure the icons do not outlive QApplication, triggering warnings on X11.

class FileTypeDataStorage {
public:
    FileType type;
    const char *typeName;
    const char *icon;
    const char *addFileFilter;
};

const FileTypeDataStorage fileTypeDataStorage[] = {
    // { FileType::Header, QT_TRANSLATE_NOOP("EmosProjectManager:EmosiFile", "Headers"),
    //   ProjectExplorer::Constants::FILE_IDL, "*.idl;"},
    { FileType::Source, QT_TRANSLATE_NOOP("EmosProjectManager:EmosiFile", "Sources"),
      ProjectExplorer::Constants::FILE_CFV, "*.idl;*.cfv;*.description;" },
    { FileType::Unknown, QT_TRANSLATE_NOOP("EmosProjectManager:EmosiFile", "Other files"),
      ProjectExplorer::Constants::FILEOVERLAY_UNKNOWN, "*;" }
};

class QmakeStaticData {
public:
    class FileTypeData {
    public:
        FileTypeData(FileType t = FileType::Unknown,
                     const QString &tN = QString(),
                     const QString &aff = QString(),
                     const QIcon &i = QIcon()) :
        type(t), typeName(tN), addFileFilter(aff), icon(i) { }

        FileType type;
        QString typeName;
        QString addFileFilter;
        QIcon icon;
    };

    QmakeStaticData();

    QVector<FileTypeData> fileTypeData;
    QIcon productIcon;
    QIcon interfaceIcon;
    QIcon componentIcon;
    QIcon architectureIcon;
    QIcon configIcon;
    QIcon scheduleIcon;

};

void clearQmakeStaticData();

QmakeStaticData::QmakeStaticData()
{
    // File type data
    const unsigned count = sizeof(fileTypeDataStorage)/sizeof(FileTypeDataStorage);
    fileTypeData.reserve(count);

    for (const FileTypeDataStorage &fileType : fileTypeDataStorage) {
        const QString desc = QCoreApplication::translate("EmosProjectManager:EmosiFile", fileType.typeName);
        const QString filter = QString::fromUtf8(fileType.addFileFilter);
        fileTypeData.push_back(QmakeStaticData::FileTypeData(fileType.type,
                                                             desc, filter,
                                                             Core::FileIconProvider::directoryIcon(QLatin1String(fileType.icon))));
    }
    // Project icon
    productIcon = Core::FileIconProvider::directoryIcon(ProjectExplorer::Constants::FILEOVERLAY_PRODUCT);
    interfaceIcon = Core::FileIconProvider::directoryIcon(ProjectExplorer::Constants::FOLDER_INERFACE_CLOSE);
    componentIcon = Core::FileIconProvider::directoryIcon(ProjectExplorer::Constants::FOLDER_COMPONENT_CLOSE);
    architectureIcon = Core::FileIconProvider::directoryIcon(ProjectExplorer::Constants::FOLDER_ARCHITECTURE_CLOSE);
    configIcon = Core::FileIconProvider::directoryIcon(ProjectExplorer::Constants::FOLDER_CONFIG_CLOSE);
    scheduleIcon = Core::FileIconProvider::directoryIcon(ProjectExplorer::Constants::FOLDER_SCHEDULE_CLOSE);

    qAddPostRoutine(clearQmakeStaticData);
}

Q_GLOBAL_STATIC(QmakeStaticData, qmakeStaticData)

void clearQmakeStaticData()
{
    qmakeStaticData()->fileTypeData.clear();
    qmakeStaticData()->productIcon = QIcon();
    qmakeStaticData()->interfaceIcon = QIcon();
    qmakeStaticData()->componentIcon = QIcon();
    qmakeStaticData()->architectureIcon = QIcon();
    qmakeStaticData()->configIcon = QIcon();
    qmakeStaticData()->scheduleIcon = QIcon();

}

} // namespace

namespace QmakeProjectManager {

static QMap<QString, QIcon> dirIconMap {
    {Constants::InterfaceDesign, qmakeStaticData->interfaceIcon},
    {Constants::ComponentDesign, qmakeStaticData->componentIcon},
    {Constants::ArchitectureDesign, qmakeStaticData->architectureIcon},
    {Constants::Config, qmakeStaticData->configIcon},
    {Constants::ScheduleConfiguration, qmakeStaticData->scheduleIcon},

};

static void createTree(QmakeBuildSystem *buildSystem,
                       const QmakePriFile *pri,
                       QmakePriFileNode *node,
                       const FilePaths &toExclude)
{
    QTC_ASSERT(pri, return);
    QTC_ASSERT(node, return);

    node->setDisplayName(pri->displayName());

    // .pro/.pri-file itself:
    //node->addNode(std::make_unique<FileNode>(pri->filePath(), FileType::Project)); //haojie.fang不显示项目文件

    // other normal files:
    const QVector<QmakeStaticData::FileTypeData> &fileTypes = qmakeStaticData()->fileTypeData;
    for (int i = 0; i < fileTypes.size(); ++i) {
        FileType type = fileTypes.at(i).type;
        const SourceFiles &newFilePaths = Utils::filtered(pri->files(type), [&toExclude](const SourceFile &fn) {
            return !Utils::contains(toExclude, [&fn](const Utils::FilePath &ex) { return fn.first.isChildOf(ex); });
        });

        if (!newFilePaths.isEmpty()) {
            for (const auto &fn : newFilePaths) {
                if(fn.first.endsWith("emosi"))
                    continue;

                type = FileNode::fileTypeForFileName(fn.first);
                auto fileNode = std::make_unique<FileNode>(fn.first, type);
                fileNode->setEnabled(fn.second == FileOrigin::ExactParse);
                node->addNode(std::move(fileNode));
            }
        }
    }

    for (QmakePriFile *c : pri->children()) {
        std::unique_ptr<QmakePriFileNode> newNode;
        if (auto pf = dynamic_cast<QmakeProFile *>(c)) {
            newNode = std::make_unique<QmakeProFileNode>(c->buildSystem(), c->filePath(), pf);
            newNode->setIcon(qmakeStaticData()->productIcon);
        } else {
            newNode = std::make_unique<QmakePriFileNode>(c->buildSystem(), node->proFileNode(),
                                                         c->filePath(), c);

            if(dirIconMap.contains(c->displayName()))
                newNode->setIcon(dirIconMap.value(c->displayName()));
            else
                newNode->setIcon(qmakeStaticData->configIcon);
        }
        createTree(buildSystem, c, newNode.get(), toExclude);
        node->addNode(std::move(newNode));
    }
}

std::unique_ptr<QmakeProFileNode> QmakeNodeTreeBuilder::buildTree(QmakeBuildSystem *buildSystem)
{
    // Remove qmake implementation details that litter up the project data:
    auto root = std::make_unique<QmakeProFileNode>(buildSystem,
                                                   buildSystem->projectFilePath(),
                                                   buildSystem->rootProFile());
    root->setIcon(qmakeStaticData()->productIcon);
    createTree(buildSystem, buildSystem->rootProFile(), root.get(), FilePaths());

    return root;
}

} // namespace QmakeProjectManager
