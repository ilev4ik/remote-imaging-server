#pragma once
#ifndef APP_H
#define APP_H

#include "singleton.h"

#include <cstdint>
#include <queue>
#include <boost\filesystem.hpp>

// custom
#include "network.h"
#include "client.h"
#include "file_reader.h"
#include "file_wrapper.h"

class app : public singleton<app>
{
public:
	static std::tuple<bool, boost::filesystem::path, boost::filesystem::path, std::string, short> 
		parse_program_args(int argc, char* argv[]);

	class connection_worker
	{
	public:
		connection_worker(std::string address, uint16_t port);
		void operator()();

	private:
		void send_file(const file_wrapper& fw);
		void connect_to_server(std::string address, uint16_t port);

	private:
		boost::shared_ptr<hive> m_hive_ptr;
		boost::shared_ptr<connection> m_connection_ptr;
		std::string m_addr;
		uint16_t m_port;
	};

	class file_reading_worker
	{
	public:
		file_reading_worker(boost::filesystem::path imgs_path);

		void operator()();

	private:
		boost::filesystem::path m_imgs_path;
	};

private:
	static std::mutex m;
	static std::atomic<bool> done;
	static std::atomic<bool> notified;

	static std::queue<file_wrapper> produced_files;
	static std::condition_variable cv;
};

#endif // APP_H