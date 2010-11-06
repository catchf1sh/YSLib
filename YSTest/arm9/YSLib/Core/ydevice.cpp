﻿// YSLib::Core::YDevice by Franksoft 2009 - 2010
// CodePage = UTF-8;
// CTime = 2009-12-28 16:39:51 + 08:00;
// UTime = 2010-11-03 19:52 + 08:00;
// Version = 0.2672;


#include "ydevice.h"
#include "yshell.h"
#include "../Shell/ygdi.h"

YSL_BEGIN

using namespace Drawing;

YSL_BEGIN_NAMESPACE(Device)

bool YScreen::S_InitScr(true);

YScreen::YScreen(SDST w, SDST h, BitmapPtr p)
	: YGraphicDevice(w, h, p),
bg(-1)
{}

void
YScreen::CheckInitialization()
{
	if(S_InitScr)
		InitScreen();
}

void
YScreen::Reset()
{
	InitScreen();
}

void
YScreen::Update(BitmapPtr buf)
{
	platform::ScreenSychronize(GetPtr(), buf);
}
void
YScreen::Update(Color c)
{
	FillSeq<PixelType>(GetPtr(), GetAreaFrom(*this), c);
}

YSL_END_NAMESPACE(Device)

YSL_END

