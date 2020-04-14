#include "Session.hpp"

#include <memory>

Session::Session(tcp::socket socket, Server& server) 
    : m_socket(std::move(socket))
    , m_server { server }
{
}

void Session::start()
{
    doReadHeader();
}

void Session::deliver(const LogMessage& msg)
{
    bool writeInProgress = !m_messagesToSendQueue.empty();
    m_messagesToSendQueue.push_back(msg);

    if (!writeInProgress)
    {
        doWrite();
    }
}

void Session::doReadHeader()
{
    auto self(shared_from_this());

    asio::async_read(
        m_socket,
        asio::buffer(m_readMessage.data(), LogMessage::header_length),
        [this, self](std::error_code ec, std::size_t /*length*/)
        {
            if (!ec && m_readMessage.decode_header())
            {
                doReadBody();
            }
            else
            {
                m_server.leave(shared_from_this());
            }
        }
    );
}

void Session::doReadBody()
{
    auto self(shared_from_this());

    asio::async_read(
        m_socket,
        asio::buffer(m_readMessage.body(), m_readMessage.body_length()),
        [this, self](std::error_code ec, std::size_t /*length*/)
        {
            if (!ec)
            {
                m_server.deliver(m_readMessage);
                doReadHeader();
            }
            else
            {
                m_server.leave(shared_from_this());
            }
        }
    );
}

void Session::doWrite()
{
    auto self(shared_from_this());

    asio::async_write(
        m_socket,
        asio::buffer(m_messagesToSendQueue.front().data(),
        m_messagesToSendQueue.front().length()),
        [this, self](std::error_code ec, std::size_t /*length*/)
        {
            if (!ec)
            {
                m_messagesToSendQueue.pop_front();

                if (!m_messagesToSendQueue.empty())
                {
                    doWrite();
                }
            }
            else
            {
                m_server.leave(shared_from_this());
            }
        }
    );
}