#include "server_info_dialog.h"

#include "server.h"

#include <QTimer>
#include <QIntValidator>
#include <QLineEdit>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <qtextedit.h>

#include <opencv2\opencv.hpp>

using namespace boost;

ServerInfoDialog::ServerInfoDialog(QWidget* parent /*= nullptr*/)
	: QDialog(parent)
	, m_server(std::make_unique<server>())
{
	setWindowTitle(tr("Server monitor"));
	init();
}

void ServerInfoDialog::init()
{
	m_port = new QLineEdit("7777");
	m_port->setClearButtonEnabled(true);
	m_port->setValidator(new QIntValidator(1024, 49151, this));

	m_addr = new QLineEdit("127.0.0.1");
	m_port->setClearButtonEnabled(true);

	auto* edits = new QHBoxLayout;
	edits->addWidget(m_addr);
	edits->addWidget(m_port);

	auto* server_stop_button = new QPushButton(tr("Stop"));
	connect(server_stop_button, &QPushButton::clicked, this, &ServerInfoDialog::stopServer);

	auto* server_start_button = new QPushButton(tr("Start"));
	connect(server_start_button, &QPushButton::clicked, this, &ServerInfoDialog::startServer);

	auto* controls = new QVBoxLayout;
	controls->addLayout(edits);
	controls->addWidget(server_start_button);
	controls->addWidget(server_stop_button);

	m_log_edit = new QTextEdit;

	auto* lt = new QHBoxLayout;
	lt->addLayout(controls);
	lt->addWidget(m_log_edit);
	setLayout(lt);
}

void ServerInfoDialog::stopServer()
{
	m_timer_id = -1;
	m_log_edit->append(tr("stopping server..."));
	m_server->stop();
	killTimer(m_timer_id);
}

void ServerInfoDialog::startServer()
{
	if (m_timer_id >= 0) {
		return;
	}

	const QString addr = m_addr->text();
	const uint16_t port = m_port->text().toInt();

	m_server->start(addr.toStdString(), port);
	m_log_edit->append(tr("starting server at: %1:%2").arg(addr).arg(port));
	m_timer_id = startTimer(1000);
}

void ServerInfoDialog::timerEvent(QTimerEvent* event)
{
	m_log_edit->append(tr("polling received tasks..."));
	QDialog::timerEvent(event);
}