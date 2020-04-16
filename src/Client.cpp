#include "Client.hpp"

#include <memory>

#include "Global.hpp"

Client::Client(tcp::socket socket, Server& server) 
    : m_socket(std::move(socket))
    , m_server { server }
{
}

void Client::start() {
    doReadHeader();
}

void Client::doReadHeader()
{
    auto self(shared_from_this());

    m_readMessage.reset();

    asio::async_read(
        m_socket,
        asio::buffer(m_readMessage.getHeader(), LogMessage::HeaderLength),
        [this, self](std::error_code ec, std::size_t /*length*/)
        {
            
            D(std::cout << "[Client][doReadHeader::async_read::handler]" << D_threadID << "Recieved new message with header \"" << m_readMessage.getHeader() << "\"\n" << std::flush);
            
            if (!ec && m_readMessage.decodeHeader())
            {
                doReadBody();
            }
            else
            {
                if (ec.value() != 10054) {
                    D(std::cout << "Error: " << ec.value() << " " << ec.message() << std::endl);
                }

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
        [this, self](std::error_code errorCode, std::size_t /*length*/)
        {
            if (!errorCode)
            {
                
                D(std::cout << "[Client][doReadBody::async_read::handler]" << D_threadID << " Received message body: \"" << m_readMessage.getConstBody() << "\" with size of " << m_readMessage.getBodyLength() << "bytes\n" << std::flush);
                m_server.sendMessageToLogProcessor(m_readMessage.getConstBody());

                // Wait for new messages
                doReadHeader();
            }
            else
            {
                m_server.leave(shared_from_this());
            }
        }
    );
}