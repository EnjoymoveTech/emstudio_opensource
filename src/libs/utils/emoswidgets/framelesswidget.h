﻿#ifndef FRAMELESSWIDGET_H
#define FRAMELESSWIDGET_H

#include <utils/utils_global.h>
#include <QWidget>

namespace EmosWidgets
{

class EMSTUDIO_UTILS_EXPORT FramelessWidget : public QObject
{
    Q_OBJECT
public:
    explicit FramelessWidget(QObject *parent = 0);

protected:
    bool eventFilter(QObject *watched, QEvent *event);

private:
    int padding;                    //边距
    QWidget *widget;                //无边框窗体

    bool pressed;                   //鼠标按下
    bool pressedLeft;               //鼠标按下左侧
    bool pressedRight;              //鼠标按下右侧
    bool pressedTop;                //鼠标按下上侧
    bool pressedBottom;             //鼠标按下下侧
    bool pressedLeftTop;            //鼠标按下左上侧
    bool pressedRightTop;           //鼠标按下右上侧
    bool pressedLeftBottom;         //鼠标按下左下侧
    bool pressedRightBottom;        //鼠标按下右下侧

    int rectX, rectY, rectW, rectH; //窗体坐标+宽高
    QPoint lastPos;                 //鼠标按下处坐标

    QRectF rectLeft;                //左侧区域
    QRectF rectRight;               //右侧区域
    QRectF rectTop;                 //上侧区域
    QRectF rectBottom;              //下侧区域
    QRectF rectLeftTop;             //左上侧区域
    QRectF rectRightTop;            //右上侧区域
    QRectF rectLeftBottom;          //左下侧区域
    QRectF rectRightBottom;         //右下侧区域

public Q_SLOTS:
    //设置边距
    void setPadding(int padding);
    //设置要无边框的窗体
    void setWidget(QWidget *widget);
};
}
#endif // FRAMELESSWIDGET_H
