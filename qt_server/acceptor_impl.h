#pragma once
#ifndef ACCEPTOR_IMPL_H
#define ACCEPTOR_IMPL_H

#include "network.h"
#include "server.h"

class server::acceptor_impl : public acceptor
{
public:
	acceptor_impl(boost::shared_ptr<hive> hive_ptr)
		: acceptor(hive_ptr)
	{
	}

	virtual ~acceptor_impl() = default;

private:
	bool on_accept(boost::shared_ptr<connection> connection, const std::string& host, uint16_t port) override
	{
		return true;
	}

	void on_timer(const boost::posix_time::time_duration& delta) override
	{
		return;
	}

	void on_error(const boost::system::error_code& error) override
	{
		return;
	}

};

#endif // ACCEPTOR_IMPL_H