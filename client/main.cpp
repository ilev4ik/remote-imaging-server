#include "client.h"

#include <conio.h>

int main(int argc, char * argv[])
{
	auto hive_ptr = boost::make_shared<hive>();

	auto connection_ptr = boost::make_shared<tcp_connection>(hive_ptr);
	connection_ptr->connect("www.google.com", 80);
	
	while(!_kbhit())
	{
		hive_ptr->poll();
		Sleep(1);
	}
	
	hive_ptr->stop();
	
	std::cin.get();

	return 0;
}


