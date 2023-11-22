#ifndef VIDEOPROCESS_H
#define VIDEOPROCESS_H

#include <QObject>
#include <QMutex>
#include "videowidget.h"

namespace EmosWidgets {
class VideoWidget;
namespace Internal {
class VideoProcess : public QObject
{
    Q_OBJECT
public:
    VideoProcess(VideoWidget* videoWidget);

    bool initImage();
    bool openImage();
    bool pauseImage();
    bool closeImage();
    bool freeImage();

    void setCameraInfo(const VideoWidget::CameraInfo& cameraInfo);
    void setGetRawDataFunc(std::function<bool(uint8_t*/* buffer*/)> getRawData);
    void setDecode2RgbFunc(std::function<bool(uint8_t*/* in buffer*/, uint8_t*/* out buffer*/)> decode2Rgb);

    /**/
    void showImage(bool show);

    VideoWidget::RunMode runMode() const;
    VideoWidget::CameraInfo getCameraInfo() const;

signals:
    void updateFps(const QString &fps);
private:
    void cameraThread();
    void processthread();
    void displayThread();

    bool tripleBufferFlip(uint8_t **pBuf);
    bool imageZoom(uint8_t* src, uint64_t src_width, uint64_t src_height, uint8_t* dst, uint64_t dst_width, uint64_t dst_height, int bit);
    void *line_memcpy(void *v_dst,const void *v_src,unsigned char c);
    QMutex m_csLock;              //use for display update data

    uint8_t* m_pCameraBuffer    =nullptr;   //camera buffer.
    uint8_t* m_pTripleBuffer    =nullptr;   //buffer between camera and process
    uint8_t* m_pCameraBufferSET =nullptr;   //camera buffer.
    uint8_t* m_pDisplayBuffer   =nullptr;   //display buffer
    uint8_t* m_pDisplayBufferSET   =nullptr;   //display buffer

    VideoWidget::RunMode m_RunMode = VideoWidget::RunMode::RunMode_Stop;//运行模式
    VideoWidget::RunMode m_CameraThreadRunMode =VideoWidget::RunMode::RunMode_Stop; //thread for video
    VideoWidget::RunMode m_ProcessThreadRunMode=VideoWidget::RunMode::RunMode_Stop;//thread for process
    VideoWidget::RunMode m_DisplayThreadRunMode=VideoWidget::RunMode::RunMode_Stop;//thread for display
    float m_fpsFrame=0;
    float m_fpsDisplay=0;
    float m_fpsProcess=0;
    QString m_fps;
    QMutex m_csLockVideo{QMutex::Recursive};//控制图像的互斥量
    bool m_updateDisplay = false;
    bool m_useDisplay = false; //是否改变显示大小
    int m_bitSize = 4;
    QImage::Format m_format = QImage::Format_RGB32;

    VideoWidget::CameraInfo m_cameraInfo;
    std::function<bool(uint8_t*/* buffer*/)> m_getRawData = nullptr;
    std::function<bool(uint8_t*/* in buffer*/, uint8_t*/* out buffer*/)> m_decode2Rgb = nullptr;
    bool m_showImage = true;//图像显示标志位

    VideoWidget* m_videoWidget;
};
}}
#endif // VIDEOPROCESS_H
