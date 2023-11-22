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

#include "qmakeparser.h"

#include <projectexplorer/task.h>
#include <projectexplorer/projectexplorerconstants.h>
#include <projectexplorer/buildmanager.h>

using namespace ProjectExplorer;
using namespace Utils;

namespace QmakeProjectManager {

QMakeParser::QMakeParser() : m_error(QLatin1String("^(.+?):(\\d+?):\\s(.+?)$"))
{
    setObjectName(QLatin1String("QMakeParser"));
}

OutputLineParser::Result QMakeParser::handleLine(const QString &line, OutputFormat type)
{
    if (type != Utils::StdErrFormat)
        return Status::NotHandled;
    QString lne = rightTrimmed(line);
    QRegularExpressionMatch match = m_error.match(lne);
    if (match.hasMatch()) {
        QString fileName = match.captured(1);
        Task::TaskType type = Task::Error;
        const QString description = match.captured(3);
        int fileNameOffset = match.capturedStart(1);
        if (fileName.startsWith(QLatin1String("WARNING: "))) {
            type = Task::Warning;
            fileName = fileName.mid(9);
            fileNameOffset += 9;
        } else if (fileName.startsWith(QLatin1String("ERROR: "))) {
            fileName = fileName.mid(7);
            fileNameOffset += 7;
        }
        if (description.startsWith(QLatin1String("note:"), Qt::CaseInsensitive))
            type = Task::Unknown;
        else if (description.startsWith(QLatin1String("warning:"), Qt::CaseInsensitive))
            type = Task::Warning;
        else if (description.startsWith(QLatin1String("error:"), Qt::CaseInsensitive))
            type = Task::Error;

        BuildSystemTask t(type, description, absoluteFilePath(FilePath::fromUserInput(fileName)),
                          match.captured(2).toInt() /* line */);
        LinkSpecs linkSpecs;
        addLinkSpecForAbsoluteFilePath(linkSpecs, t.file, t.line, fileNameOffset,
                                       fileName.length());
        scheduleTask(t, 1);
        return {Status::Done, linkSpecs};
    }
    if (lne.startsWith(QLatin1String("Project ERROR: "))
            || lne.startsWith(QLatin1String("ERROR: "))) {
        const QString description = lne.mid(lne.indexOf(QLatin1Char(':')) + 2);
        scheduleTask(BuildSystemTask(Task::Error, description), 1);
        return Status::Done;
    }
    if (lne.startsWith(QLatin1String("Project WARNING: "))
            || lne.startsWith(QLatin1String("WARNING: "))) {
        const QString description = lne.mid(lne.indexOf(QLatin1Char(':')) + 2);
        scheduleTask(BuildSystemTask(Task::Warning, description), 1);
        return Status::Done;
    }
    return Status::NotHandled;
}

} // QmakeProjectManager
