#ifndef MASCARPONELOGS_SERVER_HPP
#define MASCARPONELOGS_SERVER_HPP

#include <asio/asio.hpp>
#include <vector>

#include "LogsProcessorClient.hpp"
#include "LogMessage.hpp"

class Client;

typedef std::shared_ptr<Client> ClientPtr;
using asio::ip::tcp;

class Server
{
public:
    Server(asio::io_context& io_context, const tcp::endpoint& endpoint, LogsProcessorClient& logsProcessorClient);

    void leave(ClientPtr session);
    void sendMessageToLogProcessor(std::string_view message);

private:
    void acceptConnection();

    tcp::acceptor m_acceptor;
    std::vector<ClientPtr> m_sessions;

    LogsProcessorClient& m_logsProcessorClient;
};

#endif // MASCARPONELOGS_SERVER_HPP