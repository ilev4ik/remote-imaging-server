#include "server.h"

#include <conio.h>

int main(int argc, char* argv[])
{
	auto hive_ptr = boost::make_shared<hive>();
	auto acceptor_ptr = boost::make_shared<tcp_acceptor>(hive_ptr);
	acceptor_ptr->listen("127.0.0.1", 7777);
	
	auto connection_ptr = boost::make_shared<tcp_connection>(hive_ptr);
	acceptor_ptr->accept(connection_ptr);
	
	while(!_kbhit())
	{
		hive_ptr->poll();
		Sleep(1);
	}
	
	hive_ptr->stop();
	
	std::cin.get();

	return 0;
}



