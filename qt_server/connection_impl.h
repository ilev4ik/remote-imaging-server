#pragma once
#ifndef CONNECTION_IMPL_H
#define CONNECTION_IMPL_H

#include "server.h"
#include "network.h"

#include "boost\signals2.hpp"

class server::connection_impl : public connection
{
public:
	connection_impl(boost::shared_ptr<hive> hive_ptr)
		: connection(hive_ptr)
	{
	}

	virtual ~connection_impl() = default;


public:
	boost::signals2::signal<void(std::vector<char>)> data_received;

private:
	void on_accept(const std::string& host, uint16_t port) override
	{
		// Start the next receive
		recv();
	}

	void on_connect(const std::string& host, uint16_t port) override
	{
		// Start the next receive
		recv();
	}

	void on_send(const std::vector<char>& buffer) override
	{

	}

	void on_recv(std::vector<char>& buffer) override
	{
		data_received(buffer);

		// Start the next receive
		recv();

		// Echo the data back
		//send(buffer);
	}

	void on_timer(const boost::posix_time::time_duration& delta) override
	{

	}

	void on_error(const boost::system::error_code& error) override
	{

	}

};

#endif // CONNECTION_IMPL_H