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

// from Utils::FadingIndicator

#include <utils/utils_global.h>

#include <QString>
#include <QWidget>

namespace EmosWidgets {
namespace MessageTips {

enum TextSize {
    SmallText,
    MediumText,
    MediumText1,
    MediumText2,
    LargeText
};

enum TipsPosition {
    PositionTop,
    PositionMid,
    PositionBottom,
};

enum MessageType {
    Information = 0,
    Warning = 1,
    Critical = 2,
    Question = 3
};

EMSTUDIO_UTILS_EXPORT void showText(QWidget *parent, const QString &text,
                                     TextSize size = MediumText, TipsPosition pos = PositionMid, MessageType type = Information, int showTime = 2500, int closeTime = 500);
EMSTUDIO_UTILS_EXPORT void showPixmap(QWidget *parent, const QString &pixmap, TipsPosition pos = PositionMid, int showTime = 2000, int closeTime = 500);

}
} // EmosWidgets
