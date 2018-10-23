#pragma once
#ifndef ACCEPTOR_H
#define ACCEPTOR_H

#include <string>
#include <cstdint>

#include <boost\smart_ptr.hpp>
#include <boost\asio.hpp>

class hive;
class connection;

class acceptor : public boost::enable_shared_from_this<acceptor>
{
	friend class hive;

private:
	boost::shared_ptr<hive> m_hive;
	boost::asio::ip::tcp::acceptor m_acceptor;
	boost::asio::io_service::strand m_io_strand;
	boost::asio::deadline_timer m_timer;
	boost::posix_time::ptime m_last_time;
	int32_t m_timer_interval;
	volatile uint32_t m_error_state;

private:
	acceptor(const acceptor& rhs);
	//acceptor& operator= (const acceptor& rhs);
	void start_timer();
	void start_error(const boost::system::error_code& error);
	void dispatch_accept(boost::shared_ptr<connection> connection);
	void handle_timer(const boost::system::error_code& error);
	void handle_accept(const boost::system::error_code& error, boost::shared_ptr<connection> connection);

protected:
	acceptor(boost::shared_ptr<hive> hive);
	virtual ~acceptor();

private:
	// Called when a connection has connected to the server. This function 
	// should return true to invoke the connection's on_accept function if the 
	// connection will be kept. If the connection will not be kept, the 
	// connection's disconnect function should be called and the function 
	// should return false.
	virtual bool on_accept(boost::shared_ptr<connection> connection, const std::string& host, uint16_t port) = 0;

	// Called on each timer event.
	virtual void on_timer(const boost::posix_time::time_duration& delta) = 0;

	// Called when an error is encountered. Most typically, this is when the
	// acceptor is being closed via the stop function or if the listen is 
	// called on an address that is not available.
	virtual void on_error(const boost::system::error_code& error) = 0;

public:
	// Returns the hive object.
	boost::shared_ptr<hive> get_hive();

	// Returns the acceptor object.
	boost::asio::ip::tcp::acceptor& get_acceptor();

	// Returns the strand object.
	//asio::io_service::strand& get_strand();

	// Sets the timer interval of the object. The interval is changed after 
	// the next update is called. The default value is 1000 ms.
	void set_timer_interval(int32_t timer_interval_ms);

	// Returns the timer interval of the object.
	int32_t timer_interval() const;

	// Returns true if this object has an error associated with it.
	bool has_error();

public:
	// Begin listening on the specific network interface.
	void listen(const std::string& host, const uint16_t& port);

	// Posts the connection to the listening interface. The next client that
	// connections will be given this connection. If multiple calls to accept
	// are called at a time, then they are accepted in a FIFO order.
	void accept(boost::shared_ptr<connection> connection);

	// stop the acceptor from listening.
	void stop();
};

#endif // ACCEPTOR_H