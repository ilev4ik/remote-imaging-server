// std
#include <iostream>
#include <thread>
#include "app.h"

// boost
#include <boost\filesystem.hpp>
#include <boost\asio\ip\address.hpp>
#include <boost\range.hpp>
#include <boost\filesystem.hpp>

namespace fs = boost::filesystem;

int main(int argc, char* argv[])
{
	bool success = true;
	fs::path imgs_path;
	fs::path dest_imgs_path;
	std::string ipv4_addr;
	short port;

	std::tie(success, imgs_path, std::ignore, ipv4_addr, port) = app::parse_program_args(argc, argv);

	if (!success) {
		return 1;
	}

	std::thread connection_thread{ app::connection_worker(ipv4_addr, port) };
	std::thread file_reader_thread{ app::file_reading_worker(imgs_path) };
	
	file_reader_thread.join();
	connection_thread.join();

	std::cout << "press any key to exit" << std::endl;

	std::cin.get();
	return 0;
}


