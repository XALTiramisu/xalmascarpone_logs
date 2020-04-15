#ifndef MASCARPONELOGS_CLIENT_HPP
#define MASCARPONELOGS_CLIENT_HPP

#include <asio/asio.hpp>
#include <deque>

#include "Server.hpp"
#include "LogMessage.hpp"

using asio::ip::tcp;

typedef std::deque<LogMessage> LogMessageQueue;

class Client : public std::enable_shared_from_this<Client>
{
public:
    Client(tcp::socket socket, Server& server);

    void start();
    void deliver(const LogMessage& message);
    void deliver(std::string_view message);

private:
    void doReadHeader();
    void doReadBody();
    void doWrite();

private:
    static const std::string_view& FIRST_CLIENT_MESSAGE;

private:
    tcp::socket m_socket;
    LogMessage m_readMessage;
    LogMessageQueue m_messagesToSendQueue;
    Server& m_server;

    std::string m_appName = "";
};

#endif // MASCARPONELOGS_CLIENT_HPP