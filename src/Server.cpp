#include "Server.hpp"
#include "Session.hpp"

#include <memory>

Server::Server(asio::io_context& io_context, const tcp::endpoint& endpoint)
    : m_acceptor(io_context, endpoint)
{
    do_accept();
}

void Server::leave(SessionPtr session) {
    for (auto it = m_sessions.cbegin(); it != m_sessions.cend(); ++it) {
        if (*it == session) m_sessions.erase(it);
    }
}

void Server::do_accept()
{
    m_acceptor.async_accept(
        [this](std::error_code ec, tcp::socket socket)
        {
            if (!ec)
            {
               m_sessions.push_back(std::make_shared<Session>(std::move(socket)));
               m_sessions.back()->start();
            }

            do_accept();
        }
    );
}