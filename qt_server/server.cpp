#include "server.h"
#include "imaging.h"

#include "hive.h"
#include "connection_impl.h"
#include "acceptor_impl.h"

#include <boost\range\algorithm.hpp>
//#include <boost\range\adaptors.hpp>

using namespace boost;

server::server()
	: m_hive(make_shared<hive>())
	, m_acceptor(make_shared<acceptor_impl>(m_hive))
{

}

void server::start(std::string addr, uint16_t port)
{
	m_acceptor->listen(addr, port);	
	m_connection_pool.push_back(make_shared<connection_impl>(m_hive));
	m_acceptor->accept(m_connection_pool.back());
	m_hive->poll();
}

void server::stop()
{
	m_acceptor->stop();
	m_hive->poll();
	for_each(m_connection_pool, std::mem_fn(&connection_impl::disconnect));
	m_connection_pool.clear();
}