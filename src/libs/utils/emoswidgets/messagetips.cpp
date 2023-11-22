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

#include "messagetips.h"

#include <utils/stylehelper.h>

#include <QGraphicsOpacityEffect>
#include <QHBoxLayout>
#include <QLabel>
#include <QPainter>
#include <QPixmap>
#include <QPointer>
#include <QPropertyAnimation>
#include <QTimer>

#include "widgetsstyle.h"
#include "widgetstheme.h"

using namespace Utils;

namespace EmosWidgets {
namespace MessageTips {

class MessageTipsPrivate : public QWidget
{
    Q_OBJECT

public:
    MessageTipsPrivate(QWidget *parent, MessageTips::TextSize size, MessageType type, int closeTime)
        : QWidget(parent)
    {
        m_closeTime = closeTime;
        m_effect = new QGraphicsOpacityEffect(this);
        setGraphicsEffect(m_effect);
        m_effect->setOpacity(.999);

        m_label = new QLabel;

        int textSize = 15;
        switch(size)
        {
        case MessageTips::SmallText:
            textSize = 22;
            break;
        case MessageTips::MediumText:
            textSize = 30;
            break;
        case MessageTips::MediumText1:
            textSize = 32;
            break;
        case MessageTips::MediumText2:
            textSize = 35;
            break;
        case MessageTips::LargeText:
            textSize = 40;
            break;
        }

        QFont font("Roboto");
        font.setPixelSize(textSize);
        m_label->setFont(font);

        switch (type) {
        case Information:
            m_backgroundColor = QColor(20,20,20);
            m_textColor = QColor(255,255,255);
            break;
        case Warning:
            m_backgroundColor = QColor(255,193,37);
            m_textColor = QColor(0,0,0);
            break;
        case Critical:
            m_backgroundColor = QColor(255,0,0);
            m_textColor = QColor(255,255,255);
            break;
        case Question:
            m_backgroundColor = QColor(20,20,20);
            m_textColor = QColor(255,255,255);
            break;
        default:
            m_backgroundColor = QColor(20,20,20);
            m_textColor = QColor(255,255,255);
            break;
        }

        QPalette pal = palette();
        pal.setColor(QPalette::WindowText, m_textColor);
        m_label->setPalette(pal);
        auto layout = new QHBoxLayout;
        setLayout(layout);
        layout->addWidget(m_label);
    }

    void setText(const QString &text, TipsPosition position)
    {
        m_pixmap = QPixmap();
        m_label->setText(text);
        layout()->setSizeConstraint(QLayout::SetFixedSize);
        adjustSize();
        QWidget *parent = parentWidget();
        QPoint pos = parent ? (parent->rect().center() - rect().center()) : QPoint();
        if(position == PositionTop)
        {
            pos.setY(parent->rect().height()/10);
        }
        else if(position == PositionBottom)
        {
            pos.setY(parent->rect().height() - rect().height() - parent->rect().height() / 10);
        }

        if (pixmapIndicator && pixmapIndicator->geometry().intersects(QRect(pos, size())))
            pos.setY(pixmapIndicator->geometry().bottom() + 1);
        move(pos);
    }

    void setPixmap(const QString &uri, TipsPosition position)
    {
        m_label->hide();
        m_pixmap.load(StyleHelper::dpiSpecificImageFile(uri));
        layout()->setSizeConstraint(QLayout::SetNoConstraint);
        resize(m_pixmap.size() / m_pixmap.devicePixelRatio());
        QWidget *parent = parentWidget();
        QPoint pos = parent ? (parent->rect().center() - rect().center()) : QPoint();
        if(position == PositionTop)
        {
            pos.setY(parent->rect().height()/10);
        }
        else if(position == PositionBottom)
        {
            pos.setY(parent->rect().height() - rect().height() - parent->rect().height() / 10);
        }

        if (textIndicator && textIndicator->geometry().intersects(QRect(pos, size())))
            pos.setY(textIndicator->geometry().bottom() + 1);
        move(pos);
    }

    void run(int ms)
    {
        show();
        raise();
        QTimer::singleShot(ms, this, &MessageTipsPrivate::runInternal);
    }

    static QPointer<MessageTipsPrivate> textIndicator;
    static QPointer<MessageTipsPrivate> pixmapIndicator;

protected:
    void paintEvent(QPaintEvent *) override
    {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);
        if (!m_pixmap.isNull()) {
            p.drawPixmap(rect(), m_pixmap);
        } else {
            p.setBrush(m_backgroundColor);
            p.setPen(Qt::NoPen);
            p.drawRoundedRect(rect(), 8, 8);
        }
    }

private:
    void runInternal()
    {
        QPropertyAnimation *anim = new QPropertyAnimation(m_effect, "opacity", this);
        anim->setDuration(m_closeTime);
        anim->setEndValue(0.);
        connect(anim, &QAbstractAnimation::finished, this, &QObject::deleteLater);
        anim->start(QAbstractAnimation::DeleteWhenStopped);
    }

    QGraphicsOpacityEffect *m_effect;
    QLabel *m_label;
    QPixmap m_pixmap;
    int m_closeTime;
    QColor m_textColor;
    QColor m_backgroundColor;
};

QPointer<MessageTipsPrivate> MessageTipsPrivate::textIndicator;
QPointer<MessageTipsPrivate> MessageTipsPrivate::pixmapIndicator;


void showText(QWidget *parent, const QString &text, TextSize size, TipsPosition position, MessageType type, int showTime, int closeTime)
{
    QPointer<MessageTipsPrivate> &indicator
        = MessageTipsPrivate::textIndicator;
    if (indicator)
        delete indicator;
    indicator = new MessageTipsPrivate(parent, size, type, closeTime);
    indicator->setText(text, position);
    indicator->run(showTime); // deletes itself
}

void showPixmap(QWidget *parent, const QString &pixmap, TipsPosition position, int showTime, int closeTime)
{
    QPointer<MessageTipsPrivate> &indicator
        = MessageTipsPrivate::pixmapIndicator;
    if (indicator)
        delete indicator;
    indicator = new MessageTipsPrivate(parent, LargeText, Information, closeTime);
    indicator->setPixmap(pixmap, position);
    indicator->run(showTime); // deletes itself
}
}

}

#include "messagetips.moc"
