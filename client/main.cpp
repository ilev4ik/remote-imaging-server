// std
#include <iostream>
#include <thread>
#include <chrono>
#include <limits>
#include <mutex>
#include <queue>

// boost
#include <boost\program_options.hpp>
#include <boost\filesystem.hpp>
#include <boost\asio\ip\address.hpp>
#include <boost\range.hpp>

// custom
#include "connection_holder.h"
#include "file_reader.h"
#include "file_wrapper.h"

namespace po = boost::program_options;
namespace fs = boost::filesystem;

namespace {

	std::tuple<bool, fs::path, fs::path, std::string, short> parse_program_args(int argc, char* argv[])
	{
		po::options_description desc("Allowed options");
		desc.add_options()
			("help", "produce help message")
			("imgs-path", po::value<std::string>(), "images to process include path")
			("dest-imgs-path", po::value<std::string>(), "processed images destination folder")
			("address", po::value<std::string>(), "IPv4 address of a server")
			("port", po::value<short>(), "Port of a server machine");

		po::variables_map vm;
		try {
			po::store(po::parse_command_line(argc, argv, desc), vm);
			po::notify(vm);
		} catch (const po::error& e) {
			std::cerr << e.what() << std::endl;
			std::cout << std::endl;
			std::cout << desc << std::endl;
			return {false, "", "", "", -1};
		}

		if (argc != desc.options().size() || vm.count("help")) {
			std::cout << desc << std::endl;
			return {false, "", "", "", -1};
		}

		bool success = true;
		fs::path imgs_path;
		fs::path dest_imgs_path;
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

		auto dest_img_vv = vm["dest-imgs-path"];
		if (!dest_img_vv.empty()) {
			dest_imgs_path = dest_img_vv.as<std::string>();
			dest_imgs_path.normalize();

			if (!fs::is_directory(dest_imgs_path)) {
				std::cerr << "dest_imgs_path should be a directory!\n";
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
		return {success, imgs_path, dest_imgs_path, ipv4_addr, port};
	}

}

file_wrapper read_file_bytes(fs::path file_abs_path)
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

int main(int argc, char* argv[])
{ 
	bool success = true;
	fs::path imgs_path;
	fs::path dest_imgs_path;
	std::string ipv4_addr;
	short port;

	std::tie(success, imgs_path, std::ignore, ipv4_addr, port) = ::parse_program_args(argc, argv);

	if (!success) {
		return 1;
	}

	std::mutex m;
	std::atomic<bool> done = false;
	std::atomic<bool> notified = false;

	std::queue<file_wrapper> produced_files;
	std::condition_variable cv;

	connection_holder holder(ipv4_addr, port);
	try {
		holder.connect_to_server();
	} catch (const std::invalid_argument& e) {
		std::wcerr << "connection error: " << e.what() << std::endl;
		std::cin.get();
		return 1;
	}

	std::thread connection_thread([&]()
	{
		std::unique_lock<std::mutex> lock(m);
		while (!done) {
			while (!notified) {
				cv.wait(lock);
			}
			
			while (!produced_files.empty()) {
				auto&& file = produced_files.front();

				holder.send_file(file);
				std::wcout << file.name() << " with size: " << file.size() << std::endl;
				produced_files.pop();
			}
			
			notified = false;
		}
	});

	std::thread file_reader_thread([&]()
	{
		namespace fs = boost::filesystem;
		auto&& dir_range = boost::make_iterator_range(fs::directory_iterator(imgs_path), {});
		for (fs::directory_entry entry : dir_range) {
			fs::path p = entry.path();
			if (fs::is_regular_file(p)) {
				std::unique_lock<std::mutex> lock(m);
				auto&& file = ::read_file_bytes(fs::absolute(p.normalize()));
				std::wcout << "producing file: " << file.name() << std::endl;
				produced_files.push(file);
				notified = true;
				cv.notify_one();
			}
		}

		done = true;
		cv.notify_one();
	});
	
	file_reader_thread.join();
	connection_thread.join();

	std::cout << "press any key" << std::endl;

	std::cin.get();

	return 0;
}


