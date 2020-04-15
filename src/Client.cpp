#include "Client.hpp"

#include <memory>

#include <iostream>

const std::string_view& Client::FIRST_CLIENT_MESSAGE = "name:";

Client::Client(tcp::socket socket, Server& server) 
    : m_socket(std::move(socket))
    , m_server { server }
{
}

void Client::start()
{
    doReadHeader();
}

void Client::deliver(const LogMessage& message)
{
    bool writeInProgress = !m_messagesToSendQueue.empty();
    m_messagesToSendQueue.push_back(message);

    if (!writeInProgress)
    {
        doWrite();
    }
}

void Client::deliver(std::string_view message) {
    deliver(LogMessage(message));
}

void Client::doReadHeader()
{
    auto self(shared_from_this());

    asio::async_read(
        m_socket,
        asio::buffer(m_readMessage.getHeader(), LogMessage::HeaderLength),
        [this, self](std::error_code ec, std::size_t /*length*/)
        {
            std::cout << m_readMessage.getHeader() << "\n" << std::flush;
            if (!ec && m_readMessage.decodeHeader())
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

void Client::doReadBody()
{
    auto self(shared_from_this());

    asio::async_read(
        m_socket,
        asio::buffer(m_readMessage.getBody(), m_readMessage.getBodyLength()),
        [this, self](std::error_code ec, std::size_t /*length*/)
        {
            if (!ec)
            {
                // m_server.deliver(m_readMessage);
                std::cout << "doReadBody() \"" + std::string(m_readMessage.getData()) + "\" " << m_readMessage.getBodyLength() << "\n" << std::flush;
                if (m_appName.size() == 0) {
                    if (m_readMessage.getConstBody().find(FIRST_CLIENT_MESSAGE) != 0) {
                        deliver("Wrong first message");

                        std::cout << "doReadBody() NOT GOOD MESSAGE\n" << std::flush;
                        return;
                    }

                    m_appName = m_readMessage.getConstBody().substr(FIRST_CLIENT_MESSAGE.size());
                    std::cout << "m_appName:" << m_appName << "\n" << std::flush;
                } else {

                }

                doReadHeader();
            }
            else
            {
                m_server.leave(shared_from_this());
            }
        }
    );
}

void Client::doWrite()
{
    auto self(shared_from_this());

    asio::async_write(
        m_socket,
        asio::buffer(
            m_messagesToSendQueue.front().getData(),
            m_messagesToSendQueue.front().getDataLength()
        ),
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