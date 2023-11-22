#include "videoprocess.h"

#include <QImage>
#include <QtConcurrent/QtConcurrent>
#include <QElapsedTimer>
#include <QThread>

#ifndef SAFE_FREE
#define SAFE_FREE(p)			{	if(p) { free(p);						(p)=NULL; }	}
#endif

namespace EmosWidgets {
namespace Internal {
VideoProcess::VideoProcess(VideoWidget* videoWidget) :
    m_videoWidget(videoWidget)
{

}

bool VideoProcess::initImage()
{
    QMutexLocker lock(&m_csLockVideo);
    uint64_t nCameraSize = m_cameraInfo.rawdata_size;
    uint64_t nProcessSize = m_cameraInfo.width * m_cameraInfo.height * m_bitSize;
    uint64_t nDisPlaySize = m_cameraInfo.display_width * m_cameraInfo.display_height * m_bitSize;

    if (!m_pCameraBuffer)
        m_pCameraBuffer = (uint8_t *)malloc(nCameraSize);
    if (!m_pTripleBuffer)
        m_pTripleBuffer = (uint8_t *)malloc(nCameraSize);
    if (!m_pCameraBufferSET)
        m_pCameraBufferSET = (uint8_t *)malloc(nCameraSize);
    if (!m_pDisplayBuffer)
        m_pDisplayBuffer = (uint8_t *)malloc(nProcessSize);
    if (m_useDisplay && !m_pDisplayBufferSET)
        m_pDisplayBufferSET = (uint8_t *)malloc(nDisPlaySize);

    if (!m_pCameraBuffer ||
        !m_pTripleBuffer ||
        !m_pCameraBufferSET ||
        !m_pDisplayBuffer||
        (m_useDisplay && !m_pDisplayBufferSET))
    {
        return false;
    }

    memset(m_pCameraBuffer, 0, nCameraSize);
    memset(m_pTripleBuffer, 0, nCameraSize);
    memset(m_pCameraBufferSET, 0, nCameraSize);
    memset(m_pDisplayBuffer, 0, nProcessSize);
    if(m_useDisplay)
        memset(m_pDisplayBufferSET, 0, nDisPlaySize);

    return true;
}

void VideoProcess::setCameraInfo(const VideoWidget::CameraInfo &cameraInfo)
{
    m_cameraInfo = cameraInfo;
    m_useDisplay = true;
    if(m_cameraInfo.display_width == 0 || m_cameraInfo.display_height == 0)
    {
        m_cameraInfo.display_width = m_cameraInfo.width;
        m_cameraInfo.display_height = m_cameraInfo.height;
        m_useDisplay = false;
    }
    switch (m_cameraInfo.imageType) {
    case VideoWidget::ImageType_RGB24:
        {
            m_bitSize = 3;
            m_format = QImage::Format_RGB888;
            break;
        }
    case VideoWidget::ImageType_RGB32:
    default:
        {
            m_bitSize = 4;
            m_format = QImage::Format_RGB32;
            break;
        }
    }
}

void VideoProcess::setGetRawDataFunc(std::function<bool (uint8_t *)> getRawData)
{
    m_getRawData = getRawData;
}

void VideoProcess::setDecode2RgbFunc(std::function<bool (uint8_t *, uint8_t *)> decode2Rgb)
{
    m_decode2Rgb = decode2Rgb;
}

void VideoProcess::showImage(bool show)
{
    m_showImage = show;
}

VideoWidget::RunMode VideoProcess::runMode() const
{
    return m_RunMode;
}

VideoWidget::CameraInfo VideoProcess::getCameraInfo() const
{
    return m_cameraInfo;
}

bool VideoProcess::openImage()
{
    QMutexLocker lock(&m_csLockVideo);
    if (m_RunMode==VideoWidget::RunMode::RunMode_Play)
        return true;

    if (m_RunMode==VideoWidget::RunMode::RunMode_Stop)
    {
        m_RunMode = VideoWidget::RunMode::RunMode_Play;
        m_CameraThreadRunMode = VideoWidget::RunMode::RunMode_Play;
        m_ProcessThreadRunMode = VideoWidget::RunMode::RunMode_Play;
        m_DisplayThreadRunMode = VideoWidget::RunMode::RunMode_Play;

        QtConcurrent::run(this, &VideoProcess::cameraThread);
        QtConcurrent::run(this, &VideoProcess::processthread);
        QtConcurrent::run(this, &VideoProcess::displayThread);
    }
    else if (m_RunMode==VideoWidget::RunMode::RunMode_Pause)
    {
        m_RunMode = VideoWidget::RunMode::RunMode_Play;
        m_CameraThreadRunMode = VideoWidget::RunMode::RunMode_Play;
        m_ProcessThreadRunMode = VideoWidget::RunMode::RunMode_Play;
        m_DisplayThreadRunMode = VideoWidget::RunMode::RunMode_Play;
    }
    return true;
}

bool VideoProcess::pauseImage()
{
    QMutexLocker lock(&m_csLockVideo);
    if (m_RunMode==VideoWidget::RunMode::RunMode_Pause||m_RunMode==VideoWidget::RunMode::RunMode_Stop)
        return true;

    m_RunMode = VideoWidget::RunMode::RunMode_Pause;
    while (m_CameraThreadRunMode != VideoWidget::RunMode::RunMode_Pause) QThread::msleep(1);
    while (m_ProcessThreadRunMode != VideoWidget::RunMode::RunMode_Pause) QThread::msleep(1);
    while (m_DisplayThreadRunMode != VideoWidget::RunMode::RunMode_Pause) QThread::msleep(1);
    return true;
}

bool VideoProcess::closeImage()
{
    QMutexLocker lock(&m_csLockVideo);
    if (m_RunMode==VideoWidget::RunMode::RunMode_Stop)
        return true;

    m_RunMode = VideoWidget::RunMode::RunMode_Stop;
    while (m_CameraThreadRunMode != VideoWidget::RunMode::RunMode_Stop) QThread::msleep(1);
    while (m_ProcessThreadRunMode != VideoWidget::RunMode::RunMode_Stop) QThread::msleep(1);
    while (m_DisplayThreadRunMode != VideoWidget::RunMode::RunMode_Stop) QThread::msleep(1);
    return true;
}

bool VideoProcess::freeImage()
{
    QMutexLocker lock(&m_csLockVideo);
    SAFE_FREE(m_pTripleBuffer);
    SAFE_FREE(m_pCameraBuffer);
    SAFE_FREE(m_pDisplayBuffer);
    SAFE_FREE(m_pCameraBufferSET);
    SAFE_FREE(m_pDisplayBufferSET);
    return true;
}

bool VideoProcess::tripleBufferFlip(uint8_t **pBuf)
{
    QMutexLocker lock(&m_csLock);
    {
        uint8_t *pTmp = *pBuf;
        *pBuf = m_pTripleBuffer;
        m_pTripleBuffer = pTmp;
    }
    return true;
}


void VideoProcess::cameraThread()
{
    QElapsedTimer tickTimeHelper;
    uint64_t FrameNum = 0;
    uint64_t buffersize = m_cameraInfo.rawdata_size;

    tickTimeHelper.start();
    QString strCameraInfoTemp;
    while (m_RunMode != VideoWidget::RunMode::RunMode_Stop)
    {
        m_fps.sprintf("fps : grab:%0.2f,process:%0.2f,display:%0.2f", m_fpsFrame, m_fpsProcess, m_fpsDisplay);
        emit updateFps(m_fps);

        if (m_RunMode == VideoWidget::RunMode::RunMode_Pause)
        {
            QThread::msleep(5);
            if (m_RunMode == VideoWidget::RunMode::RunMode_Pause)
                m_CameraThreadRunMode = VideoWidget::RunMode::RunMode_Pause;
            continue;
        }

        if(!m_getRawData)
        {
            QThread::msleep(5);
            continue;
        }
        if (m_getRawData(m_pCameraBuffer))
        {
            FrameNum++;

//            if (m_bWaitFPSFlag)
//            {
//                m_bWaitFPSFlag = false;
//            }
//            if (m_bCopyFlag)
//            {
//                memcpy(m_pCameraCopyBuffer, m_pCameraBuffer, buffersize);
//                m_bCopyFlag = false;
//                if (m_bShowImage && !m_bLivePhoto)
//                    TripleBufferFlip(&m_pCameraBuffer, true);
//            }
            if (m_showImage)
                tripleBufferFlip(&m_pCameraBuffer);

            if (FrameNum % 10 == 0 && FrameNum != 0)
            {
                m_fpsFrame = (float)(10000.0 / tickTimeHelper.elapsed());
                tickTimeHelper.start();
            }
            QThread::msleep(m_cameraInfo.Sleep1);
        }
    }
    m_CameraThreadRunMode = VideoWidget::RunMode::RunMode_Stop;
    return;
}

void VideoProcess::processthread()
{
    QElapsedTimer tickTimeHelper;
    uint64_t FrameNum = 0;
    int width = m_cameraInfo.width;
    int height = m_cameraInfo.height;

    tickTimeHelper.start();
    while (m_RunMode != VideoWidget::RunMode::RunMode_Stop)
    {
        if (m_RunMode == VideoWidget::RunMode::RunMode_Pause)
        {
            QThread::msleep(5);
            if (m_RunMode == VideoWidget::RunMode::RunMode_Pause)
                m_ProcessThreadRunMode = VideoWidget::RunMode::RunMode_Pause;
            continue;
        }
        if (tripleBufferFlip(&m_pCameraBufferSET))
        {
            if (m_RunMode != VideoWidget::RunMode::RunMode_Play)
            {
                QThread::msleep(5);
                continue;
            }

            if(!m_decode2Rgb)
            {
                memcpy(m_pDisplayBuffer, m_pCameraBufferSET, m_cameraInfo.rawdata_size);
            }
            else
            {
                m_decode2Rgb(m_pCameraBufferSET, m_pDisplayBuffer);
            }

            if(m_useDisplay)
            {
                imageZoom(m_pDisplayBuffer, m_cameraInfo.width, m_cameraInfo.height,
                          m_pDisplayBufferSET, m_cameraInfo.display_width, m_cameraInfo.display_height, m_bitSize);
            }

            m_updateDisplay = true;
            FrameNum++;
            if (FrameNum % 10 == 0)
            {
                m_fpsProcess = (float)(10000.0 / tickTimeHelper.elapsed());
                FrameNum = 0;
                tickTimeHelper.start();
            }
        }
        else
        {
            QThread::msleep(5);
            continue;
        }
        QThread::msleep(m_cameraInfo.Sleep2);
    }

    m_ProcessThreadRunMode = VideoWidget::RunMode::RunMode_Stop;
    return;
}

void VideoProcess::displayThread()
{
    QElapsedTimer tickTimeHelper;
    uint64_t FrameNum = 0;
    tickTimeHelper.start();

    while (m_RunMode != VideoWidget::RunMode::RunMode_Stop)
    {
        if (m_RunMode == VideoWidget::RunMode::RunMode_Pause)
        {
            QThread::msleep(5);
            if (m_RunMode == VideoWidget::RunMode::RunMode_Pause)
                m_DisplayThreadRunMode = VideoWidget::RunMode::RunMode_Pause;
            continue;
        }

        if (m_updateDisplay)
        {
            //m_updateDisplay = false;

            if(m_useDisplay)
            {
                QImage classImage(m_pDisplayBufferSET, m_cameraInfo.display_width, m_cameraInfo.display_height, m_format);
                m_videoWidget->updateImage(classImage);
                classImage.detach();
            }
            else
            {
                QImage classImage(m_pDisplayBuffer, m_cameraInfo.display_width, m_cameraInfo.display_height, m_format);
                m_videoWidget->updateImage(classImage);
                classImage.detach();
            }

            FrameNum++;
            if (FrameNum % 10 == 0)
            {
                m_fpsDisplay = (float)(10000.0 / tickTimeHelper.elapsed());
                FrameNum = 0;
                tickTimeHelper.start();
            }
        }
        else
        {
            QThread::msleep(5);
            continue;
        }
        QThread::msleep(m_cameraInfo.Sleep3);
    }

    m_DisplayThreadRunMode = VideoWidget::RunMode::RunMode_Stop;
    return;
}

bool VideoProcess::imageZoom(uint8_t* src, uint64_t src_width, uint64_t src_height, uint8_t* dst, uint64_t dst_width, uint64_t dst_height, int bit)
{
    unsigned int src_LineBytes = src_width * bit; //一行的字节数
    unsigned int dst_LineBytes = dst_width * bit;  //一行的字节数

    unsigned long dwDstWidth = dst_width;
    unsigned long* pdwSrcXTable;
    unsigned long x;
    unsigned long y;
    unsigned long dwSrcY;
    unsigned char *pucDest;
    unsigned char *pucSrc;
    unsigned long dwPixelBytes = bit; //像素字节
    pdwSrcXTable = (unsigned long*)malloc(sizeof(unsigned long) * dwDstWidth);
    if(NULL==pdwSrcXTable)
    {
        return -1;
    }

    for(x=0; x < dwDstWidth; x++)//生成表 pdwSrcXTable
    {
        pdwSrcXTable[x]=(x*src_width/dst_width);
    }

    for(y=0; y < dst_height; y++)
    {
        dwSrcY=(y * src_height/dst_height);

        pucDest = dst + y * dst_LineBytes;
        pucSrc = src + dwSrcY * src_LineBytes;

        for(x=0; x < dwDstWidth; x++)
        {
            line_memcpy(pucDest+x * dwPixelBytes, pucSrc + pdwSrcXTable[x] * dwPixelBytes, dwPixelBytes);
        }
    }

    free(pdwSrcXTable);
    return 0;
}

/*
函数功能：内存拷贝函数
*/
void *VideoProcess::line_memcpy(void *v_dst,const void *v_src,unsigned char c)
{
    const char *src = (const char *)v_src;
    char *dst = (char *)v_dst;
    while(c--)*dst++=*src++;
    return v_dst;
}
}
}
