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

#include "ilocatorfilter.h"

#include <coreplugin/coreconstants.h>
#include <utils/fuzzymatcher.h>

#include <QBoxLayout>
#include <QCheckBox>
#include <QCoreApplication>
#include <QDialog>
#include <QDialogButtonBox>
#include <QLabel>
#include <QLineEdit>
#include <QRegularExpression>

using namespace Core;
using namespace Utils;

/*!
    \class Core::ILocatorFilter
    \inheaderfile coreplugin/locator/ilocatorfilter.h
    \inmodule EmStudio

    \brief The ILocatorFilter class adds a locator filter.

    The filter is added to \uicontrol Tools > \uicontrol Locate.
*/

/*!
    \class Core::LocatorFilterEntry
    \inmodule EmStudio
    \internal
*/

/*!
    \class Core::LocatorFilterEntry::HighlightInfo
    \inmodule EmStudio
    \internal
*/

static QList<ILocatorFilter *> g_locatorFilters;

/*!
    Constructs a locator filter with \a parent. Call from subclasses.
*/
ILocatorFilter::ILocatorFilter(QObject *parent):
    QObject(parent)
{
    g_locatorFilters.append(this);
}

ILocatorFilter::~ILocatorFilter()
{
    g_locatorFilters.removeOne(this);
}

/*!
    Returns the list of all locator filters.
*/
const QList<ILocatorFilter *> ILocatorFilter::allLocatorFilters()
{
    return g_locatorFilters;
}

/*!
    Specifies a shortcut string that can be used to explicitly choose this
    filter in the locator input field by preceding the search term with the
    shortcut string and a whitespace.

    The default value is an empty string.

    \sa setShortcutString()
*/
QString ILocatorFilter::shortcutString() const
{
    return m_shortcut;
}

/*!
    Performs actions that need to be done in the main thread before actually
    running the search for \a entry.

    Called on the main thread before matchesFor() is called in a separate
    thread.

    The default implementation does nothing.

    \sa matchesFor()
*/
void ILocatorFilter::prepareSearch(const QString &entry)
{
    Q_UNUSED(entry)
}

/*!
    Sets the \a shortcut string that can be used to explicitly choose this
    filter in the locator input field. Call from the constructor of subclasses
    to set the default setting.

    \sa shortcutString()
*/
void ILocatorFilter::setShortcutString(const QString &shortcut)
{
    m_shortcut = shortcut;
}

/*!
    Returns data that can be used to restore the settings for this filter
    (for example at startup).
    By default, adds the base settings (shortcut string, included by default)
    with a data stream.

    \sa restoreState()
*/
QByteArray ILocatorFilter::saveState() const
{
    QByteArray value;
    QDataStream out(&value, QIODevice::WriteOnly);
    out << shortcutString();
    out << isIncludedByDefault();
    return value;
}

/*!
    Restores the \a state of the filter from data previously created by
    saveState().

    \sa saveState()
*/
void ILocatorFilter::restoreState(const QByteArray &state)
{
    QString shortcut;
    bool defaultFilter;

    QDataStream in(state);
    in >> shortcut;
    in >> defaultFilter;

    setShortcutString(shortcut);
    setIncludedByDefault(defaultFilter);
}

/*!
    Opens a dialog for the \a parent widget that allows the user to configure
    various aspects of the filter. Called when the user requests to configure
    the filter.

    Set \a needsRefresh to \c true, if a refresh() should be done after
    closing the dialog. Return \c false if the user canceled the dialog.

    The default implementation allows changing the shortcut and whether the
    filter is included by default.

    \sa refresh()
*/
bool ILocatorFilter::openConfigDialog(QWidget *parent, bool &needsRefresh)
{
    Q_UNUSED(needsRefresh)

    QDialog dialog(parent, Qt::WindowTitleHint | Qt::WindowSystemMenuHint);
    dialog.setWindowTitle(msgConfigureDialogTitle());

    auto vlayout = new QVBoxLayout(&dialog);
    auto hlayout = new QHBoxLayout;
    QLineEdit *shortcutEdit = new QLineEdit(shortcutString());
    QCheckBox *includeByDefault = new QCheckBox(msgIncludeByDefault());
    includeByDefault->setToolTip(msgIncludeByDefaultToolTip());
    includeByDefault->setChecked(isIncludedByDefault());

    auto prefixLabel = new QLabel(msgPrefixLabel());
    prefixLabel->setToolTip(msgPrefixToolTip());
    hlayout->addWidget(prefixLabel);
    hlayout->addWidget(shortcutEdit);
    hlayout->addWidget(includeByDefault);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok |
                                                       QDialogButtonBox::Cancel);
    connect(buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    vlayout->addLayout(hlayout);
    vlayout->addStretch();
    vlayout->addWidget(buttonBox);

    if (dialog.exec() == QDialog::Accepted) {
        setShortcutString(shortcutEdit->text().trimmed());
        setIncludedByDefault(includeByDefault->isChecked());
        return true;
    }

    return false;
}

/*!
    Returns whether a case sensitive or case insensitive search should be
    performed for the search term \a str.
*/
Qt::CaseSensitivity ILocatorFilter::caseSensitivity(const QString &str)
{
    return str == str.toLower() ? Qt::CaseInsensitive : Qt::CaseSensitive;
}

/*!
    Creates the search term \a text as a regular expression with case
    sensitivity set to \a caseSensitivity.
*/
QRegularExpression ILocatorFilter::createRegExp(const QString &text, Qt::CaseSensitivity caseSensitivity)
{
    return FuzzyMatcher::createRegExp(text, caseSensitivity);
}

/*!
    Returns information for highlighting the results of matching the regular
    expression, specified by \a match, for the data of the type \a dataType.
*/
LocatorFilterEntry::HighlightInfo ILocatorFilter::highlightInfo(
        const QRegularExpressionMatch &match, LocatorFilterEntry::HighlightInfo::DataType dataType)
{
    const FuzzyMatcher::HighlightingPositions positions =
            FuzzyMatcher::highlightingPositions(match);

    return LocatorFilterEntry::HighlightInfo(positions.starts, positions.lengths, dataType);
}

/*!
    Specifies a title for configuration dialogs.
*/
QString ILocatorFilter::msgConfigureDialogTitle()
{
    return tr("Filter Configuration");
}

/*!
    Specifies a label for the prefix input field in configuration dialogs.
*/
QString ILocatorFilter::msgPrefixLabel()
{
    return tr("Prefix:");
}

/*!
    Specifies a tooltip for the  prefix input field in configuration dialogs.
*/
QString ILocatorFilter::msgPrefixToolTip()
{
    return tr("Type the prefix followed by a space and search term to restrict search to the filter.");
}

/*!
    Specifies a label for the include by default input field in configuration
    dialogs.
*/
QString ILocatorFilter::msgIncludeByDefault()
{
    return tr("Include by default");
}

/*!
    Specifies a tooltip for the include by default input field in configuration
    dialogs.
*/
QString ILocatorFilter::msgIncludeByDefaultToolTip()
{
    return tr("Include the filter when not using a prefix for searches.");
}

/*!
    Returns whether a configuration dialog is available for this filter.

    The default is \c true.

    \sa setConfigurable()
*/
bool ILocatorFilter::isConfigurable() const
{
    return m_isConfigurable;
}

/*!
    Returns whether using the shortcut string is required to use this filter.
    The default is \c false.

    \sa shortcutString()
    \sa setIncludedByDefault()
*/
bool ILocatorFilter::isIncludedByDefault() const
{
    return m_includedByDefault;
}

/*!
    Sets whether using the shortcut string is required to use this filter
    to \a includedByDefault.

    Call from the constructor of subclasses to change the default.

    \sa isIncludedByDefault()
*/
void ILocatorFilter::setIncludedByDefault(bool includedByDefault)
{
    m_includedByDefault = includedByDefault;
}

/*!
    Returns whether the filter should be hidden in the
    \uicontrol {Locator filters} filter, menus, and locator settings.

    The default is \c false.

    \sa setHidden()
*/
bool ILocatorFilter::isHidden() const
{
    return m_hidden;
}

/*!
    Sets the filter in the \uicontrol {Locator filters} filter, menus, and
    locator settings to \a hidden. Call in the constructor of subclasses.
*/
void ILocatorFilter::setHidden(bool hidden)
{
    m_hidden = hidden;
}

/*!
    Returns whether the filter is currently available. Disabled filters are
    neither visible in menus nor included in searches, even when the search is
    prefixed with their shortcut string.

    The default is \c true.

    \sa setEnabled()
*/
bool ILocatorFilter::isEnabled() const
{
    return m_enabled;
}

/*!
    Returns the filter's unique ID.

    \sa setId()
*/
Id ILocatorFilter::id() const
{
    return m_id;
}

/*!
    Returns the filter's action ID.
*/
Id ILocatorFilter::actionId() const
{
    return m_id.withPrefix("Locator.");
}

/*!
    Returns the filter's translated display name.

    \sa setDisplayName()
*/
QString ILocatorFilter::displayName() const
{
    return m_displayName;
}

/*!
    Returns the priority that is used for ordering the results when multiple
    filters are used.

    The default is ILocatorFilter::Medium.

    \sa setPriority()
*/
ILocatorFilter::Priority ILocatorFilter::priority() const
{
    return m_priority;
}

/*!
    Sets whether the filter is currently available to \a enabled.

    \sa isEnabled()
*/
void ILocatorFilter::setEnabled(bool enabled)
{
    m_enabled = enabled;
}

/*!
    Sets the filter's unique \a id.
    Subclasses must set the ID in their constructor.

    \sa id()
*/
void ILocatorFilter::setId(Id id)
{
    m_id = id;
}

/*!
    Sets the \a priority of results of this filter in the result list.

    \sa priority()
*/
void ILocatorFilter::setPriority(Priority priority)
{
    m_priority = priority;
}

/*!
    Sets the translated display name of this filter to \a displayString.

    Subclasses must set the display name in their constructor.

    \sa displayName()
*/
void ILocatorFilter::setDisplayName(const QString &displayString)
{
    m_displayName = displayString;
}

/*!
    Sets whether the filter provides a configuration dialog to \a configurable.
    Most filters should at least provide the default dialog.

    \sa isConfigurable()
*/
void ILocatorFilter::setConfigurable(bool configurable)
{
    m_isConfigurable = configurable;
}

/*!
    \fn QList<Core::LocatorFilterEntry> Core::ILocatorFilter::matchesFor(QFutureInterface<Core::LocatorFilterEntry> &future, const QString &entry)

    Returns the list of results of this filter for the search term \a entry.
    This is run in a separate thread, but is guaranteed to only run in a single
    thread at any given time. Quickly running preparations can be done in the
    GUI thread in prepareSearch().

    Implementations should do a case sensitive or case insensitive search
    depending on caseSensitivity(). If \a future is \c canceled, the search
    should be aborted.

    \sa prepareSearch()
    \sa caseSensitivity()
*/

/*!
    \fn void Core::ILocatorFilter::accept(Core::LocatorFilterEntry selection, QString *newText, int *selectionStart, int *selectionLength) const

    Called with the entry specified by \a selection when the user activates it
    in the result list.
    Implementations can return a new search term \a newText, which has \a selectionLength characters
    starting from \a selectionStart preselected, and the cursor set to the end of the selection.
*/

/*!
    \fn void Core::ILocatorFilter::refresh(QFutureInterface<void> &future)

    Refreshes cached data asynchronously.

    If \a future is \c canceled, the refresh should be aborted.
*/

/*!
    \enum Core::ILocatorFilter::Priority

    This enum value holds the priority that is used for ordering the results
    when multiple filters are used.

    \value  Highest
            The results for this filter are placed above the results for filters
            that have other priorities.
    \value  High
    \value  Medium
            The default value.
    \value  Low
            The results for this filter are placed below the results for filters
            that have other priorities.
*/

/*!
    \enum Core::ILocatorFilter::MatchLevel

    This enum value holds the level for ordering the results based on how well
    they match the search criteria.

    \value Best
           The result is the best match for the regular expression.
    \value Better
    \value Good
    \value Normal
    \value Count
           The result has the highest number of matches for the regular
           expression.
*/
