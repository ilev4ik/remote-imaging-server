#pragma once
#ifndef CLIENT_H
#define CLIENT_H

#include "network.h"

#include <boost\thread\mutex.hpp>
#include <iostream>
#include <iomanip>
#include <boost\date_time\posix_time\posix_time.hpp>

static boost::mutex global_stream_lock;

class tcp_connection : public connection
{
public:
	tcp_connection(boost::shared_ptr<hive> hive_ptr)
		: connection(hive_ptr)
	{
	}

	virtual ~tcp_connection() = default;

private:
	void on_accept(const std::string& host, uint16_t port) override
	{
		global_stream_lock.lock();
		std::cout << "[" << __FUNCTION__ << "] " << host << ":" << port << std::endl;
		global_stream_lock.unlock();

		// Start the next receive
		recv();
	}

	void on_connect(const std::string& host, uint16_t port) override
	{
		global_stream_lock.lock();
		std::cout << "[" << __FUNCTION__ << "] " << host << ":" << port << std::endl;
		global_stream_lock.unlock();

		// Start the next receive
		recv();

		std::string str = "GET / HTTP/1.0\r\n\r\n";

		std::vector<char> request;
		std::copy(str.begin(), str.end(), std::back_inserter(request));
		send(request);
	}

	void on_send(const std::vector<char>& buffer) override
	{
		global_stream_lock.lock();
		std::cout << "[" << __FUNCTION__ << "] " << buffer.size() << " bytes" << std::endl;
		for (size_t x = 0; x < buffer.size(); ++x) {
			std::cout << std::hex << std::setfill('0') << 
				std::setw(2) << (int)buffer[x] << " ";
			if ((x + 1) % 16 == 0) {
				std::cout << std::endl;
			}
		}
		std::cout << std::endl;
		global_stream_lock.unlock();
	}

	void on_recv(std::vector<char>& buffer) override
	{
		global_stream_lock.lock();
		std::cout << "[" << __FUNCTION__ << "] " << buffer.size() << " bytes" << std::endl;
		for (size_t x = 0; x < buffer.size(); ++x) {
			std::cout << std::hex << std::setfill('0') << 
				std::setw(2) << (int)buffer[x] << " ";
			if ((x + 1) % 16 == 0) {
				std::cout << std::endl;
			}
		}
		std::cout << std::endl;
		global_stream_lock.unlock();

		// Start the next receive
		recv();
	}

	void on_timer(const boost::posix_time::time_duration& delta) override
	{
		global_stream_lock.lock();
		std::cout << "[" << __FUNCTION__ << "] " << delta << std::endl;
		global_stream_lock.unlock();
	}

	void on_error(const boost::system::error_code& error) override
	{
		global_stream_lock.lock();
		std::cout << "[" << __FUNCTION__ << "] " << error << std::endl;
		global_stream_lock.unlock();
	}

};

#endif // CLIENT_H
