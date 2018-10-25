#include "server_info_dialog.h"

#include <QtWidgets/QApplication>

int main(int argc, char* argv[])
{
	QApplication a(argc, argv);
	ServerInfoDialog w;
	w.show();

	return a.exec();
}
