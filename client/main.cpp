#include "client.h"

// std
#include <iostream>
#include <thread>
#include <chrono>
#include <limits>

// boost
#include <boost\program_options.hpp>
#include <boost\filesystem.hpp>
#include <boost\asio\ip\address.hpp>
#include <boost\range.hpp>

// windows
#include <conio.h>

namespace po = boost::program_options;
namespace fs = boost::filesystem;

namespace {

	void connect_to_server(const std::string& host, uint16_t port)
	{
		auto hive_ptr = boost::make_shared<hive>();

		auto connection_ptr = boost::make_shared<tcp_connection>(hive_ptr);
		connection_ptr->connect(host, port);

		while (!_kbhit())
		{
			hive_ptr->poll();
			using namespace std::chrono;
			std::this_thread::sleep_for(1s);
		}

		hive_ptr->stop();

		std::cin.get();
	}
	 
	std::tuple<bool, fs::path, std::string, short> parse_program_args(int argc, char* argv[])
	{
		po::options_description desc("Allowed options");
		desc.add_options()
			("help", "produce help message")
			("imgs-path, I", po::value<std::string>(), "images to process include path")
			("address, a", po::value<std::string>(), "IPv4 address of a server")
			("port, p", po::value<short>(), "Port of a server machine");

		po::variables_map vm;
		try {
			po::store(po::parse_command_line(argc, argv, desc), vm);
			po::notify(vm);
		} catch (const po::error& e) {
			std::cerr << e.what() << std::endl;
			std::cout << std::endl;
			std::cout << desc << std::endl;
			return {false, "", "", -1};
		}

		if (argc != 4 || vm.count("help")) {
			std::cout << desc << std::endl;
			return {false, "", "", -1};
		}

		bool success = true;
		fs::path imgs_path;
		std::string ipv4_addr;
		short port;

		auto img_vv = vm["imgs-path"];
		if (!img_vv.empty()) {
			imgs_path = img_vv.as<std::string>();
			imgs_path.normalize();

			if (!fs::is_directory(imgs_path)) {
				std::cerr << "imgs-path should be a directory!\n";
				success = false;
			}
		} else {
			success = false;
		}

		auto addr_vv = vm["address"];
		if (!addr_vv.empty()) {
			ipv4_addr = addr_vv.as<std::string>();
			boost::system::error_code ec;
			boost::asio::ip::address::from_string(ipv4_addr, ec);
			if (ec) {
				std::cerr << ec.message() << std::endl;
				success = false;
			}
		} else {
			success = false;
		}

		auto port_vv = vm["port"];
		if (!port_vv.empty()) {
			port = port_vv.as<short>();
		} else {
			success = false;
		}

		if (!success) {
			std::cout << "Invalid program arguments!\n" << desc << std::endl;
		}
#ifndef NDEBUG
		std::cout << imgs_path << std::endl
			<< ipv4_addr << std::endl
			<< port << std::endl;
#endif
		return {success, imgs_path, ipv4_addr, port};
	}

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
}

int main(int argc, char* argv[])
{ 
	bool success = true;
	fs::path imgs_path;
	std::string ipv4_addr;
	short port;

	std::tie(success, imgs_path, ipv4_addr, port) = ::parse_program_args(argc, argv);

	if (success) {
		const auto& dir_range = boost::make_iterator_range(fs::directory_iterator(imgs_path), {});
		for (fs::directory_entry entry : dir_range) {
			fs::path p = entry.path();
			if (fs::is_regular_file(p)) {
				auto normalized_abs = fs::absolute(p.normalize());
				std::cout << normalized_abs << std::endl;
				
				::read_file_bytes(normalized_abs.c_str());
			}
		}

		::connect_to_server(ipv4_addr, port);
	}
	
	return 0;
}


