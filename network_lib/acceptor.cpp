#include "acceptor.h"

#include "hive.h"
#include "connection.h"

#include <boost\bind.hpp>
#include <boost\lexical_cast.hpp>
#include <boost\interprocess\detail\atomic.hpp>

using namespace boost;

acceptor::acceptor(shared_ptr<hive> hive)
	: m_hive(hive)
	, m_acceptor(hive->get_service())
	, m_io_strand(hive->get_service())
	, m_timer(hive->get_service())
	, m_timer_interval(1000)
	, m_error_state(0)
{
}

acceptor::~acceptor()
{
}

void acceptor::start_timer()
{
	m_last_time = posix_time::microsec_clock::local_time();
	m_timer.expires_from_now(posix_time::milliseconds(m_timer_interval));
	m_timer.async_wait(m_io_strand.wrap(boost::bind(&acceptor::handle_timer, shared_from_this(), asio::placeholders::error)));
}

void acceptor::start_error(const system::error_code& error)
{
	if (interprocess::ipcdetail::atomic_cas32(&m_error_state, 1, 0) == 0) {
		system::error_code ec;
		m_acceptor.cancel(ec);
		m_acceptor.close(ec);
		m_timer.cancel(ec);
		on_error(error);
	}
}

void acceptor::dispatch_accept(shared_ptr<connection> connection)
{
	m_acceptor.async_accept(
		connection->get_socket(), 
		connection->get_strand().wrap(
			bind(&acceptor::handle_accept, shared_from_this(), _1, connection)
		)
	);
}

void acceptor::handle_timer(const system::error_code& error)
{
	if (error || has_error() || m_hive->has_stopped()) {
		start_error(error);
	} else {
		on_timer(posix_time::microsec_clock::local_time() - m_last_time);
		start_timer();
	}
}

void acceptor::handle_accept(const system::error_code& error, shared_ptr<connection> connection)
{
	if (error || has_error() || m_hive->has_stopped()) {
		connection->start_error(error);
	} else {
		if (connection->get_socket().is_open()) {
			connection->start_timer();

			const auto host_addr = connection->get_socket().remote_endpoint().address().to_string();
			const auto port = connection->get_socket().remote_endpoint().port();

			if (on_accept(connection, host_addr, port)) {
				connection->on_accept(m_acceptor.local_endpoint().address().to_string(), m_acceptor.local_endpoint().port());
			}
		} else {
			start_error(error);
		}
	}
}

void acceptor::stop()
{
	m_io_strand.post(bind(&acceptor::handle_timer, shared_from_this(), asio::error::connection_reset));
}

void acceptor::accept(shared_ptr<connection> connection)
{
	m_io_strand.post(bind(&acceptor::dispatch_accept, shared_from_this(), connection));
}

void acceptor::listen(const std::string& host, const uint16_t& port)
{
	asio::ip::tcp::resolver resolver(m_hive->get_service());
	asio::ip::tcp::resolver::query query(host, lexical_cast<std::string>(port));
	asio::ip::tcp::endpoint endpoint = *resolver.resolve(query);
	m_acceptor.open(endpoint.protocol());
	m_acceptor.set_option(asio::ip::tcp::acceptor::reuse_address(false));
	m_acceptor.bind(endpoint);
	m_acceptor.listen(asio::socket_base::max_connections);
	start_timer();
}

shared_ptr<hive> acceptor::get_hive()
{
	return m_hive;
}

asio::ip::tcp::acceptor& acceptor::get_acceptor()
{
	return m_acceptor;
}

int32_t acceptor::timer_interval() const
{
	return m_timer_interval;
}

void acceptor::set_timer_interval(int32_t timer_interval)
{
	m_timer_interval = timer_interval;
}

bool acceptor::has_error()
{
	return interprocess::ipcdetail::atomic_cas32(&m_error_state, 1, 1) == 1;
}