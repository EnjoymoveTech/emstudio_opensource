#ifndef NAVTITLE_H
#define NAVTITLE_H

#include "../utils_global.h"

#include <QWidget>

namespace Utils {


class EMSTUDIO_UTILS_EXPORT NavTitle : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(int leftIcon READ getLeftIcon WRITE setLeftIcon)
    Q_PROPERTY(int rightIcon1 READ getRightIcon1 WRITE setRightIcon1)
    Q_PROPERTY(int rightIcon2 READ getRightIcon2 WRITE setRightIcon2)
    Q_PROPERTY(int rightIcon3 READ getRightIcon3 WRITE setRightIcon3)
    Q_PROPERTY(int rightIcon4 READ getRightIcon4 WRITE setRightIcon4)
    Q_PROPERTY(int rightIcon5 READ getRightIcon5 WRITE setRightIcon5)
    Q_PROPERTY(int rightIcon6 READ getRightIcon6 WRITE setRightIcon6)
    Q_PROPERTY(int rightIcon7 READ getRightIcon7 WRITE setRightIcon7)
    Q_PROPERTY(int rightIcon8 READ getRightIcon8 WRITE setRightIcon8)
    Q_PROPERTY(int rightIcon9 READ getRightIcon9 WRITE setRightIcon9)
    Q_PROPERTY(int rightIcon10 READ getRightIcon10 WRITE setRightIcon10)

    Q_PROPERTY(int padding READ getPadding WRITE setPadding)
    Q_PROPERTY(int iconSize READ getIconSize WRITE setIconSize)
    Q_PROPERTY(bool textCenter READ getTextCenter WRITE setTextCenter)
    Q_PROPERTY(QString text READ getText WRITE setText)

    Q_PROPERTY(QColor bgColor READ getBgColor WRITE setBgColor)
    Q_PROPERTY(QColor textColor READ getTextColor WRITE setTextColor)
    Q_PROPERTY(QColor borderColor READ getBorderColor WRITE setBorderColor)

    Q_PROPERTY(QColor iconNormalColor READ getIconNormalColor WRITE setIconNormalColor)
    Q_PROPERTY(QColor iconHoverColor READ getIconHoverColor WRITE setIconHoverColor)
    Q_PROPERTY(QColor iconPressColor READ getIconPressColor WRITE setIconPressColor)

public:
    explicit NavTitle(QWidget *parent = 0);

protected:
    void resizeEvent(QResizeEvent *);
    void leaveEvent(QEvent *);
    void mousePressEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void paintEvent(QPaintEvent *);
    void drawIcon(QPainter *painter, const QRect &rect, int icon);

private:
    int leftIcon;               //左侧图标
    int rightIcon1;             //右侧图标1
    int rightIcon2;             //右侧图标2
    int rightIcon3;             //右侧图标3
    int rightIcon4;             //右侧图标4
    int rightIcon5;             //右侧图标5
    int rightIcon6;             //右侧图标6
    int rightIcon7;             //右侧图标7
    int rightIcon8;             //右侧图标8
    int rightIcon9;             //右侧图标9
    int rightIcon10;            //右侧图标10

    int padding;                //左侧右侧间距
    int iconSize;               //图标大小
    bool textCenter;            //文字居中
    QString text;               //标题文字

    QColor bgColor;             //背景颜色
    QColor textColor;           //文本颜色
    QColor borderColor;         //边框颜色

    QColor iconNormalColor;     //图标正常颜色
    QColor iconHoverColor;      //图标悬停颜色
    QColor iconPressColor;      //图标按下颜色

    bool pressed;               //鼠标是否按下
    QPoint lastPoint;           //鼠标按下处的坐标

    QFont iconFont;             //图形字体
    QRect leftRect;             //左侧图标区域
    QRect rightRect1;           //右侧图标1区域
    QRect rightRect2;           //右侧图标2区域
    QRect rightRect3;           //右侧图标3区域
    QRect rightRect4;           //右侧图标4区域
    QRect rightRect5;           //右侧图标5区域
    QRect rightRect6;           //右侧图标6区域
    QRect rightRect7;           //右侧图标7区域
    QRect rightRect8;           //右侧图标8区域
    QRect rightRect9;           //右侧图标9区域
    QRect rightRect10;          //右侧图标10区域

public:
    int getLeftIcon()           const;
    int getRightIcon1()         const;
    int getRightIcon2()         const;
    int getRightIcon3()         const;
    int getRightIcon4()         const;
    int getRightIcon5()         const;
    int getRightIcon6()         const;
    int getRightIcon7()         const;
    int getRightIcon8()         const;
    int getRightIcon9()         const;
    int getRightIcon10()        const;

    int getPadding()            const;
    int getIconSize()           const;
    bool getTextCenter()        const;
    QString getText()           const;

    QColor getBgColor()         const;
    QColor getTextColor()       const;
    QColor getBorderColor()     const;

    QColor getIconNormalColor() const;
    QColor getIconHoverColor()  const;
    QColor getIconPressColor()  const;

    QSize sizeHint()            const;
    QSize minimumSizeHint()     const;

public Q_SLOTS:
    void setLeftIcon(int leftIcon);
    void setRightIcon1(int rightIcon1);
    void setRightIcon2(int rightIcon2);
    void setRightIcon3(int rightIcon3);
    void setRightIcon4(int rightIcon4);
    void setRightIcon5(int rightIcon5);
    void setRightIcon6(int rightIcon6);
    void setRightIcon7(int rightIcon7);
    void setRightIcon8(int rightIcon8);
    void setRightIcon9(int rightIcon9);
    void setRightIcon10(int rightIcon10);

    void setPadding(int padding);
    void setIconSize(int iconSize);
    void setTextCenter(bool textCenter);
    void setText(const QString &text);

    void setBgColor(const QColor &bgColor);
    void setTextColor(const QColor &textColor);
    void setBorderColor(const QColor &borderColor);

    void setIconNormalColor(const QColor &iconNormalColor);
    void setIconHoverColor(const QColor &iconHoverColor);
    void setIconPressColor(const QColor &iconPressColor);

Q_SIGNALS:
    void mousePressed(int index);
    void mouseReleased(int index);
};
}
#endif // NAVTITLE_H
