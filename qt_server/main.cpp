#include "server_info_dialog.h"

#include <QtWidgets/QApplication>

[[noreturn]] void on_terminate()
{
	if (std::uncaught_exception()) {
		try {
			std::rethrow_exception(std::current_exception());
		} catch (...) {}
	}

	std::_Exit(EXIT_FAILURE);
}

int main(int argc, char* argv[])
{
	std::set_terminate(&on_terminate);

	QApplication a(argc, argv);
	ServerInfoDialog w;
	w.show();

	return a.exec();
}
