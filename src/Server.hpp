#ifndef MASCARPONELOGS_SERVER_HPP
#define MASCARPONELOGS_SERVER_HPP

#include <asio/asio.hpp>
#include <vector>

using asio::ip::tcp;

class Session;
typedef std::shared_ptr<Session> SessionPtr;

class Server
{
public:
    Server(asio::io_context& io_context, const tcp::endpoint& endpoint);

    void leave(SessionPtr session);

private:
    void do_accept();

    tcp::acceptor m_acceptor;
    std::vector<SessionPtr> m_sessions;
};

#endif // MASCARPONELOGS_SERVER_HPP