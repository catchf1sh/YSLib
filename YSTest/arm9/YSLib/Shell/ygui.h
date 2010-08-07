﻿// YSLib::Shell::YGUI by Franksoft 2009 - 2010
// CodePage = UTF-8;
// CTime = 2009-11-16 20:06:58;
// UTime = 2010-7-26 14:02;
// Version = 0.1829;


#ifndef INCLUDED_YGUI_H_
#define INCLUDED_YGUI_H_

// YGUI ：平台无关的图形用户界面基础实现。

#include "../Core/yshell.h"
#include "ydesktop.h"
#include "yform.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Runtime)

//从指定 Shell 传递指定屏幕的指针设备光标至 GUI 界面，返回部件指针。
IWidget*
GetCursorWidgetPtr(HSHL, YDesktop&, const SPoint&);

//取指定屏幕中的当前焦点对象。
IVisualControl*
GetFocusedObject(YDesktop&);

//级联请求可视控件及上层容器焦点。
void
RequestFocusCascade(IVisualControl&);

//级联释放可视控件及上层容器焦点。
void
ReleaseFocusCascade(IVisualControl&);


//标准 GUI 事件回调函数抽象类。
typedef GAHEventCallback<Components::Controls::MVisualControl, MEventArgs> AHEventCallback;
typedef GAHEventCallback<Components::Controls::MVisualControl, Runtime::MTouchEventArgs> AHTouchCallback;
typedef GAHEventCallback<Components::Controls::MVisualControl, Runtime::MKeyEventArgs> AHKeyCallback;

//标准 GUI 事件回调函数类型。
typedef bool FTouchCallback(Components::Controls::MVisualControl&, const Runtime::MTouchEventArgs&);
typedef bool FKeyCallback(Components::Controls::MVisualControl&, const Runtime::MKeyEventArgs&);
typedef FTouchCallback* PFTouchCallback;
typedef FKeyCallback* PFKeyCallback;

//标准 GUI 事件回调函数对象类。
struct HTouchCallback : public GHBase<PFTouchCallback>, public AHTouchCallback
{
	inline explicit
	HTouchCallback(Runtime::MTouchEventArgs e, PFTouchCallback p)
	: GHBase<PFTouchCallback>(p), AHTouchCallback(e)
	{}

	inline bool
	operator()(Components::Controls::MVisualControl& c)
	{
		return GetPtr()(c, *this);
	}
};

struct HKeyCallback : public GHBase<PFKeyCallback>, public AHKeyCallback
{
	inline explicit
	HKeyCallback(Runtime::MKeyEventArgs e, PFKeyCallback p)
	: GHBase<PFKeyCallback>(p), AHKeyCallback(e)
	{}

	inline bool
	operator()(Components::Controls::MVisualControl& c)
	{
		return GetPtr()(c, *this);
	}
};


//记录输入保持状态。
class MTouchStatus
{
private:
	static SVec v_DragOffset;

public:
	DefStaticBoolGetter(OnDragging, v_DragOffset != SVec::FullScreen)

	DefStaticGetter(const SVec&, DragOffset, v_DragOffset)

	static DefSetterDe(const SVec&, DragOffset, v_DragOffset, SVec::FullScreen)
};


//响应标准屏幕点击状态。
bool
ResponseTouchUp(IWidgetContainer&, const Runtime::MTouchEventArgs&);
bool
ResponseTouchDown(IWidgetContainer&, const Runtime::MTouchEventArgs&);
bool
ResponseTouchHeld(IWidgetContainer&, const Runtime::MTouchEventArgs&);

//响应标准按键状态。
bool
ResponseKeyUp(YDesktop&, const Runtime::MKeyEventArgs&);
bool
ResponseKeyDown(YDesktop&, const Runtime::MKeyEventArgs&);
bool
ResponseKeyHeld(YDesktop&, const Runtime::MKeyEventArgs&);

YSL_END_NAMESPACE(Runtime)

YSL_BEGIN_NAMESPACE(Drawing)

//取图形接口上下文。
inline MGIC
GetGraphicInterfaceContext(HWND hWnd)
{
	YAssert(hWnd != NULL,
		"In function \"inline MGIC\nDrawing::GetGraphicInterfaceContext(HWND hWnd)\": \n"
		"The input handle is null.");

	return static_cast<MGIC>(*hWnd);
}


//绘制界面元素边框。
void
DrawBounds(MGIC&, const SPoint&, const SSize&, PixelType);

//绘制窗口边框。
void
DrawWindowBounds(HWND, PixelType);

//绘制部件边框。
void
DrawWidgetBounds(IWidget&, PixelType);

YSL_END_NAMESPACE(Drawing)

YSL_END

#endif
