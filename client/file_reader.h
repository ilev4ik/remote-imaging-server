#pragma once
#ifndef FILE_READER_H
#define FILE_READER_H

#include <vector>
#include <boost\filesystem.hpp>

class file_reader
{
public:
	file_reader(const boost::filesystem::path& dir_path)
		: m_imgs_path(dir_path)
	{

	}

	void operator()() {
	}

private:


private:
	boost::filesystem::path m_imgs_path;
};

#endif // FILE_READER_H

