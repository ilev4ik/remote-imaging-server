#pragma once
#ifndef FILE_WRAPPER_H
#define FILE_WRAPPER_H

#include <string>
#include <vector>

class file_wrapper
{
	typedef char byte_t;
	typedef std::vector<byte_t> raw_file_t;
public:
	file_wrapper(std::wstring name, ::size_t length)
		: m_name(name)
	{
		m_file.resize(length);
	}

	byte_t& operator[](std::size_t i) {
		return m_file[i];
	}

	raw_file_t data() const {
		return m_file;
	}

	std::wstring name() const {
		return m_name;
	}

	std::size_t size() const {
		return m_file.size();
	}

private:
	raw_file_t m_file;
	std::wstring m_name;
};

#endif // FILE_WRAPPER_H