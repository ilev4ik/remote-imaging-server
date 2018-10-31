#pragma once
#ifndef HIVE_H
#define HIVE_H

#include <cstdint>
#include <string>
#include <vector>
#include <list>

#include <boost\smart_ptr.hpp>
#include <boost\asio.hpp>

class hive : public boost::enable_shared_from_this<hive>
{
private:
	boost::asio::io_service m_io_service;
	boost::shared_ptr<boost::asio::io_service::work> m_work_ptr;
	volatile uint32_t m_shutdown;

	hive(const hive& rhs) = delete;
	hive& operator= (const hive& rhs) = delete;

public:
	hive();
	virtual ~hive() = default;

	// Returns the io_service of this object.
	boost::asio::io_service& get_service();

	// Returns true if the stop function has been called.
	bool has_stopped();

	// Polls the networking subsystem once from the current thread and 
	// returns.
	void poll();

	// Runs the networking system on the current thread. This function blocks 
	// until the networking system is stopped, so do not call on a single 
	// threaded app with no other means of being able to call stop 
	// unless you code in such logic.
	void run();

	// Stops the networking system. All work is finished and no more 
	// networking interactions will be possible afterwards until reset is called.
	void stop();

	// Restarts the networking system after stop as been called. A new work
	// object is created ad the shutdown flag is cleared.
	void reset();
};


#endif // HIVE_H