﻿#include "LayerManager.hpp"
#include "DrawGL.hpp"
#include "ImageDecorder.hpp"
#include "Str.hpp"

#include <Windows.h>
#include <tchar.h>

#include <cstdio>
#include <string>


//定数定義
namespace {

	//ウィンドウ名
	static const TCHAR* D_WIN_NAME = TEXT("WinWindow");

	//ウィンドウ位置
	static const int D_WIN_POSX = 0;
	static const int D_WIN_POSY = 0;

	//ウィンドウ幅高さ
	static const int D_WIN_WIDTH = 800;
	static const int D_WIN_HEIGHT = 400;

	//EGLコンフィグ属性
	EGLint attr_config[] = {
		EGL_RENDERABLE_TYPE,	EGL_OPENGL_ES2_BIT,
		EGL_SURFACE_TYPE,		EGL_WINDOW_BIT,
		EGL_RED_SIZE,			8,
		EGL_GREEN_SIZE,			8,
		EGL_BLUE_SIZE,			8,
		EGL_ALPHA_SIZE,			8,
		EGL_DEPTH_SIZE,			8,
		EGL_NONE,
	};

	//EGLPBuffer属性
	EGLint attr_pbuffer[] = {
		EGL_WIDTH,		200,
		EGL_HEIGHT,		200,
		EGL_NONE,
	};

	//EGLコンテキスト属性
	EGLint attr_context[] = {
		EGL_CONTEXT_CLIENT_VERSION,	2,
		EGL_NONE,
	};
}

//サンプル描画
namespace {

	//画像ファイルトップパス
	const std::string imagePath = "./data/image/";

	//背景色
	fw::ColorUB backColor = { 125, 125, 125, 255 };

	//ライン
	const std::int32_t linePointNum = 4;
	std::float32_t linePoints[linePointNum * 3] = {
		0.0F, 0.0F, 0.0F,
		50.0F, 50.0F, 0.0F,
		150.0F, 100.0F, 0.0F,
		300.0F, 0.0F, 0.0F,
	};
	std::uint8_t lineColors[linePointNum * 4] = {
		255, 0, 0, 255,
		255, 0, 0, 255,
		255, 0, 0, 255,
		255, 0, 0, 255,
	};

	//ポリゴン
	const std::int32_t polygonPointNum = 3;
	std::float32_t polygonPoints[polygonPointNum * 3] = {
		150.0F, 150.0F, 0.0F,
		200.0F, 200.0F, 0.0F,
		240.0F, 150.0F, 0.0F,
	};
	std::uint8_t polygonColors[polygonPointNum * 4] = {
		255, 0, 0, 255,
		0, 255, 0, 255,
		0, 0, 255, 255,
	};

	//画像
	std::float32_t angle = 0.0F;
	struct ImageFile {
		std::uint32_t		id_;
		std::string			bodyFile_;
		std::string			blendFile_;
		fw::EN_ImageFormat	format_;
	};
	ImageFile tblImageFile[] = {
		{ 0x01, "mountain.bmp", "blend_star.bmp", fw::EN_ImageFormat::BMP },
		{ 0x02, "cat.png", "", fw::EN_ImageFormat::PNG },
		{ 0x03, "flower.png", "", fw::EN_ImageFormat::PNG },
		{ 0x04, "droidkun.png", "", fw::EN_ImageFormat::PNG },
	};
	const std::int32_t imageFileNum = sizeof(tblImageFile) / sizeof(tblImageFile[0]);
	std::float32_t imgPoint[imageFileNum * 3] = {
		200.0F, 300.0F, 0.0F,
		300.0F, 300.0F, 0.0F,
		400.0F, 300.0F, 0.0F,
		500.0F, 300.0F, 0.0F,
	};

	//テキスト
	const std::int32_t textNum = 1;
	std::float32_t textPoint[textNum * 3] = {
		500.0F, 100.0F, 0.0F,
	};
	std::char8_t* sjis = "OpenGL<_^:^;> ｱｲｳｴ東京ドーム。compass.";
	const fw::StringU8 sjisText(sjis, static_cast<std::int32_t>(strlen(sjis)), fw::EN_CharCode::SJIS);

	//画面
	class Screen {
		fw::LayerIF*		layer_;
		fw::ImageDecorder	imgDecoder_[imageFileNum];

	public:
		//コンストラクタ
		Screen() :
			layer_(nullptr)
		{
		}

		//画面作成
		void Screen::create(fw::LayerIF* layer)
		{
			this->layer_ = layer;

			//画像デコード
			for (std::int32_t iImage = 0; iImage < imageFileNum; iImage++) {
				ImageFile* imageFile = &tblImageFile[iImage];
				if (imageFile->bodyFile_.empty()) {
					continue;
				}
				const std::string bodyFilePath = imagePath + imageFile->bodyFile_;
				if (imageFile->blendFile_.empty()) {
					this->imgDecoder_[iImage].decode_RGBA8888(bodyFilePath.c_str(), nullptr, imageFile->format_);
				}
				else {
					const std::string blendFilePath = imagePath + imageFile->blendFile_;
					this->imgDecoder_[iImage].decode_RGBA8888(bodyFilePath.c_str(), blendFilePath.c_str(), imageFile->format_);
				}
			}
		}

		//画面破棄
		void Screen::destroy()
		{
		}

		//サンプル描画
		void Screen::draw()
		{
			printf("<Screen::draw> angle:%.2f\n", angle);

			//描画インターフェースを取得
			fw::DrawIF* drawIF = this->layer_->getDrawIF();

			fw::AreaI area = { 0, 0, 0, 0 };
			this->layer_->getSize(&area.xmax, &area.ymax);

			//描画開始
			this->layer_->beginDraw();

			//描画処理
			drawIF->setup(area);

			//画面クリア
			drawIF->clear(backColor);

			//画像描画
			for (std::int32_t iImage = 0; iImage < imageFileNum; iImage++) {
				std::int32_t dataSize, w, h;
				std::uint8_t* data = this->imgDecoder_[iImage].getDecodeData(&dataSize, &w, &h);

				fw::ImageAttr imgAttr;
				imgAttr.id = 0;
				imgAttr.width = w;
				imgAttr.height = h;
				imgAttr.pixFormat = fw::EN_PixelFormat::RGBA;
				imgAttr.baseLoc = fw::EN_BaseLoc::CENTER_CENTER;
				drawIF->drawImage(&imgPoint[iImage * 3], angle, data, imgAttr);
			}

			//ライン描画
			drawIF->drawLines(linePointNum, &linePoints[0], &lineColors[0], 10.0F, fw::EN_LineType::LINE_STRIP);

			//ポリゴン描画
			drawIF->drawPolygons(polygonPointNum, &polygonPoints[0], &polygonColors[0], fw::EN_PolygonType::TRIANGLE_STRIP);

			//テキスト描画
			fw::TextAttr textAttr = { 0 };
			textAttr.size = 20;
			textAttr.bodyColor = { 255, 255, 0, 255 };
			drawIF->drawText(textPoint, sjisText, textAttr);

			//描画終了
			this->layer_->endDraw();

			angle += 10.0F;
			if (angle >= 360.0F) {
				angle = 0.0F;
			}
		}
	};
}

//内部関数
namespace {

	//ウィンドウオブジェクト
	class Window;
	Window* gWindow = nullptr;

	//ウィンドウプロシージャ
	LRESULT CALLBACK winproc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	/**
	 * ウィンドウクラス
	 */
	class Window {
		//メンバ変数
		HINSTANCE			hInstance_;
		HWND				hWnd_;
		fw::LayerManager	layerManager_;
		Screen				screen_;

	public:
		//コンストラクタ
		Window() :
			hInstance_(nullptr), hWnd_(nullptr), layerManager_(), screen_()
		{
		}

		//デストラクタ
		~Window()
		{
			this->destroy();
		}

		//ウィンドウ作成
		int create()
		{
			printf("<Window::create>\n");

			int rc = -1;

			//インスタンスハンドル取得
			this->hInstance_ = ::GetModuleHandle(nullptr);
			printf("GetModuleHandle %p\n", this->hInstance_);

			//クラス登録情報設定
			WNDCLASSEX wcex;
			wcex.cbSize = sizeof(WNDCLASSEX);
			wcex.style = CS_HREDRAW | CS_VREDRAW;
			wcex.lpfnWndProc = winproc;
			wcex.lpszClassName = D_WIN_NAME;
			wcex.cbClsExtra = 0;
			wcex.cbWndExtra = 0;
			wcex.hInstance = this->hInstance_;
			wcex.hIcon = ::LoadIcon(nullptr, IDI_APPLICATION);
			wcex.hCursor = ::LoadCursor(nullptr, IDC_ARROW);
			wcex.hbrBackground = nullptr;
			wcex.lpszMenuName = nullptr;
			wcex.hIconSm = ::LoadIcon(nullptr, IDI_APPLICATION);

			//クラス登録
			ATOM ret = ::RegisterClassEx(&wcex);
			if (ret == 0) {
				printf("[ERROR] %s:%d RegisterClassEx\n", __FUNCTION__, __LINE__);
				goto END;
			}
			printf("RegisterClassEx\n");

			//ウィンドウ作成
			HWND hWnd = ::CreateWindowEx(
				0,
				D_WIN_NAME,
				D_WIN_NAME,
				WS_OVERLAPPEDWINDOW,
				D_WIN_POSX,
				D_WIN_POSY,
				D_WIN_WIDTH,
				D_WIN_HEIGHT,
				nullptr,
				nullptr,
				this->hInstance_,
				nullptr
			);
			if (hWnd == nullptr) {
				printf("[ERROR] %s:%d CreateWindowEx\n", __FUNCTION__, __LINE__);
				goto END;
			}
			printf("CreateWindowEx %p\n", hWnd);

			// ウィンドウ表示
			ShowWindow(hWnd, SW_SHOW);
			UpdateWindow(hWnd);

			//正常終了
			rc = 0;

		END:
			return rc;
		}

		//ウィンドウ破棄
		void destroy()
		{
			printf("<Window::destroy>\n");
			if (this->hWnd_ != nullptr) {
				::DestroyWindow(this->hWnd_);
			}
		}

		//ウィンドウ開始
		int start()
		{
			printf("<Window::start>\n");

			MSG msg;
			while (GetMessage(&msg, nullptr, 0, 0) != 0) {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}

			return 0;
		}

		//WM_CREATEイベント処理
		void winproc_create(HWND hWnd)
		{
			printf("<winproc_create> hWnd:0x%p\n", hWnd);

			//ウィンドウハンドルを保持
			this->hWnd_ = hWnd;

			//レイヤー管理オブジェクト作成
			this->layerManager_.createWindow(this->hWnd_);

			//ウィンドウサイズを取得
			RECT rect;
			::GetClientRect(hWnd, &rect);
			const std::int32_t width = rect.right - rect.left;
			const std::int32_t height = rect.bottom - rect.top;

			//レイヤー作成
			fw::LayerIF* layer = this->layerManager_.createLayer(width, height);

			//画面作成
			this->screen_.create(layer);
		}

		//WM_DESTROYイベント処理
		void winproc_destroy()
		{
			::PostQuitMessage(0);
		}

		//WM_PAINTイベント処理
		void winproc_paint()
		{
			PAINTSTRUCT ps;
			::BeginPaint(this->hWnd_, &ps);
			::EndPaint(this->hWnd_, &ps);

			//サンプル描画
			this->screen_.draw();

			//次フレーム描画のため、WM_PAINTイベントを発行
			RECT rect;
			::GetClientRect(this->hWnd_, &rect);
			::InvalidateRect(this->hWnd_, &rect, false);
		}

		//WM_LBUTTONDOWNイベント処理
		void winproc_lbuttondown(LPARAM lParam)
		{
			//座標を取得
			int x = LOWORD(lParam);
			int y = HIWORD(lParam);

			printf("<winproc_lbuttondown> x:%d y:%d\n", x, y);
		}

		//WM_LBUTTONUPイベント処理
		void winproc_lbuttonup(LPARAM lParam)
		{
			//座標を取得
			int x = LOWORD(lParam);
			int y = HIWORD(lParam);

			printf("<winproc_lbuttonup> x:%d y:%d\n", x, y);
		}

		//WM_MOUSEWHEELイベント処理
		void winproc_mousewheel(WPARAM wParam, LPARAM lParam)
		{
			//座標を取得
			int x = LOWORD(lParam);
			int y = HIWORD(lParam);

			//回転量を取得(120の倍数、正値は奥、負値は手前)
			int zdelta = GET_WHEEL_DELTA_WPARAM(wParam);

			if (zdelta >= 0) {
				//ホイール処理(奥)
				printf("<winproc_mousewheel>FORWARD x:%d y:%d zdelta:%d\n", x, y, zdelta);
			}
			else {
				//ホイール処理(手前)
				printf("<winproc_mousewheel>BACKWARD x:%d y:%d zdelta:%d\n", x, y, zdelta);
			}
		}

		//WM_MOUSEMOVEイベント処理
		void winproc_mousemove(LPARAM lParam)
		{
			//座標を取得
			int x = LOWORD(lParam);
			int y = HIWORD(lParam);

			//タッチ移動
			//printf("<winproc_mousemove> x:%d y:%d\n", x, y);
		}
	};

	//ウィンドウプロシージャ
	LRESULT CALLBACK winproc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		LRESULT ret = 0L;

		switch (msg) {
		case WM_CREATE:
			gWindow->winproc_create(hWnd);
			break;

		case WM_DESTROY:
			gWindow->winproc_destroy();
			break;

		case WM_PAINT:
			gWindow->winproc_paint();
			break;

		case WM_LBUTTONDOWN:
			gWindow->winproc_lbuttondown(lParam);
			break;

		case WM_LBUTTONUP:
			gWindow->winproc_lbuttonup(lParam);
			break;

		case WM_MOUSEWHEEL:
			gWindow->winproc_mousewheel(wParam, lParam);
			break;

		case WM_MOUSEMOVE:
			gWindow->winproc_mousemove(lParam);
			break;

		default:
			//デフォルト処理
			ret = ::DefWindowProc(hWnd, msg, wParam, lParam);
			break;
		}

		return ret;
	}
}

int main()
{
	//コンソールウィンドウ生成
	::AllocConsole();
	FILE* fConsole = nullptr;
	freopen_s(&fConsole, "CONOUT$", "w", stdout);

	//ウインドウ作成
	gWindow = new Window();
	int rc = gWindow->create();
	if (rc == 0) {
		//ウィンドウ開始
		rc = gWindow->start();
	}

	//ウインドウ破棄
	delete gWindow;

	//コンソールウィンドウ破棄
	fclose(fConsole);
	//::FreeConsole();	//x64で例外発生

	return rc;
}
