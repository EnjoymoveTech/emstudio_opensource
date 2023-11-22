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

#pragma once

#include "utils_global.h"

#include "porting.h"

#include <QList>
#include <QString>

QT_BEGIN_NAMESPACE
class QJsonValue;
QT_END_NAMESPACE

namespace Utils {

// Create a usable settings key from a category,
// for example Editor|C++ -> Editor_C__
EMSTUDIO_UTILS_EXPORT QString settingsKey(const QString &category);

// Return the common prefix part of a string list:
// "C:\foo\bar1" "C:\foo\bar2"  -> "C:\foo\bar"
EMSTUDIO_UTILS_EXPORT QString commonPrefix(const QStringList &strings);

// Return the common path of a list of files:
// "C:\foo\bar1" "C:\foo\bar2"  -> "C:\foo"
EMSTUDIO_UTILS_EXPORT QString commonPath(const QStringList &files);

// On Linux/Mac replace user's home path with ~
// Uses cleaned path and tries to use absolute path of "path" if possible
// If path is not sub of home path, or when running on Windows, returns the input
EMSTUDIO_UTILS_EXPORT QString withTildeHomePath(const QString &path);

// Removes first unescaped ampersand in text
EMSTUDIO_UTILS_EXPORT QString stripAccelerator(const QString &text);
// Quotes all ampersands
EMSTUDIO_UTILS_EXPORT QString quoteAmpersands(const QString &text);

EMSTUDIO_UTILS_EXPORT bool readMultiLineString(const QJsonValue &value, QString *out);

// Compare case insensitive and use case sensitive comparison in case of that being equal.
EMSTUDIO_UTILS_EXPORT int caseFriendlyCompare(const QString &a, const QString &b);

class EMSTUDIO_UTILS_EXPORT AbstractMacroExpander
{
public:
    virtual ~AbstractMacroExpander() {}
    // Not const, as it may change the state of the expander.
    //! Find an expando to replace and provide a replacement string.
    //! \param str The string to scan
    //! \param pos Position to start scan on input, found position on output
    //! \param ret Replacement string on output
    //! \return Length of string part to replace, zero if no (further) matches found
    virtual int findMacro(const QString &str, int *pos, QString *ret);
    //! Provide a replacement string for an expando
    //! \param name The name of the expando
    //! \param ret Replacement string on output
    //! \return True if the expando was found
    virtual bool resolveMacro(const QString &name, QString *ret, QSet<AbstractMacroExpander *> &seen) = 0;
private:
    bool expandNestedMacros(const QString &str, int *pos, QString *ret);
};

EMSTUDIO_UTILS_EXPORT void expandMacros(QString *str, AbstractMacroExpander *mx);
EMSTUDIO_UTILS_EXPORT QString expandMacros(const QString &str, AbstractMacroExpander *mx);

EMSTUDIO_UTILS_EXPORT int parseUsedPortFromNetstatOutput(const QByteArray &line);

template<typename T, typename Container>
T makeUniquelyNumbered(const T &preferred, const Container &reserved)
{
    if (!reserved.contains(preferred))
        return preferred;
    int i = 2;
    T tryName = preferred + QString::number(i);
    while (reserved.contains(tryName))
        tryName = preferred + QString::number(++i);
    return tryName;
}

template<typename T, typename Container>
T makeUniquelyNumberedInsensitive(const T &preferred, const Container &reserved, const T &first)
{
    Container lowerReserved;
    for(auto &reserve : reserved)
    {
        lowerReserved << reserve.toLower();
    }
    if (!lowerReserved.contains((preferred + first).toLower()))
        return preferred + first;
    int i = 2;
    T tryName = preferred + QString::number(i);
    while (lowerReserved.contains(tryName.toLower()))
        tryName = preferred + QString::number(++i);
    return tryName;
}

EMSTUDIO_UTILS_EXPORT int makeUniquelyId(const QSet<int> & idList, int step = 1, int start = 1);

EMSTUDIO_UTILS_EXPORT QString formatElapsedTime(qint64 elapsed);

/* This function is only necessary if you need to match the wildcard expression against a
 * string that might contain path separators - otherwise
 * QRegularExpression::wildcardToRegularExpression() can be used.
 * Working around QRegularExpression::wildcardToRegularExpression() taking native separators
 * into account and handling them to disallow matching a wildcard characters.
 */
EMSTUDIO_UTILS_EXPORT QString wildcardToRegularExpression(const QString &original);
} // namespace Utils
