#pragma once
#ifndef CONNECTION_HOLDER_H
#define CONNECTION_HOLDER_H

#include "client.h"
#include "file_wrapper.h"

#include <string>
#include <cstdint>

#include <conio.h>

class connection_holder
{
public:
	connection_holder(const std::string& host, uint16_t port)
		: m_hive_ptr(boost::make_shared<hive>())
		, m_connection_ptr(boost::make_shared<tcp_connection>(m_hive_ptr))
		, m_host(host)
		, m_port(port)
	{

	}

	void operator()() {
		using namespace std::chrono;
		connect_to_server();

		while (!_kbhit()) {
			m_hive_ptr->poll();
			std::this_thread::sleep_for(1s);
		}

		m_hive_ptr->stop();
	}

	void connect_to_server()
	{
		m_connection_ptr->connect(m_host, m_port);
	}

	void send_file(const file_wrapper& fw)
	{
		m_connection_ptr->send(fw.data());
		m_hive_ptr->poll();
	}

private:
	boost::shared_ptr<hive> m_hive_ptr;
	boost::shared_ptr<connection> m_connection_ptr;
	const std::string& m_host;
	uint16_t m_port{0};
};

#endif CONNECTION_HOLDER_H

