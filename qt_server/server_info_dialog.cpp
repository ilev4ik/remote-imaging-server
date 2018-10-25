#include "server_info_dialog.h"

#include "imaging.h"

#include <QTimer>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <qtextedit.h>

using namespace boost;

ServerInfoDialog::ServerInfoDialog(QWidget* parent /*= nullptr*/)
	: QDialog(parent)
	, m_hive_ptr(make_shared<hive>())
	, m_acceptor_ptr(make_shared<tcp_acceptor>(m_hive_ptr))
	, m_connection_ptr(make_shared<tcp_connection>(m_hive_ptr))
{
	init();
}

void ServerInfoDialog::init()
{
	auto* lt = new QHBoxLayout;

	auto* buttons_lt = new QVBoxLayout;
	auto* server_stop_button = new QPushButton(tr("Stop Server"));
	connect(server_stop_button, &QPushButton::clicked, this, &ServerInfoDialog::stopServer);

	auto* server_start_button = new QPushButton(tr("Start Server"));
	connect(server_start_button, &QPushButton::clicked, this, &ServerInfoDialog::startServer);
	
	buttons_lt->addWidget(server_start_button);
	buttons_lt->addWidget(server_stop_button);

	lt->addLayout(buttons_lt);

	m_log_edit = new QTextEdit;
	lt->addWidget(m_log_edit);
	setLayout(lt);
}

void ServerInfoDialog::stopServer()
{
	m_log_edit->append(tr("stopping server..."));
	m_hive_ptr->stop();
	killTimer(m_timer_id);
	m_timer_id = -1;
}

void ServerInfoDialog::startServer()
{
	static const char* addr = "127.0.0.1";
	static int port = 7777;
	m_acceptor_ptr->listen(addr, port);	
	m_acceptor_ptr->accept(m_connection_ptr);

	m_log_edit->append(tr("starting server at: %1:%2").arg(addr).arg(port));
	m_timer_id = startTimer(1000);
}

void ServerInfoDialog::timerEvent(QTimerEvent* event)
{
	m_log_edit->append(tr("polling received tasks..."));
	m_hive_ptr->poll();
	QDialog::timerEvent(event);
}