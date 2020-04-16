#ifndef MASCARPONELOGS_LOGSPROCESSORCLIENT_HPP
#define MASCARPONELOGS_LOGSPROCESSORCLIENT_HPP

#include <asio/asio.hpp>
#include <queue>
#include <functional>
#include <thread>
#include <mutex>
#include <atomic>

#include "LogMessage.hpp"

using asio::ip::tcp;

typedef std::function<std::tuple<std::string, std::string>()> GetConnectionDataFunction;
typedef std::function<std::string()> GetEncryptionKeyFunction;

class LogsProcessorClient {
public:
    LogsProcessorClient(asio::io_context& io_context, const GetConnectionDataFunction& getConnectionDataFunction, const GetEncryptionKeyFunction& getEncryptionKeyFunction);
    ~LogsProcessorClient();

    void logMessage(std::string_view message);

private:
    void encryptMessagesLoop();
    void sendMessagesLoop();

private:
    asio::io_context& m_IOContext;
    tcp::socket m_socket;

    std::queue<std::string> m_normalMessages;
    std::queue<std::string> m_encryptedMessages;

    std::thread m_encryptThread;
    std::mutex m_encryptThreadMutex;
    std::atomic<bool> m_runEncryptThread = true;

    std::thread m_sendMessagesThread;
    std::mutex m_sendMessagesThreadMutex;
    std::atomic<bool> m_runSendMessagesThread = true;

    GetConnectionDataFunction m_getConnectionDataFunction;
    GetEncryptionKeyFunction m_getEncryptionKeyFunction;
};

#endif // MASCARPONELOGS_LOGSPROCESSORCLIENT_HPP