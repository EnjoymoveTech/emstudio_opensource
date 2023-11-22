#include "videowidget.h"
#include <QApplication>
#include <QColor>

#include <QFontDatabase>
#include <QPushButton>
#include <QTreeWidget>
#include <QLayout>
#include <QTimer>
#include <QDir>
#include <QPainter>
#include <QEvent>
#include <QMimeData>
#include <QUrl>
#include <QDebug>
#include <QDropEvent>

#include "videoprocess.h"
#include "drawinfohelper.h"

namespace EmosWidgets {
namespace Internal{

class VideoWidgetPrivate : public QObject
{
    Q_OBJECT
public:
    VideoWidgetPrivate(VideoWidget* videoWidget, QObject *parent=nullptr);

    CDrawInfoHelper m_drawHelper;//用户使用的图像标记添加
    QImage image;                   //要显示的图片
    QWidget *flowPanel;             //悬浮条面板

    int panelHeight = 40;            //面板高度
    bool copyImage = true;                 //是否拷贝图片
    bool drawImage = true;                 //是否绘制图片
    bool fillImage = true;                 //自动拉伸填充

    bool flowEnable = true;                //是否显示悬浮条
    QColor flowBgColor = "#050505";             //悬浮条背景颜色
    QColor flowPressColor = "#5EC7D9";          //悬浮条按下颜色
    int iconSize = 35;                   //图标大小

    int borderWidth = 5;               //边框宽度
    QColor borderColor = "#000000";             //边框颜色
    QColor focusColor = "#22A3A9";              //有焦点边框颜色
    QString bgText = "Stop";                 //默认无图像显示文字
    QImage bgImage = QImage();                 //默认无图像背景图片

    bool fpsVisible = true;               //显示fps
    int fpsFontSize = 16;               //fps字号
    QColor fpsColor = "#FF0000";               //fps颜色
    QString fpsText = "fps";               //fps文本
    VideoWidget::OSDPosition fpsPosition = VideoWidget::OSDPosition_Right_Top;       //fps位置

    bool osd1Visible = true;               //显示标签1
    int osd1FontSize = 16;               //标签1字号
    QString osd1Text = "时间";               //标签1文本
    QColor osd1Color = "#FF0000";               //标签1颜色
    QImage osd1Image = QImage();               //标签1图片
    VideoWidget::OSDFormat osd1Format = VideoWidget::OSDFormat_DateTime;           //标签1文本格式
    VideoWidget::OSDPosition osd1Position = VideoWidget::OSDPosition_Left_Top;       //标签1位置

    bool osd2Visible = false;               //显示标签2
    int osd2FontSize = 16;               //标签2字号
    QString osd2Text= "通道名称";               //标签2文本
    QColor osd2Color = "#FF0000";               //标签2颜色
    QImage osd2Image = QImage();               //标签2图片
    VideoWidget::OSDFormat osd2Format = VideoWidget::OSDFormat_Text;           //标签2文本格式
    VideoWidget::OSDPosition osd2Position = VideoWidget::OSDPosition_Left_Bottom;       //标签2位置

    QScopedPointer<Internal::VideoProcess> m_videoProcess;
    VideoWidget* q;
    QMutex m_csImageLock;
    QImage dropImage = QImage();               //拖拽的图片

    void drawBorder(QPainter *painter);
    void drawBg(QPainter *painter);
    void drawImg(QPainter *painter, QImage img);
    void drawOSD(QPainter *painter,
                 int osdFontSize,
                 const QString &osdText,
                 const QColor &osdColor,
                 const QImage &osdImage,
                 const VideoWidget::OSDFormat &osdFormat,
                 const VideoWidget::OSDPosition &osdPosition);
    //初始化悬浮条样式
    void initFlowStyle();
    //接收图像并绘制
    void updateImage(const QImage &image);
    //拖拽图片显示设置
    bool getUrlRawData(uint8_t* buffer);
    bool setUrlImage(const QString &url);
};

VideoWidgetPrivate::VideoWidgetPrivate(VideoWidget* videoWidget, QObject *parent) :
    QObject(parent),
    q(videoWidget)
{
    m_videoProcess.reset(new VideoProcess(q));
}

void VideoWidgetPrivate::drawBorder(QPainter *painter)
{
    painter->save();
    QPen pen;
    pen.setWidth(borderWidth);
    pen.setColor(q->hasFocus() ? focusColor : borderColor);
    painter->setPen(pen);
    painter->drawRect(q->rect());
    painter->restore();
}

void VideoWidgetPrivate::drawBg(QPainter *painter)
{
    painter->save();

    //背景图片为空则绘制文字,否则绘制背景图片
    if (bgImage.isNull()) {
        painter->setPen(q->palette().foreground().color());
        painter->drawText(q->rect(), Qt::AlignCenter, bgText);
    } else {
        //居中绘制
        int pixX = q->rect().center().x() - bgImage.width() / 2;
        int pixY = q->rect().center().y() - bgImage.height() / 2;
        QPoint point(pixX, pixY);
        painter->drawImage(point, bgImage);
    }

    painter->restore();
}

void VideoWidgetPrivate::drawImg(QPainter *painter, QImage img)
{
    painter->save();

    int offset = borderWidth * 1 + 0;
    if (fillImage) {
        QRect rect(offset / 2, offset / 2, q->width() - offset, q->height() - offset);
        painter->drawImage(rect, img);
    } else {
        //按照比例自动居中绘制
        img = img.scaled(q->width() - offset, q->height() - offset, Qt::KeepAspectRatio);
        int pixX = q->rect().center().x() - img.width() / 2;
        int pixY = q->rect().center().y() - img.height() / 2;
        QPoint point(pixX, pixY);
        painter->drawImage(point, img);
    }

    painter->restore();
}

void VideoWidgetPrivate::drawOSD(QPainter *painter,
                          int osdFontSize,
                          const QString &osdText,
                          const QColor &osdColor,
                          const QImage &osdImage,
                          const VideoWidget::OSDFormat &osdFormat,
                          const VideoWidget::OSDPosition &osdPosition)
{
    painter->save();

    //标签位置尽量偏移多一点避免遮挡
    QRect osdRect(q->rect().x() + (borderWidth * 2), q->rect().y() + (borderWidth * 2), q->width() - (borderWidth * 5), q->height() - (borderWidth * 5));
    int flag = Qt::AlignLeft | Qt::AlignTop;
    QPoint point = QPoint(osdRect.x(), osdRect.y());

    if (osdPosition == VideoWidget::OSDPosition_Left_Top) {
        flag = Qt::AlignLeft | Qt::AlignTop;
        point = QPoint(osdRect.x(), osdRect.y());
    } else if (osdPosition == VideoWidget::OSDPosition_Left_Bottom) {
        flag = Qt::AlignLeft | Qt::AlignBottom;
        point = QPoint(osdRect.x(), osdRect.height() - osdImage.height());
    } else if (osdPosition == VideoWidget::OSDPosition_Right_Top) {
        flag = Qt::AlignRight | Qt::AlignTop;
        point = QPoint(osdRect.width() - osdImage.width(), osdRect.y());
    } else if (osdPosition == VideoWidget::OSDPosition_Right_Bottom) {
        flag = Qt::AlignRight | Qt::AlignBottom;
        point = QPoint(osdRect.width() - osdImage.width(), osdRect.height() - osdImage.height());
    }

    if (osdFormat == VideoWidget::OSDFormat_Image) {
        painter->drawImage(point, osdImage);
    } else {
        QDateTime now = QDateTime::currentDateTime();
        QString text = osdText;
        if (osdFormat == VideoWidget::OSDFormat_Date) {
            text = now.toString("yyyy-MM-dd");
        } else if (osdFormat == VideoWidget::OSDFormat_Time) {
            text = now.toString("HH:mm:ss");
        } else if (osdFormat == VideoWidget::OSDFormat_DateTime) {
            text = now.toString("yyyy-MM-dd HH:mm:ss");
        }

        //设置颜色及字号
        QFont font;
        font.setPixelSize(osdFontSize);
        painter->setPen(osdColor);
        painter->setFont(font);

        painter->drawText(osdRect, flag, text);
    }

    painter->restore();
}

void VideoWidgetPrivate::initFlowStyle()
{
    //设置样式以便区分,可以自行更改样式,也可以不用样式
    QStringList qss;
    QString rgba = QString("rgba(%1,%2,%3,150)").arg(flowBgColor.red()).arg(flowBgColor.green()).arg(flowBgColor.blue());
    qss.append(QString("#flowPanel{background:%1;border:none;}").arg(rgba));
    qss.append(QString("QPushButton{border:none;padding:0px;background:rgba(0,0,0,0);}"));
    qss.append(QString("QPushButton:pressed{background:%1;}").arg(flowPressColor.name()));
    flowPanel->setStyleSheet(qss.join(""));
}

void VideoWidgetPrivate::updateImage(const QImage &image)
{
    QMutexLocker lock(&m_csImageLock);
    //拷贝图片有个好处,当处理器比较差的时候,图片不会产生断层,缺点是占用时间
    //默认QImage类型是浅拷贝,可能正在绘制的时候,那边已经更改了图片的上部分数据
    this->image = copyImage ? image.copy() : image;
    q->update();
}

bool VideoWidgetPrivate::getUrlRawData(uint8_t* buffer)
{
    memcpy(buffer, dropImage.bits(), dropImage.width() * dropImage.height() * (dropImage.format()==QImage::Format_RGB888?3:4));
    return true;
}

bool VideoWidgetPrivate::setUrlImage(const QString &url)
{
    dropImage = QImage(url);
    if(dropImage.isNull())
        return false;

    VideoWidget::CameraInfo camInfo;
    camInfo.width = dropImage.width();
    camInfo.height = dropImage.height();

    if(dropImage.format() == QImage::Format_RGB32 ||
            dropImage.format() == QImage::Format_ARGB32)
        camInfo.imageType = VideoWidget::ImageType_RGB32;
    else if(dropImage.format() == QImage::Format_RGB888)
        camInfo.imageType = VideoWidget::ImageType_RGB24;
    else
        return false;

    camInfo.rawdata_size = dropImage.width()*dropImage.height()*(dropImage.format()==QImage::Format_RGB888?3:4);
    camInfo.display_width = 1080;
    camInfo.display_height = 720;
    q->setCameraInfo(camInfo);
    q->setGetRawDataFunc(std::bind(&VideoWidgetPrivate::getUrlRawData, this, std::placeholders::_1));
    return true;
}

}
VideoWidget::VideoWidget(QWidget *parent) :
    QWidget(parent),
    d(new Internal::VideoWidgetPrivate(this, this))
{
    //设置强焦点
    setFocusPolicy(Qt::StrongFocus);
    //设置支持拖放
    setAcceptDrops(true);

    d->image = QImage();

    //顶部工具栏,默认隐藏,鼠标移入显示移除隐藏
    d->flowPanel = new QWidget(this);
    d->flowPanel->setObjectName("flowPanel");
    d->flowPanel->setVisible(false);

    //用布局顶住,左侧弹簧
    QHBoxLayout *layout = new QHBoxLayout;
    layout->setSpacing(2);
    layout->setMargin(0);
    layout->addStretch();
    d->flowPanel->setLayout(layout);

    //按钮集合名称,如果需要新增按钮则在这里增加即可
    QList<QString> btns;
    btns << "open" << "pause" << "stop";

    QList<QIcon> icons;
    icons << QApplication::style()->standardIcon(QStyle::SP_MediaPlay);
    icons << QApplication::style()->standardIcon(QStyle::SP_MediaPause);
    icons << QApplication::style()->standardIcon(QStyle::SP_MediaStop);

    //循环添加顶部按钮
    for (int i = 0; i < btns.count(); i++) {
        QPushButton *btn = new QPushButton;
        //绑定按钮单击事件,用来发出信号通知
        connect(btn, SIGNAL(clicked(bool)), this, SLOT(btnClicked()));
        //设置标识,用来区别按钮
        btn->setObjectName(btns.at(i));
        //设置固定宽度
        btn->setFixedWidth(d->iconSize + 5);
        //设置拉伸策略使得填充
        btn->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
        //设置焦点策略为无焦点,避免单击后焦点跑到按钮上
        btn->setFocusPolicy(Qt::NoFocus);
        //设置图标大小和图标
        btn->setIconSize(QSize(d->iconSize, d->iconSize));
        btn->setIcon(icons.at(i));

        //将按钮加到布局中
        layout->addWidget(btn);
    }

    d->initFlowStyle();

    connect(d->m_videoProcess.get(), &Internal::VideoProcess::updateFps, [&](const QString &fps){
        d->fpsText = fps;
    });
}

VideoWidget::~VideoWidget()
{
    close();
}

void VideoWidget::btnClicked()
{
    QPushButton *btn = (QPushButton *)sender();
    emit btnClicked(btn->objectName());
}

void VideoWidget::updateImage(const QImage &image)
{
    d->updateImage(image);
}

void VideoWidget::setCameraInfo(const CameraInfo &cameraInfo)
{
    d->m_videoProcess->setCameraInfo(cameraInfo);
}

void VideoWidget::setGetRawDataFunc(std::function<bool (uint8_t *)> getRawData)
{
    d->m_videoProcess->setGetRawDataFunc(getRawData);
}

void VideoWidget::setDecode2RgbFunc(std::function<bool (uint8_t *, uint8_t *)> decode2Rgb)
{
    d->m_videoProcess->setDecode2RgbFunc(decode2Rgb);
}

void VideoWidget::resizeEvent(QResizeEvent *)
{
    //flowPanel->setGeometry(borderWidth, borderWidth, this->width() - (borderWidth * 2), height);
    d->flowPanel->setGeometry(d->borderWidth, this->height() - d->panelHeight - d->borderWidth, this->width() - (d->borderWidth * 2), d->panelHeight);
}

void VideoWidget::enterEvent(QEvent *)
{
    //这里还可以增加一个判断,是否获取了焦点的才需要显示
    //if (this->hasFocus()) {}
    if (d->flowEnable) {
        d->flowPanel->setVisible(true);
    }
}

void VideoWidget::leaveEvent(QEvent *)
{
    if (d->flowEnable) {
        d->flowPanel->setVisible(false);
    }
}

void VideoWidget::dropEvent(QDropEvent *event)
{
    //拖放完毕鼠标松开的时候执行
    //判断拖放进来的类型,取出文件,进行播放
    if(event->mimeData()->hasUrls()) {
        QString url = event->mimeData()->urls().first().toLocalFile();
        if(d->setUrlImage(url))
        {
            this->restart();
        }
        emit fileDrag(url);
    }
}

void VideoWidget::dragEnterEvent(QDragEnterEvent *event)
{
    //拖曳进来的时候先判断下类型,非法类型则不处理
    if(event->mimeData()->hasFormat("text/uri-list")) {
        event->setDropAction(Qt::LinkAction);
        event->accept();
    } else {
        event->ignore();
    }
}

void VideoWidget::paintEvent(QPaintEvent *)
{
    //如果不需要绘制
    if (!d->drawImage) {
        return;
    }

    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing);

    //绘制边框
    d->drawBorder(&painter);

    if (!d->image.isNull())
    {
        //绘制背景图片
        d->drawImg(&painter, d->image);

        //绘制标签1
        if (d->fpsVisible) {
            d->drawOSD(&painter, d->fpsFontSize, d->fpsText, d->fpsColor, QImage(), OSDFormat_Text, d->fpsPosition);
        }

        //绘制标签1
        if (d->osd1Visible) {
            d->drawOSD(&painter, d->osd1FontSize, d->osd1Text, d->osd1Color, d->osd1Image, d->osd1Format, d->osd1Position);
        }

        //绘制标签2
        if (d->osd2Visible) {
            d->drawOSD(&painter, d->osd2FontSize, d->osd2Text, d->osd2Color, d->osd2Image, d->osd2Format, d->osd2Position);
        }

        d->m_drawHelper.SetVirtualXandY(getScaleVirtualX(), getScaleVirtualY());
        d->m_drawHelper.SetVirtualWidthAndHeight(getScaleVirtualWidth(), getScaleVirtualHeight());
        d->m_drawHelper.DrawInfo(&painter);
    } else {
        //绘制背景
        d->drawBg(&painter);
    }
}

QImage VideoWidget::getImage() const
{
    return d->image;
}

QString VideoWidget::getUrl() const
{
    return this->property("url").toString();
}

bool VideoWidget::getDrawImage() const
{
    return d->drawImage;
}

bool VideoWidget::getFillImage() const
{
    return d->fillImage;
}

bool VideoWidget::getFlowEnable() const
{
    return d->flowEnable;
}

QColor VideoWidget::getFlowBgColor() const
{
    return d->flowBgColor;
}

QColor VideoWidget::getFlowPressColor() const
{
    return d->flowPressColor;
}

int VideoWidget::getFlowHeight() const
{
    return d->panelHeight;
}

int VideoWidget::getBorderWidth() const
{
    return d->borderWidth;
}

QColor VideoWidget::getBorderColor() const
{
    return d->borderColor;
}

QColor VideoWidget::getFocusColor() const
{
    return d->focusColor;
}

QString VideoWidget::getBgText() const
{
    return d->bgText;
}

QImage VideoWidget::getBgImage() const
{
    return d->bgImage;
}

bool VideoWidget::getFpsVisible() const
{
    return d->fpsVisible;
}

int VideoWidget::getFpsFontSize() const
{
    return d->fpsFontSize;
}

QString VideoWidget::getFpsText() const
{
    return d->fpsText;
}

QColor VideoWidget::getFpsColor() const
{
    return d->fpsColor;
}

VideoWidget::OSDPosition VideoWidget::getFpsPosition() const
{
    return d->fpsPosition;
}

bool VideoWidget::getOSD1Visible() const
{
    return d->osd1Visible;
}

int VideoWidget::getOSD1FontSize() const
{
    return d->osd1FontSize;
}

QString VideoWidget::getOSD1Text() const
{
    return d->osd1Text;
}

QColor VideoWidget::getOSD1Color() const
{
    return d->osd1Color;
}

QImage VideoWidget::getOSD1Image() const
{
    return d->osd1Image;
}

VideoWidget::OSDFormat VideoWidget::getOSD1Format() const
{
    return d->osd1Format;
}

VideoWidget::OSDPosition VideoWidget::getOSD1Position() const
{
    return d->osd1Position;
}

bool VideoWidget::getOSD2Visible() const
{
    return d->osd2Visible;
}

int VideoWidget::getOSD2FontSize() const
{
    return d->osd2FontSize;
}

QString VideoWidget::getOSD2Text() const
{
    return d->osd2Text;
}

QColor VideoWidget::getOSD2Color() const
{
    return d->osd2Color;
}

QImage VideoWidget::getOSD2Image() const
{
    return d->osd2Image;
}

VideoWidget::OSDFormat VideoWidget::getOSD2Format() const
{
    return d->osd2Format;
}

VideoWidget::OSDPosition VideoWidget::getOSD2Position() const
{
    return d->osd2Position;
}

double VideoWidget::getScaleVirtualX() const
{
    return 0;
}

double VideoWidget::getScaleVirtualY() const
{
    return 0;
}

double VideoWidget::getScaleVirtualWidth() const
{
    return 1.0*d->m_videoProcess->getCameraInfo().width;
}

double VideoWidget::getScaleVirtualHeight() const
{
    return 1.0*d->m_videoProcess->getCameraInfo().height;
}

QSize VideoWidget::sizeHint() const
{
    return QSize(500, 350);
}

QSize VideoWidget::minimumSizeHint() const
{
    return QSize(50, 35);
}



void VideoWidget::setSaveFile(bool saveFile)
{

}

void VideoWidget::setSaveInterval(int saveInterval)
{

}

void VideoWidget::setSavePath(const QString &savePath)
{
    //如果目录不存在则新建
    QDir dir(savePath);
    if (!dir.exists()) {
        dir.mkdir(savePath);
    }


}

void VideoWidget::setFileName(const QString &fileName)
{

}

void VideoWidget::setDrawImage(bool drawImage)
{
    d->drawImage = drawImage;
}

void VideoWidget::setFillImage(bool fillImage)
{
    d->fillImage = fillImage;
}

void VideoWidget::setFlowEnable(bool flowEnable)
{
    d->flowEnable = flowEnable;
}

void VideoWidget::setFlowBgColor(const QColor &flowBgColor)
{
    if (d->flowBgColor != flowBgColor) {
        d->flowBgColor = flowBgColor;
        d->initFlowStyle();
    }
}

void VideoWidget::setFlowPressColor(const QColor &flowPressColor)
{
    if (d->flowPressColor != flowPressColor) {
        d->flowPressColor = flowPressColor;
        d->initFlowStyle();
    }
}

void VideoWidget::setFlowHeight(int height)
{
    d->panelHeight = height;
}

void VideoWidget::setBorderWidth(int borderWidth)
{
    d->borderWidth = borderWidth;
}

void VideoWidget::setBorderColor(const QColor &borderColor)
{
    d->borderColor = borderColor;
}

void VideoWidget::setFocusColor(const QColor &focusColor)
{
    d->focusColor = focusColor;
}

void VideoWidget::setBgText(const QString &bgText)
{
    d->bgText = bgText;
}

void VideoWidget::setBgImage(const QImage &bgImage)
{
    d->bgImage = bgImage;
}

void VideoWidget::setFpsVisible(bool fpsVisible)
{
    d->fpsVisible = fpsVisible;
}

void VideoWidget::setFpsFontSize(int fpsFontSize)
{
    d->fpsFontSize = fpsFontSize;
}

void VideoWidget::setFpsColor(const QColor &fpsColor)
{
    d->fpsColor = fpsColor;
}

void VideoWidget::setFpsPosition(const OSDPosition &fpsPosition)
{
    d->fpsPosition = fpsPosition;
}

void VideoWidget::setOSD1Visible(bool osdVisible)
{
    d->osd1Visible = osdVisible;
}

void VideoWidget::setOSD1FontSize(int osdFontSize)
{
    d->osd1FontSize = osdFontSize;
}

void VideoWidget::setOSD1Text(const QString &osdText)
{
    d->osd1Text = osdText;
}

void VideoWidget::setOSD1Color(const QColor &osdColor)
{
    d->osd1Color = osdColor;
}

void VideoWidget::setOSD1Image(const QImage &osdImage)
{
    d->osd1Image = osdImage;
}

void VideoWidget::setOSD1Format(const VideoWidget::OSDFormat &osdFormat)
{
    d->osd1Format = osdFormat;
}

void VideoWidget::setOSD1Position(const VideoWidget::OSDPosition &osdPosition)
{
    d->osd1Position = osdPosition;
}

void VideoWidget::setOSD2Visible(bool osdVisible)
{
    d->osd2Visible = osdVisible;
}

void VideoWidget::setOSD2FontSize(int osdFontSize)
{
    d->osd2FontSize = osdFontSize;
}

void VideoWidget::setOSD2Text(const QString &osdText)
{
    d->osd2Text = osdText;
}

void VideoWidget::setOSD2Color(const QColor &osdColor)
{
    d->osd2Color = osdColor;
}

void VideoWidget::setOSD2Image(const QImage &osdImage)
{
    d->osd2Image = osdImage;
}

void VideoWidget::setOSD2Format(const VideoWidget::OSDFormat &osdFormat)
{
    d->osd2Format = osdFormat;
}

void VideoWidget::setOSD2Position(const VideoWidget::OSDPosition &osdPosition)
{
    d->osd2Position = osdPosition;
}

bool VideoWidget::open()
{
    if(d->m_videoProcess->runMode() == RunMode_Stop)
        if(!d->m_videoProcess->initImage())
            return false;

    if(!d->m_videoProcess->openImage())
        return false;

    return true;
}

bool VideoWidget::pause()
{
    if(d->m_videoProcess->runMode() == RunMode_Stop)
        return false;

    if(!d->m_videoProcess->pauseImage())
        return false;

    return true;
}

bool VideoWidget::next()
{
    if(d->m_videoProcess->runMode() == RunMode_Stop)
        return false;

    if(!d->m_videoProcess->openImage())
        return false;

    return true;
}

bool VideoWidget::close()
{
    if(!d->m_videoProcess->closeImage())
        return false;

    if(!d->m_videoProcess->freeImage())
        return false;

    d->updateImage(QImage());

    return true;
}

bool VideoWidget::restart()
{
    if(!close())
        return false;

    if(!open())
        return false;

    return true;
}

void VideoWidget::addRect(int x, int y, int Width, int Height, QColor itemColor, float fLineWidth, RectType tRectType, const QString& text, QColor FontColor)
{
    d->m_drawHelper.Add_Rect(x,y,Width,Height,itemColor,fLineWidth,(CDrawInfoItemRectType)tRectType,text,FontColor);
}

void VideoWidget::addCenterRect(int CenterX, int CenterY, int Width, int Height, QColor itemColor, float fLineWidth, RectType tRectType, const QString& text, QColor FontColor)
{
    d->m_drawHelper.Add_CenterRect(CenterX,CenterY,Width,Height,itemColor,fLineWidth,(CDrawInfoItemRectType)tRectType,text,FontColor);
}

void VideoWidget::addLine(int x1, int y1, int x2, int y2, QColor itemColor, float fLineWidth, const QString& text, QColor FontColor)
{
    d->m_drawHelper.Add_Line(x1,y1,x2,y2,itemColor,fLineWidth,text,FontColor);
}

void VideoWidget::addString(int x, int y, const QString& text, QColor FontColor, float fFontSize)
{
    d->m_drawHelper.Add_String(x,y,text,FontColor,fFontSize);
}

void VideoWidget::resetItem()
{
    d->m_drawHelper.ResetItem();
}

}

#include "videowidget.moc"
