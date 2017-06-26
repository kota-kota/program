﻿#pragma once
#include "Common.hpp"
#include "DrawIF.hpp"
#include "utl/File.hpp"
#include "image/Bitmap.hpp"

namespace ui
{
	class UiMng {
		//メンバ変数
		draw::DrawIF*	mDrawIF;
		draw::DrawSetup	mDrawSetup;

		utl::File		mFile;
		image::Bitmap	mBitmap;

	public:
		//コンストラクタ
		UiMng(draw::DrawIF* drawIF = nullptr);
		//画面中心位置を設定
		void setScreenPosition(cmn::CoordI16 screenPos);
		//描画
		void draw();
	};
}
