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

#include "codegeneration.h"

#include "algorithm.h"

#include <QTextStream>
#include <QSet>
#include <QStringList>
#include <QFileInfo>

namespace Utils {

EMSTUDIO_UTILS_EXPORT QString fileNameToCppIdentifier(const QString &s)
{
    QString rc;
    const int len = s.size();
    const QChar underscore =  QLatin1Char('_');
    const QChar dot =  QLatin1Char('.');

    for (int i = 0; i < len; i++) {
        const QChar c = s.at(i);
        if (c == underscore || c.isLetterOrNumber())
            rc += c;
        else if (c == dot)
            rc += underscore;
    }
    return rc;
}

EMSTUDIO_UTILS_EXPORT QString headerGuard(const QString &file)
{
    return headerGuard(file, QStringList());
}

EMSTUDIO_UTILS_EXPORT QString headerGuard(const QString &file, const QStringList &namespaceList)
{
    const QChar underscore = QLatin1Char('_');
    QString rc;
    for (int i = 0; i < namespaceList.count(); i++)
        rc += namespaceList.at(i).toUpper() + underscore;

    const QFileInfo fi(file);
    rc += fileNameToCppIdentifier(fi.fileName()).toUpper();
    return rc;
}

EMSTUDIO_UTILS_EXPORT
void writeIncludeFileDirective(const QString &file, bool globalInclude,
                               QTextStream &str)
{
    const QChar opening = globalInclude ?  QLatin1Char('<') : QLatin1Char('"');
    const QChar closing = globalInclude ?  QLatin1Char('>') : QLatin1Char('"');
    str << QLatin1String("#include ") << opening << file <<  closing << QLatin1Char('\n');
}

EMSTUDIO_UTILS_EXPORT void writeBeginQtVersionCheck(QTextStream &str)
{
    str << QLatin1String("#if QT_VERSION >= 0x050000\n");
}

static void qtSection(const QStringList &qtIncludes, QTextStream &str)
{
    QStringList sorted = qtIncludes;
    Utils::sort(sorted);
    for (const QString &inc : qAsConst(sorted)) {
        if (!inc.isEmpty())
            str << QStringLiteral("#include <%1>\n").arg(inc);
    }
}

EMSTUDIO_UTILS_EXPORT
void writeQtIncludeSection(const QStringList &qt4,
                           const QStringList &qt5,
                           bool addQtVersionCheck,
                           bool includeQtModule,
                           QTextStream &str)
{
    std::function<QString(const QString &)> trans;
    if (includeQtModule)
        trans = [](const QString &i) { return i; };
    else
        trans = [](const QString &i) { return i.mid(i.indexOf(QLatin1Char('/')) + 1); };

    QSet<QString> qt4Only = Utils::transform<QSet>(qt4, trans);
    QSet<QString> qt5Only = Utils::transform<QSet>(qt5, trans);

    if (addQtVersionCheck) {
        QSet<QString> common = qt4Only;
        common.intersect(qt5Only);

        // qglobal.h is needed for QT_VERSION
        if (includeQtModule)
            common.insert(QLatin1String("QtCore/qglobal.h"));
        else
            common.insert(QLatin1String("qglobal.h"));

        qt4Only.subtract(common);
        qt5Only.subtract(common);

        qtSection(Utils::toList(common), str);

        if (!qt4Only.isEmpty() || !qt5Only.isEmpty()) {
            if (addQtVersionCheck)
                writeBeginQtVersionCheck(str);
            qtSection(Utils::toList(qt5Only), str);
            if (addQtVersionCheck)
                str << QLatin1String("#else\n");
            qtSection(Utils::toList(qt4Only), str);
            if (addQtVersionCheck)
                str << QLatin1String("#endif\n");
        }
    } else {
        if (!qt5Only.isEmpty()) // default to Qt5
            qtSection(Utils::toList(qt5Only), str);
        else
            qtSection(Utils::toList(qt4Only), str);
    }
}

EMSTUDIO_UTILS_EXPORT
QString writeOpeningNameSpaces(const QStringList &l, const QString &indent,
                               QTextStream &str)
{
    const int count = l.size();
    QString rc;
    if (count) {
        str << '\n';
        for (int i = 0; i < count; i++) {
            str << rc << "namespace " << l.at(i) << " {\n";
            rc += indent;
        }
    }
    return rc;
}

EMSTUDIO_UTILS_EXPORT
void writeClosingNameSpaces(const QStringList &l, const QString &indent,
                            QTextStream &str)
{
    if (!l.empty())
        str << '\n';
    for (int i = l.size() - 1; i >= 0; i--) {
        if (i)
            str << QString(indent.size() * i, QLatin1Char(' '));
        str << "} // namespace " <<  l.at(i) << '\n';
    }
}

} // namespace Utils
