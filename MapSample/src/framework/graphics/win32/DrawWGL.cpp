﻿#include "DrawWGL.hpp"

//using指定
using cmn::Int16_t;
using cmn::Int32_t;
using cmn::Float32_t;
using cmn::Float64_t;
using cmn::Color;
using cmn::CoordI32;
using cmn::Size;
using cmn::CoordVec;
using cmn::Image;
using graphics::DrawWGL;

//コンストラクタ
DrawWGL::DrawWGL(HWND hWnd)
	: hWnd(hWnd), hDC(nullptr), hGLRC(nullptr)
{
	if (this->hWnd != nullptr) {
		//デバイスコンテキストハンドルを取得
		this->hDC = ::GetDC(this->hWnd);

		//ピクセルフォーマット
		const PIXELFORMATDESCRIPTOR pFormat = {
			sizeof(PIXELFORMATDESCRIPTOR),	//nSize
			1,		//nVersion
			PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,	//dwFlags
			PFD_TYPE_RGBA,	//iPixelType
			32,		//cColorBits
			0,		//cRedBits
			0,		//cRedShift
			0,		//cGreenBits
			0,		//cGreenShift
			0,		//cBlueBits
			0,		//cBlueShift
			0,		//cAlphaBits
			0,		//cAlphaShift
			0,		//cAccumBits
			0,		//cAccumRedBits
			0,		//cAccumGreenBits
			0,		//cAccumBlueBits
			0,		//cAccumAlphaBits
			24,		//cDepthBits
			8,		//cStencilBits
			0,		//cAuxBuffers
			0,		//iLayerType
			0,		//bReserved
			0,		//dwLayerMask
			0,		//dwVisibleMask
			0		//dwDamageMask
		};

		//ピクセルフォーマットを選択
		Int32_t format = ::ChoosePixelFormat(this->hDC, &pFormat);
		::SetPixelFormat(this->hDC, format, &pFormat);

		//描画コンテキストハンドルを作成
		this->hGLRC = ::wglCreateContext(this->hDC);
	}
}

//デストラクタ
DrawWGL::~DrawWGL()
{
	//描画コンテキストハンドルを破棄
	::wglDeleteContext(this->hGLRC);

	//デバイスコンテキストを破棄
	::ReleaseDC(this->hWnd, this->hDC);
}

//描画セットアップ
void DrawWGL::setup(CoordI32 mapPos)
{
	//描画領域取得
	Size drawSize;
	this->getDrawSize(drawSize);

	//ビューポート設定
	glViewport(0, 0, drawSize.w, drawSize.h);

	//プロジェクション設定
	Float64_t left = mapPos.x - drawSize.w / 2;
	Float64_t right = left + drawSize.w;
	Float64_t top = mapPos.y - drawSize.h / 2;
	Float64_t bottom = top + drawSize.h;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//glOrtho(left, right, bottom, top, -1, 1);
	glOrtho(left, right, top, bottom, -1, 1);

	//モデルビュー設定
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

//描画カレント
void DrawWGL::makeCurrent(const bool current)
{
	if (current) {
		//カレント設定
		::wglMakeCurrent(this->hDC, this->hGLRC);
	}
	else {
		//カレント解除
		::wglMakeCurrent(this->hDC, nullptr);
	}
}

//描画更新
void DrawWGL::swapBuffers()
{
	::SwapBuffers(this->hDC);
}

//クリア
void DrawWGL::clear(const Color& color)
{
	Float32_t r = static_cast<Float32_t>(color.r) / 255.0f;
	Float32_t g = static_cast<Float32_t>(color.g) / 255.0f;
	Float32_t b = static_cast<Float32_t>(color.b) / 255.0f;
	Float32_t a = static_cast<Float32_t>(color.a) / 255.0f;

	glClearColor(r, g, b, a);
	glClearDepth(1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

//点描画
void DrawWGL::drawPoint(const Color& color, const CoordVec& coord, const float size)
{
	Float32_t r = static_cast<Float32_t>(color.r) / 255.0f;
	Float32_t g = static_cast<Float32_t>(color.g) / 255.0f;
	Float32_t b = static_cast<Float32_t>(color.b) / 255.0f;
	Float32_t a = static_cast<Float32_t>(color.a) / 255.0f;

	//点の大きさを設定
	glPointSize(size);

	//点描画
	glBegin(GL_POINTS);
	{
		//色
		glColor4f(r, g, b, a);

		//座標
		for (auto itr = coord.begin(); itr != coord.end(); itr++) {
			glVertex3i(itr->x, itr->y, itr->z);
		}
	}
	glEnd();
}

//ライン描画
void DrawWGL::drawLine(const Color& color, const CoordVec& coord, const float width)
{
	Float32_t r = static_cast<Float32_t>(color.r) / 255.0f;
	Float32_t g = static_cast<Float32_t>(color.g) / 255.0f;
	Float32_t b = static_cast<Float32_t>(color.b) / 255.0f;
	Float32_t a = static_cast<Float32_t>(color.a) / 255.0f;

	//ラインの太さを設定
	glLineWidth(width);

	//ライン描画
	glBegin(GL_LINE_LOOP);
	{
		//色
		glColor4f(r, g, b, a);

		//座標
		for (auto itr = coord.begin(); itr != coord.end(); itr++) {
			glVertex3i(itr->x, itr->y, itr->z);
		}
	}
	glEnd();
}

//テクスチャ描画
void DrawWGL::drawTextrue(const CoordVec& coord, const Image& tex, const Size texSize)
{
	//テクスチャ作成
	GLuint texID;
	glGenTextures(1, &texID);

	//テクスチャ割り当て
	glBindTexture(GL_TEXTURE_2D, texID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texSize.w, texSize.h, 0, GL_RGBA, GL_UNSIGNED_BYTE, &tex[0]);

	//テクスチャ画像は1バイト単位
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	//テクスチャパラメータ
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	//テクスチャ環境
	//テクスチャカラーを使用する
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	//テクスチャ描画
	glEnable(GL_TEXTURE_2D);
	glBegin(GL_TRIANGLE_STRIP);
	{
		//色
		//念のためフラグメントカラーを1.0fで初期化する
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

		//座標
		auto itr = coord.begin();
		while (itr != coord.end()) {
			glTexCoord2i(itr->x, itr->y);
			itr++;
			glVertex3i(itr->x, itr->y, itr->z);
			itr++;
		}
	}
	glEnd();
	glDisable(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, 0);
	glDeleteTextures(1, &texID);
}

//描画領域取得
void DrawWGL::getDrawSize(Size& drawSize)
{
	RECT rect;
	::GetClientRect(this->hWnd, &rect);
	drawSize.w = static_cast<Int16_t>(rect.right - rect.left);
	drawSize.h = static_cast<Int16_t>(rect.bottom - rect.top);
}
