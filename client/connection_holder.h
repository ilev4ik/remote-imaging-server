#pragma once
#ifndef CONNECTION_HOLDER_H
#define CONNECTION_HOLDER_H

#include "client.h"

#include <string>
#include <cstdint>

#include <conio.h>

class connection_holder
{
public:
	connection_holder(const std::string& host, uint16_t port)
		: m_hive_ptr(boost::make_shared<hive>())
		, m_connection_ptr(std::make_unique<tcp_connection>(m_hive_ptr))
		, m_host(host)
		, m_port(port)
	{

	}

	void operator()() {
		connect_to_server();
	}

private:
	void connect_to_server()
	{
		try {
			m_connection_ptr->connect(m_host, m_port);

			while (!_kbhit()) {
				m_hive_ptr->poll();
				using namespace std::chrono;
				std::this_thread::sleep_for(1s);
			}

			m_hive_ptr->stop();
		} catch (const std::invalid_argument& e) {
			std::cerr << e.what() << std::endl;
		}
	}

private:
	boost::shared_ptr<hive> m_hive_ptr;
	std::unique_ptr<connection> m_connection_ptr;
	const std::string& m_host;
	uint16_t m_port{0};
};

#endif CONNECTION_HOLDER_H

