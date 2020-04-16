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

private:
    void doReadHeader();
    void doReadBody();

private:
    tcp::socket m_socket;
    LogMessage m_readMessage;
    LogMessageQueue m_messagesToSendQueue;
    Server& m_server;

    std::string m_appName = "";
};

#endif // MASCARPONELOGS_CLIENT_HPP