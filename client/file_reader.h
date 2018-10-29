#pragma once
#ifndef FILE_READER_H
#define FILE_READER_H

#include "file_wrapper.h"

#include <boost\filesystem.hpp>

static file_wrapper read_file_bytes(boost::filesystem::path file_abs_path)
{
	using namespace std;
	ifstream ifs(file_abs_path.c_str(), ios::binary);

	ifs.seekg(0, ifs.end);
	std::streamoff length = ifs.tellg();
	ifs.seekg(0, ifs.beg);

	file_wrapper file(file_abs_path.filename().c_str(), length);

	ifs.seekg(0, ios::beg);
	ifs.read(&file[0], length);

	return file;
}

#endif // FILE_READER_H

