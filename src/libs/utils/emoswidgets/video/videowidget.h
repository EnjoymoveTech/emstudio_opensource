#ifndef VIDEOWIDGET_H
#define VIDEOWIDGET_H

#include <utils/utils_global.h>

#include <QWidget>
#include <QDateTime>
#include <functional>
#include "drawinfohelper.h"

namespace EmosWidgets {

namespace Internal {
class VideoWidgetPrivate;
class VideoProcess;
}
class EMSTUDIO_UTILS_EXPORT VideoWidget : public QWidget
{
    Q_OBJECT
public:
    enum RectType
    {
        RectType_Rect,//空心方块
        RectType_RectFill,//实心方块
        RectType_RectDash,//虚线方块
        RectType_Ellipse,//空心圆
        RectType_EllipseFill,//实心圆
        RectType_EllipseDash//虚线圆
    };

    //运行状态
    enum RunMode
    {
        RunMode_Play,
        RunMode_Pause,
        RunMode_Stop
    };
    //图像类型
    enum ImageType
    {
        ImageType_RGB24		=	0x1,
        ImageType_RGB32		=   0x2,
        //ImageType_Y8        =   0x3,
    };
    //图像信息
    typedef struct _CameraInfo{
        uint64_t rawdata_size; //原始数据大小

        uint64_t width; //图像宽
        uint64_t height; //图像高
        ImageType imageType = ImageType_RGB32; //解码出的图像类型

        uint64_t display_width = 0; //显示的大小
        uint64_t display_height = 0;

        int Sleep1 = 2;
        int Sleep2 = 2;
        int Sleep3 = 2;
    }CameraInfo;
    //标签格式
    enum OSDFormat {
        OSDFormat_Text = 0,             //文本
        OSDFormat_Date = 1,             //日期
        OSDFormat_Time = 2,             //时间
        OSDFormat_DateTime = 3,         //日期时间
        OSDFormat_Image = 4             //图片
    };

    //标签位置
    enum OSDPosition {
        OSDPosition_Left_Top = 0,       //左上角
        OSDPosition_Left_Bottom = 1,    //左下角
        OSDPosition_Right_Top = 2,      //右上角
        OSDPosition_Right_Bottom = 3    //右下角
    };

    explicit VideoWidget(QWidget *parent = 0);
    ~VideoWidget();

protected:
    void resizeEvent(QResizeEvent *);
    void enterEvent(QEvent *);
    void leaveEvent(QEvent *);
    void dropEvent(QDropEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);
    void paintEvent(QPaintEvent *);

public:
    QImage getImage()               const;
    QString getUrl()                const;

    bool getDrawImage()             const;
    bool getFillImage()             const;

    bool getFlowEnable()            const;
    QColor getFlowBgColor()         const;
    QColor getFlowPressColor()      const;
    int getFlowHeight()             const;

    int getBorderWidth()            const;
    QColor getBorderColor()         const;
    QColor getFocusColor()          const;
    QString getBgText()             const;
    QImage getBgImage()             const;

    bool getFpsVisible()           const;
    int getFpsFontSize()           const;
    QString getFpsText()           const;
    QColor getFpsColor()           const;
    OSDPosition getFpsPosition()   const;

    bool getOSD1Visible()           const;
    int getOSD1FontSize()           const;
    QString getOSD1Text()           const;
    QColor getOSD1Color()           const;
    QImage getOSD1Image()           const;
    OSDFormat getOSD1Format()       const;
    OSDPosition getOSD1Position()   const;

    bool getOSD2Visible()           const;
    int getOSD2FontSize()           const;
    QString getOSD2Text()           const;
    QColor getOSD2Color()           const;
    QImage getOSD2Image()           const;
    OSDFormat getOSD2Format()       const;
    OSDPosition getOSD2Position()   const;

    double getScaleVirtualX()       const;
    double getScaleVirtualY()       const;
    double getScaleVirtualWidth()   const;
    double getScaleVirtualHeight()  const;

    QSize sizeHint()                const;
    QSize minimumSizeHint()         const;

signals:
    //播放成功
    void receivePlayOk();
    //播放失败
    void receivePlayError();
    //播放结束
    void receivePlayFinsh();

    //收到图片信号
    void receiveImage(const QImage &image);

    //接收到拖曳文件
    void fileDrag(const QString &url);

    //工具栏单击
    void btnClicked(const QString &objName);

public slots:
    //设置是否保存文件
    void setSaveFile(bool saveFile);
    //设置保存间隔
    void setSaveInterval(int saveInterval);
    //设置保存文件夹
    void setSavePath(const QString &savePath);
    //设置保存文件名称
    void setFileName(const QString &fileName);

    //设置是否实时绘制图片
    void setDrawImage(bool drawImage);
    //设置是否拉伸填充
    void setFillImage(bool fillImage);

    //设置是否启用悬浮条
    void setFlowEnable(bool flowEnable);    
    //设置悬浮条背景颜色
    void setFlowBgColor(const QColor &flowBgColor);
    //设置悬浮条按下颜色
    void setFlowPressColor(const QColor &flowPressColor);
    //设置悬浮条高度
    void setFlowHeight(int height);

    //设置边框宽度
    void setBorderWidth(int borderWidth);
    //设置边框颜色
    void setBorderColor(const QColor &borderColor);
    //设置有焦点边框颜色
    void setFocusColor(const QColor &focusColor);    
    //设置无图像文字
    void setBgText(const QString &bgText);
    //设置无图像背景图
    void setBgImage(const QImage &bgImage);

    //设置fps是否可见
    void setFpsVisible(bool fpsVisible);
    //设置fps文字字号
    void setFpsFontSize(int fpsFontSize);
    //设置fps文字颜色
    void setFpsColor(const QColor &fpsColor);
    //设置fps位置
    void setFpsPosition(const OSDPosition &fpsPosition);

    //设置标签1是否可见
    void setOSD1Visible(bool osdVisible);
    //设置标签1文字字号
    void setOSD1FontSize(int osdFontSize);
    //设置标签1文本
    void setOSD1Text(const QString &osdText);
    //设置标签1文字颜色
    void setOSD1Color(const QColor &osdColor);
    //设置标签1图片
    void setOSD1Image(const QImage &osdImage);
    //设置标签1格式
    void setOSD1Format(const OSDFormat &osdFormat);
    //设置标签1位置
    void setOSD1Position(const OSDPosition &osdPosition);

    //设置标签2是否可见
    void setOSD2Visible(bool osdVisible);
    //设置标签2文字字号
    void setOSD2FontSize(int osdFontSize);
    //设置标签2文本
    void setOSD2Text(const QString &osdText);
    //设置标签2文字颜色
    void setOSD2Color(const QColor &osdColor);
    //设置标签2图片
    void setOSD2Image(const QImage &osdImage);
    //设置标签2格式
    void setOSD2Format(const OSDFormat &osdFormat);
    //设置标签2位置
    void setOSD2Position(const OSDPosition &osdPosition);

    //设置camera信息 必须设置
    void setCameraInfo(const CameraInfo& cameraInfo);

    //设置获取源数据回调函数 必须设置
    void setGetRawDataFunc(std::function<bool(uint8_t*/* buffer*/)> getRawData);

    //设置解码数据到rgb回调函数 必须设置
    void setDecode2RgbFunc(std::function<bool(uint8_t*/* in buffer*/, uint8_t*/* out buffer*/)> decode2Rgb);

    //打开设备
    bool open();
    //暂停
    bool pause();
    //继续
    bool next();
    //关闭设备
    bool close();
    //重新加载
    bool restart();

    //画框
    void addRect(int x, int y, int Width, int Height, QColor itemColor = QColor(255, 0, 0), float fLineWidth = 1,
                 RectType tRectType = RectType_Rect, const QString& text = "", QColor FontColor = QColor(255, 255, 255));
    void addCenterRect(int CenterX, int CenterY, int Width, int Height, QColor itemColor = QColor(255, 0, 0), float fLineWidth = 1,
                       RectType tRectType = RectType_Rect, const QString& text = "", QColor FontColor = QColor(255, 255, 255));
    void addLine(int x1, int y1, int x2, int y2, QColor itemColor = QColor(255, 0, 0), float fLineWidth = 1,
                 const QString& text = "", QColor FontColor = QColor(255, 255, 255));
    void addString(int x, int y, const QString& text, QColor FontColor = QColor(255, 255, 255), float fFontSize = 0);
    void resetItem();//删除所有item

private slots:
    //处理按钮单击
    void btnClicked();
private:
    void updateImage(const QImage &image);
    friend Internal::VideoProcess;
    QScopedPointer<Internal::VideoWidgetPrivate> d;
};
}
#endif // VIDEOWIDGET_H
