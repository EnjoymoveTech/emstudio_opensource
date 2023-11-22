#include "zoomruler.h"
#include <algorithm>
#include <QPainter>

namespace EmosWidgets {

ZoomRuler::ZoomRuler()
{
    m_zoomIndex = 0;
    m_zoomCount = 100;

    m_startValue = 0.00f;

    bgColor = QColor(20,20,20);
    lineColor = QColor(230,230,230);
    textColor = QColor(230,230,230);
    linehight = 15;
    fontsize = 12;
    rulerUnit = "ms";

    for(int i = 0; i < m_zoomCount; i++)
        m_zoom.push_back(static_cast<float>((i + 1)));
}

ZoomRuler::~ZoomRuler()
{

}

void ZoomRuler::setBgColor(const QColor& color)
{
    bgColor = color;
}

void ZoomRuler::setLineColor(const QColor& color)
{
    lineColor = color;
}

void ZoomRuler::setTextColor(const QColor& color)
{
    textColor = color;
}

void ZoomRuler::setFontSize(int size)
{
    fontsize = size;
}

void ZoomRuler::setLineHeight(int height)
{
    linehight = height;
}

void ZoomRuler::setUnit(QString unit)
{
    rulerUnit = unit;
}

bool ZoomRuler::zoomIn()
{
    if(canZoomIn())
    {
        m_zoomIndex ++;
        update();
        return true;
    }
    return false;
}

bool ZoomRuler::canZoomIn() const
{
    return (m_zoomIndex < m_zoom.size()-1) && (m_zoomIndex >= 0);
}

bool ZoomRuler::zoomOut()
{
    if(canZoomOut())
    {
        m_zoomIndex --;
        update();
        return true;
    }
    return false;
}

bool ZoomRuler::setZoom(float zoom)
{
    if(zoom >= 1 && zoom <= m_zoom.back())
    {
        for(int i = 0; i < m_zoomCount; i++)
        {
            if(zoom < m_zoom[i])
            {
                m_zoomIndex = i-1;
                update();
                return true;
            }
        }
    }
    return false;
}

float ZoomRuler::curZoom() const
{
    return m_zoom[m_zoomIndex];
}

bool ZoomRuler::canZoomOut() const
{
    return (m_zoomIndex < m_zoom.size()) && (m_zoomIndex > 0);
}

void ZoomRuler::reset()
{
    m_zoomIndex = 0;
    update();
}

float ZoomRuler::curSpaceValue() const
{
    return m_zoom[m_zoomIndex];
}

float ZoomRuler::curStartValue() const
{
    return m_startValue;
}

void ZoomRuler::setCurStartValue(float v)
{
    m_startValue = v;
    update();
}

//QRectF ZoomRuler::boundingRect() const
//{
//    return QRectF(0, 0, m_iWidth, m_iHeight);
//}

void ZoomRuler::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    //painter->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

    //绘制背景+边框
    painter->setPen(Qt::NoPen);
    painter->setBrush(bgColor);
    painter->drawRect(rect());

    //绘制刻度
    QPen linepen;
    linepen.setWidth(1);
    linepen.setColor(lineColor);
    painter->setPen(linepen);

    QFont font = painter->font();
    font.setPixelSize(fontsize);
    painter->setFont(font);

    float spaceValue = m_zoom[m_zoomIndex];
    int time = m_startValue;

    int rectX = 0;    //当前绘制坐标

    int useheight = linehight < rect().height() ? linehight : rect().height();
    int useheight2 = linehight/2 < rect().height()/2? linehight/2 : rect().height()/2;

    for(int rectIndex = 0; ruler_space * rectIndex <= rect().width(); rectIndex ++)
    {
        rectX = ruler_space * rectIndex + rect().x();

        if(rectIndex % 10 == 0)
        {
            painter->drawLine(QPoint(rectX, rect().y()), QPoint(rectX, useheight));

            painter->setPen(textColor);
            painter->drawText(rectX + 2, useheight + 5, QString::number(time));

            if(time == int(m_startValue))//绘制单位
                painter->drawText(rectX + 2 + fontsize, useheight + 5, rulerUnit);

            time += spaceValue;
            painter->setPen(linepen);
        }
        else
            painter->drawLine(QPoint(rectX, rect().y()), QPoint(rectX, useheight2));
    }
    painter->drawLine(rect().x(), rect().y(), rect().width(), rect().y());
}
}
