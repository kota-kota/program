﻿#include <cstdint>
#include "win32/WindowWGL.hpp"

//メイン処理
std::int32_t main()
{
	//ウィンドウ作成・スタート
	window::WindowWGL window;
	window.start();

	return 0;
}