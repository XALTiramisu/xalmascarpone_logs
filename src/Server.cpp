#include "Server.hpp"
#include "Client.hpp"

#include <memory>

#include <iostream>

Server::Server(asio::io_context& io_context, const tcp::endpoint& endpoint, LogsProcessorClient& logsProcessorClient)
    : m_acceptor(io_context)
    , m_logsProcessorClient {logsProcessorClient}
{
    m_acceptor.open(endpoint.protocol());
    m_acceptor.set_option(asio::ip::tcp::acceptor::reuse_address(true));
    m_acceptor.bind(endpoint);
    m_acceptor.listen();

    acceptConnection();
}

void Server::leave(ClientPtr session) {
    for (auto it = m_sessions.cbegin(); it != m_sessions.cend(); ++it) {
        if (*it == session) {
            m_sessions.erase(it);
            return;
        }
    }
}

void Server::sendMessageToLogProcessor(const LogMessage& message) {
    // m_logsProcessorClient.
}

void Server::acceptConnection()
{
    std::cout << std::this_thread::get_id() << "|acceptConnection|Entered\n" << std::flush;

    m_acceptor.async_accept(
        [this](std::error_code ec, tcp::socket socket)
        {
            std::cout << std::this_thread::get_id() << "|acceptConnection|New connection, errors: " << ec << ":" << ec.message() << "\n" << std::flush;

            if (!ec)
            {
               m_sessions.emplace_back(std::make_shared<Client>(std::move(socket), *this));
               m_sessions.back()->start();
            } else {

            }

            acceptConnection();
        }
    );
}