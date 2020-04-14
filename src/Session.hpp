#ifndef MASCARPONELOGS_SESSION_HPP
#define MASCARPONELOGS_SESSION_HPP

#include <asio/asio.hpp>
#include <deque>

#include "Server.hpp"
#include "LogMessage.hpp"

using asio::ip::tcp;

typedef std::deque<LogMessage> LogMessageQueue;

class Session : public std::enable_shared_from_this<Session>
{
public:
    Session(tcp::socket socket, Server& server);

    void start();
    void deliver(const LogMessage& msg);
    void leave(chat_participant_ptr session);

private:
    void doReadHeader();
    void doReadBody();
    void doWrite();

    tcp::socket m_socket;
    LogMessage m_readMessage;
    LogMessageQueue m_messagesToSendQueue;
    Server& m_server;
};

#endif // MASCARPONELOGS_SESSION_HPP