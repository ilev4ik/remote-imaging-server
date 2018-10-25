#include "connection.h"
#include "hive.h"

#include <boost\interprocess\detail\atomic.hpp>
#include <boost\bind.hpp>
#include <boost\lexical_cast.hpp>

using namespace boost;

using std::vector;
using std::list;
using std::string;

connection::connection(shared_ptr<hive> hive_ptr)
	: m_hive(hive_ptr)
	, m_socket(hive_ptr->get_service())
	, m_io_strand(hive_ptr->get_service())
	, m_timer(hive_ptr->get_service())
	, m_receive_buffer_size(4096)
	, m_timer_interval(1000)
	, m_error_state(0)
{
}

connection::~connection()
{
}

void connection::bind(const std::string& ip, uint16_t port)
{
	asio::ip::tcp::endpoint endpoint(asio::ip::address::from_string(ip), port);
	m_socket.open(endpoint.protocol());
	m_socket.set_option(asio::ip::tcp::acceptor::reuse_address(false));
	m_socket.bind(endpoint);
}

void connection::start_send()
{
	if (!m_pending_sends.empty()) {
		
		asio::async_write(
			m_socket, 
			asio::buffer(m_pending_sends.front()), 
			m_io_strand.wrap(
				boost::bind(&connection::handle_send, shared_from_this(), asio::placeholders::error, m_pending_sends.begin())
			)
		);	
	}
}

void connection::start_recv(std::size_t total_bytes)
{
	using namespace asio::placeholders;

	if (total_bytes > 0) {
		m_recv_buffer.resize(total_bytes);
		
		asio::async_read(m_socket, asio::buffer(m_recv_buffer), 
			m_io_strand.wrap(
				boost::bind(&connection::handle_recv, shared_from_this(), error, bytes_transferred)
			)
		);
		
	} else {
		m_recv_buffer.resize(m_receive_buffer_size);
		
		m_socket.async_read_some(asio::buffer(m_recv_buffer), m_io_strand.wrap(
				boost::bind(&connection::handle_recv, shared_from_this(), error, bytes_transferred)
			)
		);
	}
}

void connection::start_timer()
{
	using namespace asio::placeholders;
	m_last_time = posix_time::microsec_clock::local_time();
	m_timer.expires_from_now(posix_time::milliseconds(m_timer_interval));
	m_timer.async_wait(
		m_io_strand.wrap(boost::bind(&connection::dispatch_timer, shared_from_this(), error))
	);
}

void connection::start_error(const system::error_code& error)
{
	if (interprocess::ipcdetail::atomic_cas32(&m_error_state, 1, 0) == 0) {
		system::error_code ec;
		m_socket.shutdown(asio::ip::tcp::socket::shutdown_both, ec);
		m_socket.close(ec);
		m_timer.cancel(ec);
		on_error(error);
	}
}

void connection::handle_connect(const system::error_code& error)
{
	if (error || has_error() || m_hive->has_stopped()) {
		start_error(error);
	} else {
		if (m_socket.is_open()) {
			on_connect(m_socket.remote_endpoint().address().to_string(), m_socket.remote_endpoint().port());
		} else {
			start_error(error);
		}
	}
}

void connection::handle_send(const system::error_code& error, std::list<std::vector<char>>::iterator itr)
{
	if (error || has_error() || m_hive->has_stopped()) {
		start_error(error);
	} else {
		on_send(*itr);
		m_pending_sends.erase(itr);
		start_send();
	}
}

void connection::handle_recv(const system::error_code& error, std::size_t actual_bytes)
{
	if (error || has_error() || m_hive->has_stopped()) {
		start_error(error);
	} else {
		m_recv_buffer.resize(actual_bytes);
		on_recv(m_recv_buffer);
		m_pending_recvs.pop_front();
		if (!m_pending_recvs.empty()) {
			start_recv(m_pending_recvs.front());
		}
	}
}

void connection::handle_timer(const system::error_code& error)
{
	if (error || has_error() || m_hive->has_stopped()) {
		start_error(error);
	} else {
		on_timer(posix_time::microsec_clock::local_time() - m_last_time);
		start_timer();
	}
}

void connection::dispatch_send(std::vector<char> buffer)
{
	bool should_start_send = m_pending_sends.empty();
	m_pending_sends.push_back(buffer);
	if (should_start_send) {
		start_send();
	}
}

void connection::dispatch_recv(std::size_t total_bytes)
{
	bool should_start_receive = m_pending_recvs.empty();
	m_pending_recvs.push_back(total_bytes);
	if (should_start_receive) {
		start_recv(total_bytes);
	}
}

void connection::dispatch_timer(const system::error_code& error)
{
	m_io_strand.wrap(boost::bind(&connection::handle_timer, shared_from_this(), error));
}

void connection::connect(const std::string& host, uint16_t port)
{
	using namespace asio::placeholders;
	system::error_code ec;
	asio::ip::tcp::resolver resolver(m_hive->get_service());
	asio::ip::tcp::resolver::query query(host, lexical_cast<std::string>(port));
	asio::ip::tcp::resolver::iterator iterator = resolver.resolve(query, ec);
	if (ec) {
		throw std::invalid_argument(ec.message().c_str());
	}

	m_socket.async_connect(*iterator,
		m_io_strand.wrap(boost::bind(&connection::handle_connect, shared_from_this(), error))
	);
	start_timer();
}

void connection::disconnect()
{
	m_io_strand.post(boost::bind(&connection::handle_timer, shared_from_this(), asio::error::connection_reset));
}

void connection::recv(std::size_t total_bytes)
{
	m_io_strand.post(boost::bind(&connection::dispatch_recv, shared_from_this(), total_bytes));
}

void connection::send(const std::vector<char>& buffer)
{
	m_io_strand.post(boost::bind(&connection::dispatch_send, shared_from_this(), buffer));
}

asio::ip::tcp::socket& connection::get_socket()
{
	return m_socket;
}

asio::io_service::strand& connection::get_strand()
{
	return m_io_strand;
}

shared_ptr<hive> connection::get_hive()
{
	return m_hive;
}

void connection::set_receive_buffer_size(std::size_t size)
{
	m_receive_buffer_size = size;
}

std::size_t connection::receive_buffer_size() const
{
	return m_receive_buffer_size;
}

int32_t connection::timer_interval() const
{
	return m_timer_interval;
}

void connection::set_timer_interval(int32_t timer_interval)
{
	m_timer_interval = timer_interval;
}

bool connection::has_error()
{
	return interprocess::ipcdetail::atomic_cas32(&m_error_state, 1, 1) == 1;
}
