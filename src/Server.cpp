#include "Server.hpp"
#include "Client.hpp"

#include <memory>

#include "Global.hpp"

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

void Server::sendMessageToLogProcessor(std::string_view message) {
    m_logsProcessorClient.logMessage(message);
}

void Server::acceptConnection() {
    D(std::cout << "[Server][acceptConnection]" << D_threadID << "Called" << std::endl);

    m_acceptor.async_accept(
        [this](std::error_code errorCode, tcp::socket socket) {
            D(std::cout << "[Server][acceptConnection::async_accept::handler]" << D_threadID << "New connection incoming" << std::endl);

            if (!errorCode) {
                D(std::cout << "[Server][acceptConnection::async_accept::handler]" << D_threadID << "Accepted connection" << std::endl);

                m_sessions.emplace_back(std::make_shared<Client>(std::move(socket), *this));
                m_sessions.back()->start();
            } else {
                D(std::cout << "[Server][acceptConnection::async_accept::handler]" << D_threadID << "Connection with error [" << errorCode << "][" << errorCode.message() << "]" << std::endl);
            }

            acceptConnection();
        }
    );
}