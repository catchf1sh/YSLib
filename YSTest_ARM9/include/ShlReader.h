﻿/*
	Copyright (C) by Franksoft 2010 - 2012.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ShlReader.h
\ingroup YReader
\brief Shell 阅读器框架。
\version r1735;
\author FrankHB<frankhb1989@gmail.com>
\since build 263 。
\par 创建时间:
	2011-11-24 17:08:33 +0800;
\par 修改时间:
	2012-02-06 02:05 +0800;
\par 文本编码:
	UTF-8;
\par 模块名称:
	YReader::ShlReader;
*/


#ifndef INCLUDED_SHLREADER_H_
#define INCLUDED_SHLREADER_H_

#include "Shells.h"
#include "DSReader.h"
#include "HexBrowser.h"
#include "ColorPicker.h"

YSL_BEGIN_NAMESPACE(YReader)

class ShlReader;


class ReaderBox : public Control
{
public:
	ShlReader& Shell;
	/*!
	\brief 弹出菜单按钮。
	\since build 274 。
	*/
	Button btnMenu;
	/*!
	\brief 显示信息框按钮。
	\since build 274 。
	*/
	Button btnInfo;
	/*!
	\brief 返回按钮。
	\since build 274 。
	*/
	Button btnReturn;
	ProgressBar pbReader;
	Label lblProgress;

	ReaderBox(const Rect&, ShlReader&);

	virtual IWidget*
	GetTopWidgetPtr(const Point&, bool(&)(const IWidget&));

	virtual Rect
	Refresh(const PaintContext&);

	/*!
	\brief 更新进度数据。
	\since build 271 。
	*/
	void
	UpdateData(DualScreenReader&);
};


class TextInfoBox : public DialogBox
{
public:
	ShlReader& Shell;
	Label lblEncoding;
	Label lblSize;
	/*!
	\brief 指示迭代器位置。
	\since build 273 。
	*/
	//@{
	Label lblTop;
	Label lblBottom;
	//@}

	TextInfoBox(ShlReader&);

	virtual Rect
	Refresh(const PaintContext&);

	void
	UpdateData(DualScreenReader&);
};


/*!
\brief 设置面板。
\since build 275 。
*/
class SettingPanel : public DialogPanel
{
	/*!
	\brief 友元类：共享设置状态。
	\since build 279 。
	*/
	friend class TextReaderManager;

protected:
	/*!
	\brief 文本区域示例文本标签。
	\note 不加入 SettingPanel 容器，加入上屏桌面。
	\since build 280 。
	*/
	Label lblAreaUp, lblAreaDown;
	/*!
	\brief 字体大小递减/递增按钮。
	\since build 280 。
	*/
	Button btnFontSizeDecrease, btnFontSizeIncrease;
	/*!
	\brief 设置按钮。
	\since build 279 。
	*/
	Button btnSetUpBack, btnSetDownBack, btnTextColor;
	/*!
	\brief 字体设置下拉列表。
	\since build 282 。
	*/
	DropDownList ddlFont;
	ColorBox boxColor;

private:
	/*!
	\brief 当前设置的颜色指针。
	\none 为空指针时忽略。
	\since build 277 。
	*/
	Color* pColor;

public:
	SettingPanel();

	/*!
	\brief 更新示例文本。
	\since build 280 。
	*/
	void
	UpdateInfo();
};


class FileInfoPanel : public Panel
{
public:
	Label lblPath, lblSize, lblAccessTime, lblModifiedTime, lblOperations;
	
	FileInfoPanel();
};


class ReaderManager
{
public:
	static string path;
	static bool is_text;

	ShlReader& Shell;

	ReaderManager(ShlReader&);
	DefDelCopyCtor(ReaderManager)
	DefDelMoveCtor(ReaderManager)
	virtual DefEmptyDtor(ReaderManager)
	DeclIEntry(void Activate())
	DeclIEntry(void Deactivate())
};


class TextReaderManager : public ReaderManager
{
public:
	DualScreenReader Reader;
	ReaderBox boxReader;
	TextInfoBox boxTextInfo;
	/*!
	\brief 设置面板。
	\since build 275 。
	*/
	SettingPanel pnlSetting;
	unique_ptr<TextFile> pTextFile;
	MenuHost mhMain;

	TextReaderManager(ShlReader&);

	virtual void
	Activate();

	virtual void
	Deactivate();

private:
	/*!
	\brief 执行阅读器命令。
	\since build 274 。
	*/
	void
	Execute(IndexEventArgs::ValueType);

	void
	ShowMenu(Menu::ID, const Point&);

	void
	OnClick(TouchEventArgs&&);

	void
	OnKeyDown(KeyEventArgs&&);
};


class HexReaderManager : public ReaderManager
{
public:
	HexViewArea HexArea;
	FileInfoPanel pnlFileInfo;

	HexReaderManager(ShlReader&);

	virtual void
	Activate();

	virtual void
	Deactivate();

	void
	UpdateInfo();
};


class ShlReader : public ShlDS
{
public:
	typedef ShlDS ParentType;

	shared_ptr<Image> hUp, hDn;

private:
	unique_ptr<ReaderManager> pManager;

public:
	ShlReader();

	virtual int
	OnActivated(const Message&);

	virtual int
	OnDeactivated(const Message&);
};

YSL_END_NAMESPACE(YReader)

#endif
