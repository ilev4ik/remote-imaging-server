#include "hive.h"

#include <boost\interprocess\detail\atomic.hpp>

using namespace boost;

hive::hive()
	: m_work_ptr(new asio::io_service::work(m_io_service))
	, m_shutdown(0)
{
}

asio::io_service& hive::get_service()
{
	return m_io_service;
}

bool hive::has_stopped()
{
	return interprocess::ipcdetail::atomic_cas32(&m_shutdown, 1, 1) == 1;
}

void hive::poll()
{
	m_io_service.poll();
}

void hive::run()
{
	m_io_service.run();
}

void hive::stop()
{
	if (interprocess::ipcdetail::atomic_cas32(&m_shutdown, 1, 0) == 0) {
		m_work_ptr.reset();
		m_io_service.run();
		m_io_service.stop();
	}
}

void hive::reset()
{
	if (interprocess::ipcdetail::atomic_cas32(&m_shutdown, 0, 1) == 1) {
		m_io_service.reset();
		m_work_ptr.reset(new asio::io_service::work(m_io_service));
	}
}
