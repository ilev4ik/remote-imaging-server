#pragma once
#ifndef SERVER_INFO_DIALOG_H
#define SERVER_INFO_DIALOG_H

#include "server.h"

#include <QtWidgets/QDialog>

#include <boost\smart_ptr.hpp>

class QTimerEvent;
class QTextEdit;

class ServerInfoDialog : public QDialog
{
	Q_OBJECT

public:
	ServerInfoDialog(QWidget* parent = nullptr);

protected:
	void timerEvent(QTimerEvent* event) override;

private:
	void init();
	void stopServer();
	void startServer();

private: // network
	boost::shared_ptr<hive> m_hive_ptr;
	boost::shared_ptr<tcp_acceptor> m_acceptor_ptr;
	boost::shared_ptr<tcp_connection> m_connection_ptr;

private:
	int m_timer_id{ -1 };
	QTextEdit* m_log_edit{};
};

#endif //SERVER_INFO_DIALOG_H