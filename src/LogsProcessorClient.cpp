#include "LogsProcessorClient.hpp"

#include <iostream>

LogsProcessorClient::LogsProcessorClient(asio::io_context& io_context)
    : m_IOContext {io_context}
    , m_socket {io_context}
{}

void LogsProcessorClient::connect(const std::string& ip, const std::string& port, const std::string& key) {
    tcp::resolver resolver(m_IOContext);
    auto endpoints = resolver.resolve(ip, port);

    asio::async_connect(m_socket, endpoints,
        [this](std::error_code ec, tcp::endpoint)
        {
            if (ec) {
                std::cerr << "Error:" << ec << "\n";
            }

            std::cout << "Connected\n";
        }
    );
}