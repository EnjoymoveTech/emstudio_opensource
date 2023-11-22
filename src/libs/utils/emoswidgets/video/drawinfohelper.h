#ifndef DrawInfoHelper_h__
#define DrawInfoHelper_h__

#include <QMutex>
#include <QString>
#include <QPainter>
#include <map>

namespace EmosWidgets {

#define DrawItemIndexSet				0        //0-100000       Set_func 使用
#define DrawItemIndexAdd                100000   //100000+        Add_func 使用

enum class CDrawInfoItemRectType
{
	Rect,//空心方块
	RectFill,//实心方块
	RectDash,//虚线方块
	Ellipse,//空心圆
	EllipseFill,//实心圆
	EllipseDash//虚线圆
};

class CDrawInfoHelper
{
public:
    CDrawInfoHelper();
	~CDrawInfoHelper();

    //下列3个接口设置的含义
    //                 Full Width
    //          ┌────────────────────────────┐
    //          │                            │Full height
    //          │       width                │
    //          │ (X,Y)┌──────┐              │
    //          │      │      │ height       │
    //          │      └──────┘              │
    //          │          ↑                 │
    //          │      QPainter              │
    //          │                            │
    //          └────────────────────────────┘
    //
    //
    void SetVirtualXandY(double iVirtualX = 0, double iVirtualY =0);
    void SetVirtualWidthAndHeight(double iVirtualWidth = 400, double iVirtualHeight = 300);
    void SetVirtualFullWidthAndHeight(double iVirtualFullWidth = 400, double iVirtualFullHeight = 300);//这个暂时没什么用

    enum class CDrawInfoItemBaseType
    {
        None=0,
        Rect,
        Line,
        String
    };
	class CDrawInfoItemBase
	{
	public:
        CDrawInfoItemBase(void){}
        ~CDrawInfoItemBase(void){}

        virtual void toDynamic(){}

        CDrawInfoItemBaseType type;

        int x=0,y=0;//图形的起始位置
        QColor itemColor = QColor(125, 125, 125);//图形的颜色
		float fLineWidth = 1;//图形的宽度

        QString strString;//字符串
        float fFontSize = 0;//字符串的字体大小,默认自适应
        QColor FontColor = QColor(255, 255, 255);//字符串的颜色,默认为白色

	};
private:
	class CDrawInfoItemRect :public CDrawInfoItemBase
	{
	public:
        CDrawInfoItemRect(){type=CDrawInfoItemBaseType::Rect;}
		int Width;
		int Height;
		CDrawInfoItemRectType tRectType;
	};

	class CDrawInfoItemLine :public CDrawInfoItemBase
	{
	public:
        CDrawInfoItemLine(){type=CDrawInfoItemBaseType::Line;}
		int x2,y2;
	};
	class CDrawInfoItemString :public CDrawInfoItemBase
	{
	public:
        CDrawInfoItemString(){type=CDrawInfoItemBaseType::String;}

	};
private:
    double m_dVirtualX=0;//虚拟左上角X
    double m_dVirtualY=0;//虚拟左上角Y
    double m_dVirtualWidth=400;//虚拟宽度
    double m_dVirtualHeight=300;//虚拟高度
    double m_dVirtualFullWidth=400;//虚拟全尺寸宽度
    double m_dVirtualFullHeight=300;//虚拟全尺寸高度
    int m_iMapIndex = DrawItemIndexAdd;
    std::map<int, CDrawInfoItemBase *> m_mMap;
    QMutex m_csLock{QMutex::Recursive};
public:
	//画空心方块
public:
    bool Add_Rect(int x, int y, int Width, int Height, QColor itemColor = QColor(255, 0, 0), float fLineWidth = 1, CDrawInfoItemRectType tRectType = CDrawInfoItemRectType::Rect, QString text = "", QColor FontColor = QColor(255, 255, 255));
    bool Set_Rect(int index, int x, int y, int Width, int Height, QColor itemColor = QColor(255, 0, 0), float fLineWidth = 1, CDrawInfoItemRectType tRectType = CDrawInfoItemRectType::Rect, QString text = "", QColor FontColor = QColor(255, 255, 255));
private:
    static CDrawInfoItemBase *Create_Rect(int x, int y, int Width, int Height, QColor itemColor = QColor(255, 0, 0), float fLineWidth = 1, CDrawInfoItemRectType tRectType = CDrawInfoItemRectType::Rect, QString text = "", QColor FontColor = QColor(255, 255, 255));//x,y为左上坐标
    static bool Set_Rect(CDrawInfoItemBase *item, int x, int y, int Width, int Height, QColor itemColor = QColor(255, 0, 0), float fLineWidth = 1, CDrawInfoItemRectType tRectType = CDrawInfoItemRectType::Rect, QString text = "", QColor FontColor = QColor(255, 255, 255));

	//画xy为中心的空心方块
public:
    inline bool Add_CenterRect(int CenterX, int CenterY, int Width, int Height, QColor itemColor = QColor(255, 0, 0), float fLineWidth = 1, CDrawInfoItemRectType tRectType = CDrawInfoItemRectType::Rect, QString text = "", QColor FontColor = QColor(255, 255, 255)){
        return Add_Rect(CenterX - Width / 2, CenterY - Height / 2,Width,Height,itemColor, fLineWidth, tRectType, text, FontColor);
    }
    inline bool Set_CenterRect(int index, int CenterX, int CenterY, int Width, int Height, QColor itemColor = QColor(255, 0, 0), float fLineWidth = 1, CDrawInfoItemRectType tRectType = CDrawInfoItemRectType::Rect, QString text = "", QColor FontColor = QColor(255, 255, 255)){
        return Set_Rect(index,CenterX - Width / 2, CenterY - Height / 2,Width,Height,itemColor, fLineWidth, tRectType, text, FontColor);
    }

	//画线
public:
    bool Add_Line(int x1, int y1, int x2, int y2, QColor itemColor = QColor(255, 0, 0), float fLineWidth = 1, QString text = "", QColor FontColor = QColor(255, 255, 255));
    bool Set_Line(int index, int x1, int y1, int x2, int y2, QColor itemColor = QColor(255, 0, 0), float fLineWidth = 1, QString text = "", QColor FontColor = QColor(255, 255, 255));
private:
    static CDrawInfoItemBase *Create_Line(int x1, int y1, int x2, int y2, QColor itemColor = QColor(255, 0, 0), float fLineWidth = 1, QString text = "", QColor FontColor = QColor(255, 255, 255));//x,y为左上坐标
    static bool Set_Line(CDrawInfoItemBase *item, int x1, int y1, int x2, int y2, QColor itemColor = QColor(255, 0, 0), float fLineWidth = 1, QString text = "", QColor FontColor = QColor(255, 255, 255));

	//画字
public:
    bool Add_String(int x, int y, QString text, QColor FontColor = QColor(255, 255, 255), float fFontSize = 0);
    bool Set_String(int index, int x, int y, QString text, QColor FontColor = QColor(255, 255, 255), float fFontSize = 0);
private:
    static CDrawInfoItemString *Create_String(int x, int y, QString text, QColor FontColor = QColor(255, 255, 255), float fFontSize = 0);//x,y为左上坐标
    static bool Set_String(CDrawInfoItemBase *item, int x, int y, QString text, QColor FontColor = QColor(255, 255, 255), float fFontSize = 0);
public:
    bool ResetItem();//删除所有item
    bool ResetAddItem();//删除用ADD方法添加的item
    bool ResetSetItem();//删除用Set方法添加的item
    bool DeleteItem(int index);//删除ITEM

    bool DrawInfo(QPainter *painter);//绘制区域
private:
    bool SetItem(int index, CDrawInfoItemBase *item);
    bool AddItem(CDrawInfoItemBase *item);
};

}
#endif // DrawInfoHelper_h__
