﻿/*
	Copyright (C) by Franksoft 2010 - 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ydesktop.cpp
\ingroup Shell
\brief 平台无关的桌面抽象层。
\version 0.2303;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2010-05-02 12:00:08 +0800;
\par 修改时间:
	2011-05-26 22:31 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::UI::Desktop;
*/


#include "ydesktop.h"
#include <algorithm>

YSL_BEGIN

using namespace Drawing;
using namespace Runtime;

YSL_BEGIN_NAMESPACE(Components)

using namespace Controls;

Desktop::Desktop(YScreen& s, Color c, const shared_ptr<Image>& hImg)
	: Frame(Rect::FullScreen, hImg),
	Screen(s)
{
	BackColor = c;
}

IControl*
Desktop::GetTopVisibleDesktopObjectPtr(const Point& pt) const
{
	for(auto i(sWidgets.crbegin()); i != sWidgets.crend(); ++i)
	{
		try
		{
			if(i->second->IsVisible() && Contains(*i->second, pt))
				return &dynamic_cast<IControl&>(*i->second);
		}
		catch(std::bad_cast&)
		{}
	}
	return nullptr;
}

bool
Desktop::MoveToTop(IControl& ctl)
{
	auto i(std::find_if(sWidgets.begin(), sWidgets.end(),
		[&](const WidgetMap::value_type& val){
		return val.second == &ctl;
	}));

	if(i != sWidgets.end())
	{
		const Widgets::ZOrderType z(i->first);

		sWidgets.erase(i);
		sWidgets.insert(make_pair(z, static_cast<IWidget*>(&ctl)));
		bRefresh = true;
		return true;
	}
	return false;
}

/*void
Desktop::RemoveTopDesktopObject()
{
	if(!sDOs.empty())
	{
		sDOs.back()->ReleaseFocus(EventArgs());
		sDOs.pop_back();
		bRefresh = true;
	}
}*/

void
Desktop::ClearContents()
{
	ClearFocusingPtr();
	sWidgets.clear();
	sFocusContainers.clear();
	bRefresh = true;
}

void
Desktop::Update()
{
	if(bRefresh)
		bUpdate = false;
	if(bUpdate)
		Screen.Update(GetContext().GetBufferPtr());
}

YSL_END_NAMESPACE(Components)

YSL_END
