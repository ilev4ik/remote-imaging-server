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
		namespace fs = boost::filesystem;
		auto&& dir_range = boost::make_iterator_range(fs::directory_iterator(m_imgs_path), {});
		for (fs::directory_entry entry : dir_range) {
			fs::path p = entry.path();
			if (fs::is_regular_file(p)) {
				auto normalized_abs = fs::absolute(p.normalize());
				std::cout << normalized_abs << std::endl;

				std::vector<char> raw_jpeg = read_file_bytes(normalized_abs.c_str());
			}
		}
	}

private:
	std::vector<char> read_file_bytes(const wchar_t* file_abs_path)
	{
		using namespace std;
		ifstream ifs(file_abs_path, ios::binary);

		ifs.seekg(0, ifs.end);
		std::streamoff length = ifs.tellg();
		ifs.seekg(0, ifs.beg);

		std::vector<char> result(length);

		ifs.seekg(0, ios::beg);
		ifs.read(&result[0], length);

		return result;
	}

private:
	boost::filesystem::path m_imgs_path;
};

#endif // FILE_READER_H

