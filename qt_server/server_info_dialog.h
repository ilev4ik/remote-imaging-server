#pragma once
#ifndef SERVER_INFO_DIALOG_H
#define SERVER_INFO_DIALOG_H

#include <QtWidgets/QDialog>

class ServerInfoDialog : public QDialog
{
	Q_OBJECT

public:
	ServerInfoDialog(QWidget* parent = nullptr);

};

#endif //SERVER_INFO_DIALOG_H