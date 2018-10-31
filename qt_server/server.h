#pragma once
#ifndef SERVER_H
#define SERVER_H

#include "acceptor.h"

#include <boost\shared_ptr.hpp>
#include <vector>
#include <unordered_map>

class hive;

class server
{
	class connection_impl;
	class acceptor_impl;

public:
	server();

	void start(std::string addr, uint16_t port);
	void stop();

private: // logic
	std::unordered_map<int, std::vector<char>> pics;

private:
	boost::shared_ptr<hive> m_hive;
	boost::shared_ptr<acceptor_impl> m_acceptor;
	std::vector<boost::shared_ptr<connection_impl>> m_connection_pool;
};

#endif // SERVER_H

/*


m_connection_ptr->data_received.connect(
[this](const std::vector<char>& data)
{
cv::Mat raw_data(data);
cv::Mat raw_decoded_image = imdecode(raw_data, cv::IMREAD_COLOR);
m_log_edit->append(tr("image [%3] with size w:%1, h:%2")
.arg(raw_decoded_image.cols)
.arg(raw_decoded_image.rows)
.arg(data.size())
);
imshow("test window", raw_decoded_image);
});
*/