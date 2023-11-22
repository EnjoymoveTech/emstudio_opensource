#ifndef ZOOMRULER_H
#define ZOOMRULER_H

#include <utils/utils_global.h>
#include <QGraphicsRectItem>
#include <QObject>

namespace EmosWidgets {

const int ruler_space = 12;//每个小标尺间的像素

class EMSTUDIO_UTILS_EXPORT ZoomRuler : public QObject, public QGraphicsRectItem
{
    Q_OBJECT
public:
    ZoomRuler();
    ~ZoomRuler();

public:
    float curZoom() const;
    bool canZoomIn() const;
    bool canZoomOut() const;
    float curSpaceValue() const;
    float curStartValue()const;

public slots:
    void setBgColor(const QColor& color);
    void setLineColor(const QColor& color);
    void setTextColor(const QColor& color);
    void setFontSize(int size);
    void setLineHeight(int height);
    void setUnit(QString unit);
    bool zoomIn();//放大
    bool zoomOut();//缩小
    bool setZoom(float zoom);
    void reset();//重置
    void setCurStartValue(float v);
protected:
    //QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

private:
    int m_zoomCount;
    int m_zoomIndex;

    float m_startValue;

    QList<float> m_zoom;

    QString rulerUnit;  //刻度单位
    QColor bgColor;  //背景颜色
    QColor lineColor;
    QColor textColor; //文字颜色
    int linehight;  //刻度线高度(长条)
    int fontsize;   //字体大小
};
}
#endif // ZOOMRULER_H
