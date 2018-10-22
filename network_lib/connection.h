#pragma once
#ifndef CONNECTION_H
#define CONNECTION_H

#include <cstdint>
#include <string>
#include <vector>
#include <list>

#include <boost\smart_ptr.hpp>
#include <boost\asio.hpp>

class hive;

class connection : public boost::enable_shared_from_this<connection>
{
	friend class acceptor;
	friend class hive;

public:
	// Returns the hive object.
	boost::shared_ptr<hive> get_hive();

	// Returns the socket object.
	boost::asio::ip::tcp::socket& get_socket();

	// Returns the strand object.
	boost::asio::io_service::strand& get_strand();

	// Sets the application specific receive buffer size used. For stream 
	// based protocols such as HTTP, you want this to be pretty large, like 
	// 64kb. For packet based protocols, then it will be much smaller, 
	// usually 512b - 8kb depending on the protocol. The default value is
	// 4kb.
	void set_receive_buffer_size(std::size_t size);

	// Returns the size of the receive buffer size of the current object.
	std::size_t receive_buffer_size() const;

	// Sets the timer interval of the object. The interval is changed after 
	// the next update is called.
	void set_timer_interval(int32_t timer_interval_ms);

	// Returns the timer interval of the object.
	int32_t timer_interval() const;

	// Returns true if this object has an error associated with it.
	bool has_error();

	// Binds the socket to the specified interface.
	void bind(const std::string& ip, uint16_t port);

	// Starts an a/synchronous connect.
	void connect(const std::string& host, uint16_t port);

	// Posts data to be sent to the connection.
	void send(const std::vector<uint8_t>& buffer);

	// Posts a recv for the connection to process. If total_bytes is 0, then 
	// as many bytes as possible up to receive_buffer_size() will be 
	// waited for. If recv is not 0, then the connection will wait for exactly
	// total_bytes before invoking on_recv.
	void recv(std::size_t total_bytes = 0);

	// Posts an asynchronous disconnect event for the object to process.
	void disconnect();

private:
	boost::shared_ptr<hive> m_hive;
	boost::asio::ip::tcp::socket m_socket;
	boost::asio::io_service::strand m_io_strand;
	boost::asio::deadline_timer m_timer;
	boost::posix_time::ptime m_last_time;
	std::vector<uint8_t> m_recv_buffer;
	std::list<std::size_t> m_pending_recvs;
	std::list<std::vector<uint8_t>> m_pending_sends;
	std::size_t m_receive_buffer_size;
	int32_t m_timer_interval;
	volatile uint32_t m_error_state;

protected:
	connection(boost::shared_ptr<hive> hive);
	virtual ~connection();

private:
	connection(const connection& rhs);
	//connection& operator= (const connection& rhs);
	void start_send();
	void start_recv(std::size_t total_bytes);
	void start_timer();
	void start_error(const boost::system::error_code& error);
	void dispatch_send(std::vector<uint8_t> buffer);
	void dispatch_recv(std::size_t total_bytes);
	void dispatch_timer(const boost::system::error_code& error);
	void handle_connect(const boost::system::error_code& error);
	void handle_send(const boost::system::error_code& error, std::list<std::vector<uint8_t>>::iterator itr);
	void handle_recv(const boost::system::error_code& error, std::size_t actual_bytes);
	void handle_timer(const boost::system::error_code& error);

private:
	// Called when the connection has successfully connected to the local
	// host.
	virtual void on_accept(const std::string& host, uint16_t port) = 0;

	// Called when the connection has successfully connected to the remote
	// host.
	virtual void on_connect(const std::string& host, uint16_t port) = 0;

	// Called when data has been sent by the connection.
	virtual void on_send(const std::vector<uint8_t>& buffer) = 0;

	// Called when data has been received by the connection. 
	virtual void on_recv(std::vector<uint8_t>& buffer) = 0;

	// Called on each timer event.
	virtual void on_timer(const boost::posix_time::time_duration& delta) = 0;

	// Called when an error is encountered.
	virtual void on_error(const boost::system::error_code& error) = 0;


};

#endif // CONNECTION_H