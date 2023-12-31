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

#include "generaleditor_global.h"

#include "icodestylepreferencesfactory.h"

QT_BEGIN_NAMESPACE
class QVBoxLayout;
QT_END_NAMESPACE

namespace TextEditor {

class ICodeStylePreferencesFactory;
class ICodeStylePreferences;
class SnippetEditorWidget;

class TEXTEDITOR_EXPORT CodeStyleEditor : public CodeStyleEditorWidget
{
    Q_OBJECT
public:
    CodeStyleEditor(ICodeStylePreferencesFactory *factory,
                           ICodeStylePreferences *codeStyle, QWidget *parent = nullptr);

private:
    void updatePreview();

    QVBoxLayout *m_layout;
    ICodeStylePreferencesFactory *m_factory;
    ICodeStylePreferences *m_codeStyle;
    SnippetEditorWidget *m_preview;
};

} // namespace TextEditor
