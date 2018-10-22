#include <iostream>
#include <boost\thread.hpp>
#include <boost\asio.hpp>
#include <boost\lexical_cast.hpp>

struct T : std::enable_shared_from_this<T> {};

int main(int agrc, char* argv[])
{
	std::shared_ptr<T> ptr1(new T);
	std::shared_ptr<T> ptr2 = ptr1->shared_from_this();

	std::cout << "ptr1: " << ptr1.use_count() << std::endl;
	std::cout << "ptr2: " << ptr2.use_count() << std::endl;

	std::cin.get();

	return 0;
}