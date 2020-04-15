#ifndef MASCARPONELOGS_LOGSPROCESSORCLIENT_HPP
#define MASCARPONELOGS_LOGSPROCESSORCLIENT_HPP

#include <asio/asio.hpp>

using asio::ip::tcp;

class LogsProcessorClient {
public:
    LogsProcessorClient(asio::io_context& io_context);

    void connect(const std::string& ip, const std::string& port, const std::string& key);

private:
    asio::io_context& m_IOContext;
    tcp::socket m_socket;
};

#endif // MASCARPONELOGS_LOGSPROCESSORCLIENT_HPP