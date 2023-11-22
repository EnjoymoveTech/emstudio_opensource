#include "DrawInfoHelper.h"
#include <QApplication>
#include <QFont>
#include <math.h>

namespace EmosWidgets {

#ifndef SAFE_DELETE_TOOL
#ifndef SAFE_DELETE
#define SAFE_DELETE(p)			{	if(p) { delete (p);						(p)=NULL; }	}
#endif
#ifndef SAFE_FREE
#define SAFE_FREE(p)			{	if(p) { free(p);						(p)=NULL; }	}
#endif
#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p)			{	if(p) { (p)->Release();					(p)=NULL; }	}
#endif
#define SAFE_DELETE_ARRAY(p)	{	if(p) { delete [] (p);					(p)=NULL; }	}
#define SAFE_CLOSE_HANDLE(p)	{	if(p) { ::CloseHandle(p);				(p)=NULL; }	}
#define SAFE_CLOSE_FILE(p)		{	if(p) { fclose(p);						(p)=NULL; }	}
#define SAFE_DELETE_OBJECT(p)	{	if(p) { ::DeleteObject((HGDIOBJ) p);	(p)=NULL; }	}
#define SAFE_FREE_LIBRARY(p)	{	if(p) { FreeLibrary(p);					(p)=NULL; }	}
#endif	/* SAFE_DELETE_TOOL */

#define sqr(x) (x*x)

CDrawInfoHelper::CDrawInfoHelper()
{
}

CDrawInfoHelper::~CDrawInfoHelper()
{
    ResetItem();
}

void CDrawInfoHelper::SetVirtualXandY(double iVirtualX, double iVirtualY)
{
    m_dVirtualX=iVirtualX;
    m_dVirtualY=iVirtualY;
}

void CDrawInfoHelper::SetVirtualWidthAndHeight(double iVirtualWidth, double iVirtualHeight)
{
    m_dVirtualWidth=iVirtualWidth;
    m_dVirtualHeight=iVirtualHeight;
}

void CDrawInfoHelper::SetVirtualFullWidthAndHeight(double iVirtualFullWidth, double iVirtualFullHeight)
{
    m_dVirtualFullWidth=iVirtualFullWidth;
    m_dVirtualFullHeight=iVirtualFullHeight;
}

bool CDrawInfoHelper::Add_Rect(int x, int y, int Width, int Height, QColor itemColor /*= QColor(125, 125, 125)*/, float fLineWidth /*= 1*/, CDrawInfoItemRectType tRectType, QString text /*= ""*/, QColor FontColor)
{
    CDrawInfoItemBase *item=Create_Rect(x,y,Width,Height,itemColor,fLineWidth,tRectType,text,FontColor);
    return AddItem(item);
}

CDrawInfoHelper::CDrawInfoItemBase * CDrawInfoHelper::Create_Rect(int x, int y, int Width, int Height, QColor itemColor /*= QColor(125, 125, 125)*/, float fLineWidth/*=1*/, CDrawInfoItemRectType tRectType, QString text /*= ""*/, QColor FontColor)
{
    CDrawInfoItemBase *item=new CDrawInfoHelper::CDrawInfoItemRect();
    Set_Rect(item,x,y,Width,Height,itemColor,fLineWidth,tRectType,text,FontColor);
    return item;
}

bool CDrawInfoHelper::Set_Rect(CDrawInfoItemBase *item, int x, int y, int Width, int Height, QColor itemColor /*= QColor(125, 125, 125)*/, float fLineWidth /*= 1*/, CDrawInfoItemRectType tRectType, QString text /*= ""*/, QColor FontColor)
{
    CDrawInfoItemRect *pRect = dynamic_cast<CDrawInfoItemRect *>(item);
    if (NULL != pRect)
    {
        pRect->x = x;
        pRect->y = y;
        pRect->Width = Width;
        pRect->Height = Height;
        pRect->itemColor = itemColor;
        pRect->fLineWidth = fLineWidth;
        pRect->tRectType = tRectType;
        pRect->strString = text;
        pRect->FontColor = FontColor;
        return true;
    }
    return false;
}

bool CDrawInfoHelper::Add_Line(int x1, int y1, int x2, int y2, QColor itemColor /*= QColor(125, 125, 125)*/, float fLineWidth /*= 1*/, QString text /*= ""*/, QColor FontColor)
{
    CDrawInfoItemBase *item=Create_Line(x1, y1, x2, y2, itemColor, fLineWidth, text, FontColor);
    return AddItem(item);
}

CDrawInfoHelper::CDrawInfoItemBase * CDrawInfoHelper::Create_Line(int x1, int y1, int x2, int y2, QColor itemColor /*= QColor(125, 125, 125)*/, float fLineWidth /*= 1*/, QString text /*= ""*/, QColor FontColor)
{
    CDrawInfoItemBase *item=new CDrawInfoHelper::CDrawInfoItemLine();
    Set_Line(item,x1, y1, x2, y2, itemColor, fLineWidth, text, FontColor);
    return item;
}

bool CDrawInfoHelper::Set_Line(CDrawInfoItemBase *item, int x1, int y1, int x2, int y2, QColor itemColor /*= QColor(125, 125, 125)*/, float fLineWidth /*= 1*/, QString text /*= ""*/, QColor FontColor)
{
    CDrawInfoItemLine *pRect = dynamic_cast<CDrawInfoItemLine *>(item);
    if (NULL != pRect)
    {
        pRect->x = x1;
        pRect->y = y1;
        pRect->x2 = x2;
        pRect->y2 = y2;
        pRect->itemColor = itemColor;
        pRect->fLineWidth = fLineWidth;
        pRect->strString = text;
        pRect->FontColor = FontColor;
        return true;
    }
    return false;
}

bool CDrawInfoHelper::Add_String(int x, int y, QString text, QColor FontColor /*= QColor(255, 255, 255)*/, float fFontSize /*= 0*/)
{
    CDrawInfoHelper::CDrawInfoItemString *item = Create_String(x, y, text, FontColor, fFontSize);
    return AddItem(item);
}

CDrawInfoHelper::CDrawInfoItemString * CDrawInfoHelper::Create_String(int x, int y, QString text, QColor FontColor /*= QColor(255, 255, 255)*/, float fFontSize /*= 0*/)
{
    CDrawInfoHelper::CDrawInfoItemString *item = new CDrawInfoHelper::CDrawInfoItemString();
    Set_String(item, x, y, text, FontColor, fFontSize);
    return item;
}

bool CDrawInfoHelper::Set_String(CDrawInfoItemBase *item, int x, int y, QString text, QColor FontColor /*= QColor(255, 255, 255)*/, float fFontSize /*= 0*/)
{
    CDrawInfoItemString *pString = dynamic_cast<CDrawInfoItemString *>(item);
    if (NULL != pString)
    {
        pString->x = x;
        pString->y = y;
        pString->strString = text;
        pString->FontColor = FontColor;
        pString->fFontSize = fFontSize;
        return true;
    }
    return false;
}

bool CDrawInfoHelper::Set_Rect(int index, int x, int y, int Width, int Height, QColor itemColor /*= QColor(125, 125, 125)*/, float fLineWidth /*= 1*/, CDrawInfoItemRectType tRectType, QString text /*= ""*/, QColor FontColor)
{
    int iIndex = index;
    QMutexLocker lock(&m_csLock);
    CDrawInfoItemBase *item = m_mMap[iIndex];
    if(item!=nullptr && item->type!=CDrawInfoItemBaseType::Rect)
        SAFE_DELETE(item);

    if (item==nullptr)
        return SetItem(iIndex,Create_Rect(x, y, Width, Height, itemColor, fLineWidth, tRectType, text, FontColor));
    else
        return Set_Rect(item,x, y, Width, Height, itemColor, fLineWidth, tRectType, text, FontColor);
    return false;
}

bool CDrawInfoHelper::Set_Line(int index, int x1, int y1, int x2, int y2, QColor itemColor /*= QColor(125, 125, 125)*/, float fLineWidth /*= 1*/, QString text /*= ""*/, QColor FontColor)
{
    int iIndex = index;
    QMutexLocker lock(&m_csLock);
    CDrawInfoItemBase *item = m_mMap[iIndex];
    if(item!=nullptr && item->type!=CDrawInfoItemBaseType::Rect)
        SAFE_DELETE(item);

    if (item==nullptr)
        return SetItem(iIndex,Create_Line(x1, y1, x2, y2, itemColor, fLineWidth, text, FontColor));
    else
        return Set_Line(item,x1, y1, x2, y2, itemColor, fLineWidth, text, FontColor);
    return false;
}

bool CDrawInfoHelper::Set_String(int index, int x, int y, QString text, QColor FontColor /*= QColor(255, 255, 255)*/, float fFontSize /*= 0*/)
{
    int iIndex = index;
    QMutexLocker lock(&m_csLock);
    CDrawInfoItemBase *item = m_mMap[iIndex];
    if(item!=nullptr && item->type!=CDrawInfoItemBaseType::Rect)
        SAFE_DELETE(item);

    if (item==nullptr)
        return SetItem(iIndex,Create_String(x, y, text, FontColor, fFontSize));
    else
        return Set_String(item,x, y, text, FontColor, fFontSize);
    return false;
}

bool CDrawInfoHelper::AddItem(CDrawInfoItemBase *item)
{
    QMutexLocker lock(&m_csLock);
    if (m_iMapIndex >= INT_MAX)
    {
        SAFE_DELETE(item);
        return false;
    }
    m_mMap[m_iMapIndex]=item;
    m_iMapIndex++;
    return true;
}

bool CDrawInfoHelper::SetItem(int index, CDrawInfoItemBase *item)
{
    QMutexLocker lock(&m_csLock);
    if (index >=DrawItemIndexAdd)
    {
        SAFE_DELETE(item);
        return false;
    }
    m_mMap[index]=item;
    return true;
}

bool CDrawInfoHelper::DeleteItem(int index)
{
    QMutexLocker lock(&m_csLock);
    int iIndex = index;
    bool bFind = false;
    for (auto keyValue = m_mMap.begin(); keyValue != m_mMap.end(); keyValue++)
    {
        int Index = keyValue->first;
        if (Index == iIndex)
        {
            CDrawInfoItemBase *item = keyValue->second;
            SAFE_DELETE(item);
            m_mMap.erase(keyValue);
            bFind = true;
            break;
        }

    }
    return bFind;
}

bool CDrawInfoHelper::ResetItem()
{
    QMutexLocker lock(&m_csLock);
    for (auto keyValue : m_mMap)
    {
        CDrawInfoItemBase *item = keyValue.second;
        SAFE_DELETE(item);
    }
    m_mMap.clear();
    m_iMapIndex = DrawItemIndexAdd;
    return true;
}

bool CDrawInfoHelper::ResetAddItem()
{
    QMutexLocker lock(&m_csLock);
    for (auto keyValue = m_mMap.begin(); keyValue != m_mMap.end(); )
    {
        int Index = keyValue->first;
        if (Index >=DrawItemIndexAdd)
        {
            CDrawInfoItemBase *item = keyValue->second;
            SAFE_DELETE(item);
            keyValue=m_mMap.erase(keyValue);
        }
        else
            keyValue++;
    }
    m_iMapIndex = DrawItemIndexAdd;
    return true;
}

bool CDrawInfoHelper::ResetSetItem()
{
    QMutexLocker lock(&m_csLock);
    for (auto keyValue = m_mMap.begin(); keyValue != m_mMap.end(); )
    {
        int Index = keyValue->first;
        if (Index >= DrawItemIndexSet
            && Index < DrawItemIndexAdd)
        {
            CDrawInfoItemBase *item = keyValue->second;
            SAFE_DELETE(item);
            keyValue=m_mMap.erase(keyValue);
        }
        else
            keyValue++;

    }
    return true;
}

bool CDrawInfoHelper::DrawInfo(QPainter *painter)
{
    QMutexLocker lock(&m_csLock);

    if (m_dVirtualWidth==0 || m_dVirtualHeight ==0)
        return false;

    QRect clientRect=painter->window();
    int iClientRectWidth = clientRect.width();
    int iClientRectHeight = clientRect.height();
    int iClientRectxy = (int)sqrt(iClientRectWidth*iClientRectWidth + iClientRectHeight*iClientRectHeight);

    for (auto keyValue : m_mMap)
    {
        CDrawInfoItemBase *item = keyValue.second;

        painter->save();
        float iFontSize = 0;
        if (item->fFontSize == 0)
            iFontSize = 30;
        else
            iFontSize = item->fFontSize;

        iFontSize=iFontSize*iClientRectHeight/1500;

        CDrawInfoHelper::CDrawInfoItemRect *pRect = dynamic_cast<CDrawInfoHelper::CDrawInfoItemRect *>(item);
        if (NULL != pRect)
        {
            int iShowX = (pRect->x - m_dVirtualX)*iClientRectWidth / m_dVirtualWidth;//显示的X坐标
            int iShowY = (pRect->y - m_dVirtualY)*iClientRectHeight / m_dVirtualHeight;//显示的Y坐标
            int iShowWidth = pRect->Width*iClientRectWidth / m_dVirtualWidth;//显示的宽度
            int iShowHeight = pRect->Height*iClientRectHeight / m_dVirtualHeight;//显示的长度
            if (iShowWidth == 0 && pRect->Width != 0)
                iShowWidth = 1;
            if (iShowHeight == 0 && pRect->Height != 0)
                iShowHeight = 1;
            QRect rect(iShowX, iShowY, iShowWidth, iShowHeight);

            QColor color(item->itemColor);
            QPen pen;
            pen.setWidthF(pRect->fLineWidth);
            pen.setColor(color);
            switch (pRect->tRectType)
            {
            case CDrawInfoItemRectType::Rect:
                {
                    painter->setPen(pen);
                    painter->drawRect(rect);
                }
            break;
            case CDrawInfoItemRectType::RectFill:
                {
                    painter->setPen(pen);
                    painter->setBrush(QBrush(color));
                    painter->drawRect(rect);
                }
            break;
            case CDrawInfoItemRectType::RectDash:
            {
                pen.setStyle(Qt::PenStyle::DashLine);
                painter->setPen(pen);
                painter->drawRect(rect);
            }
            break;
            case CDrawInfoItemRectType::Ellipse:
            {
                painter->setPen(pen);
                painter->drawEllipse(rect);
            }
            break;
            case CDrawInfoItemRectType::EllipseFill:
            {
                painter->setPen(pen);
                painter->setBrush(QBrush(color));
                painter->drawEllipse(rect);
            }
            break;
            case CDrawInfoItemRectType::EllipseDash:
            {
                pen.setStyle(Qt::PenStyle::DashLine);
                painter->setPen(pen);
                painter->drawEllipse(rect);
            }
            break;
            default:
                break;
            }
            if (!pRect->strString.isEmpty())
            {
                if (iFontSize<1)
                    continue;
                QFont font;
                //font.setFamily(QString::fromStdWString(L"黑体"));
                font.setPointSize(iFontSize);
                font.setBold(true);
                painter->setFont(font);
                QColor fontColor(item->FontColor);
                painter->setPen(fontColor);

                if (pRect->tRectType == CDrawInfoItemRectType::Rect
                    || pRect->tRectType == CDrawInfoItemRectType::RectFill
                    || pRect->tRectType == CDrawInfoItemRectType::RectDash)
                {
                    if ((pRect->x + pRect->Width / 2) < m_dVirtualWidth / 2
                         && (pRect->y + pRect->Height / 2) < m_dVirtualHeight / 2)//左上
                    {
                        painter->drawText(QRect(iShowX, iShowY- iFontSize /2,iClientRectWidth,iClientRectHeight ),
                                          pRect->strString);

                    }
                    else if ((pRect->x + pRect->Width / 2) > m_dVirtualWidth / 2
                             && (pRect->y + pRect->Height / 2) < m_dVirtualHeight / 2)//右上
                    {
                        painter->drawText(QRect(0,iShowY-iFontSize/2,iShowX+iShowWidth,iClientRectHeight),
                                          pRect->strString,QTextOption(Qt::AlignRight));
                    }
                    else if ((pRect->x + pRect->Width / 2) < m_dVirtualWidth / 2
                             && (pRect->y + pRect->Height / 2) > m_dVirtualHeight / 2)//左下
                    {
                        painter->drawText(QRect(iShowX,0,iClientRectWidth,iShowY+iShowHeight+iFontSize/2),
                                          pRect->strString,QTextOption(Qt::AlignLeft|Qt::AlignBottom));
                    }
                    else //右下
                    {
                        painter->drawText(QRect(0,0,iShowX+iShowWidth,iShowY+iShowHeight+iFontSize/2),
                                          pRect->strString,QTextOption(Qt::AlignRight|Qt::AlignBottom));
                    }
                }
                else
                {
                    double dOffsetX,dOffsetY;
                    if (pRect->Width == 0 && pRect->Height == 0)
                    {
                        dOffsetX = 0;
                        dOffsetY = 0;
                    }
                    else
                    {
                        dOffsetX = (sqrt(sqr(pRect->Width / 2) + sqr(pRect->Height / 2)) - pRect->Width / 2) / sqrt(sqr(pRect->Width / 2) + sqr(pRect->Height / 2))*(pRect->Width / 2);
                        dOffsetY = (sqrt(sqr(pRect->Width / 2) + sqr(pRect->Height / 2)) - pRect->Width / 2) / sqrt(sqr(pRect->Width / 2) + sqr(pRect->Height / 2))*(pRect->Height / 2);
                    }
                    painter->drawText(QPoint((pRect->x-m_dVirtualX + (int)dOffsetX)*(float)1.0*iClientRectWidth / m_dVirtualWidth, (pRect->y -m_dVirtualY + (int)dOffsetY)*(float)1.0*iClientRectHeight / m_dVirtualHeight - iFontSize / 2),pRect->strString);
                }
            }
        }

        CDrawInfoHelper::CDrawInfoItemLine *pLine = dynamic_cast<CDrawInfoHelper::CDrawInfoItemLine *>(item);
        if (NULL != pLine)
        {
            QPoint pointStart, pointEnd;

            QColor color(item->itemColor);
            QPen pen;
            pen.setWidthF(pLine->fLineWidth);
            pen.setColor(color);
            painter->setPen(pen);

            pointStart.setX((pLine->x-m_dVirtualX) *iClientRectWidth / m_dVirtualWidth);
            pointStart.setY((pLine->y-m_dVirtualY)*iClientRectHeight / m_dVirtualHeight);
            pointEnd.setX((pLine->x2-m_dVirtualX)*iClientRectWidth / m_dVirtualWidth);
            pointEnd.setY((pLine->y2-m_dVirtualY)*iClientRectHeight / m_dVirtualHeight);

            if ((pointStart.x() == pointEnd.x()) && (pointStart.y() == pointEnd.y()))
            {
                painter->drawPoint(pointStart);
            }
            else
            {
                painter->drawLine(pointStart,pointEnd);
            }
            if (!pLine->strString.isEmpty())
            {
                QFont font;
                //font.setFamily(QString::fromStdWString(L"黑体"));
                font.setPointSize(iFontSize);
                font.setBold(true);
                painter->setFont(font);
                QColor fontColor(item->FontColor);
                painter->setPen(fontColor);

                painter->drawText(QRect(pointStart,QSize(iClientRectWidth,iClientRectHeight)),pLine->strString);
            }
        }
        CDrawInfoHelper::CDrawInfoItemString *pString = dynamic_cast<CDrawInfoHelper::CDrawInfoItemString *>(item);
        if (NULL != pString)
        {
            QFont font;
            //font.setFamily(QString::fromStdWString(L"黑体"));
            font.setPointSize(iFontSize);
            font.setBold(true);
            painter->setFont(font);
            QColor fontColor(item->FontColor);
            painter->setPen(fontColor);

            int iShowX = (pString->x-m_dVirtualX)*iClientRectWidth / m_dVirtualWidth;//显示的X坐标
            int iShowY = (pString->y-m_dVirtualY)*iClientRectHeight / m_dVirtualHeight;//显示的Y坐标

            painter->drawText(QRect(iShowX,iShowY,iClientRectWidth,iClientRectHeight),pString->strString);
         }
         painter->restore();
    }
    return true;
}

}
