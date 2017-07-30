﻿#ifndef INCLUDED_DRAWWGL
#define INCLUDED_DRAWWGL

#include "Common.hpp"
#include "../DrawIF.hpp"
#include <Windows.h>
#include <gl/GL.h>

namespace graphics
{
	class DrawWGL : public DrawIF {
		HWND	hWnd;
		HDC		hDC;
		HGLRC	hGLRC;

	public:
		//コンストラクタ
		DrawWGL(HWND hWnd = nullptr);
		//デストラクタ
		virtual ~DrawWGL();
		//セットアップ
		virtual void setup(cmn::CoordI32 mapPos);
		//描画カレント
		virtual void makeCurrent(const bool current);
		//描画更新
		virtual void swapBuffers();
		//クリア
		virtual void clear(const cmn::Color& color);
		//点描画
		virtual void drawPoint(const cmn::Color& color, const cmn::CoordVec& coord, const float size);
		//ライン描画
		virtual void drawLine(const cmn::Color& color, const cmn::CoordVec& coord, const float width);
		//テクスチャ描画
		virtual void drawTextrue(const cmn::CoordVec& coord, const cmn::Image& tex, const cmn::Size texSize);

		//描画領域取得
		virtual void getDrawSize(cmn::Size& drawSize);
	};
}

#endif //INCLUDED_DRAWWGL
