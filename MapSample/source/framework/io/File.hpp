﻿#ifndef INCLUDED_FILE_HPP
#define INCLUDED_FILE_HPP

#include "Std.hpp"
#include <string>

namespace fw {

	//----------------------------------------------------------
	//
	// ファイルクラス
	//
	//----------------------------------------------------------

	class File {
		//メンバ変数
		std::string		filePath_;
		size_t			fileSize_;
		FILE*			fp_;

	public:
		//コンストラクタ
		File();
		//デストラクタ
		~File();
		//作成
		bool create(const std::string& filePath);
		//ファイルオープン
		bool open(const std::string& mode);
		//ファイルクローズ
		void close();
		//ファイル読み込み
		bool read(std::uint8_t** const data, const std::int32_t offset, const size_t size);
		//ファイルサイズ取得
		size_t getFileSize();
	};
}

#endif //INCLUDED_FILE_HPP
