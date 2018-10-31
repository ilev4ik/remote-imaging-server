#pragma once
#ifndef SERVER_INFO_DIALOG_H
#define SERVER_INFO_DIALOG_H

#include "server.h"
#include <boost\smart_ptr.hpp>

#include <QtWidgets\QDialog>

class QTimerEvent;
class QTextEdit;
class QLineEdit;

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

private: 
	std::unique_ptr<server> m_server;

private:
	int m_timer_id{ -1 };
	QTextEdit* m_log_edit{};
	QLineEdit* m_addr{};
	QLineEdit* m_port{};
};

#endif //SERVER_INFO_DIALOG_H