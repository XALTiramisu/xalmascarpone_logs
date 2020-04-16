#include "LogsProcessorClient.hpp"

#include <chrono>
#include <iostream>
#include <stdexcept>

#include "Global.hpp"

#include "cryptopp/cryptlib.h"
#include "cryptopp/secblock.h"
#include "cryptopp/chacha.h"
#include "cryptopp/files.h"

using namespace std::chrono_literals;

LogsProcessorClient::LogsProcessorClient(asio::io_context& io_context, const GetConnectionDataFunction& getConnectionDataFunction, const GetEncryptionKeyFunction& getEncryptionKeyFunction)
    : m_IOContext {io_context}
    , m_socket {io_context}
    , m_getConnectionDataFunction {getConnectionDataFunction}
    , m_getEncryptionKeyFunction {getEncryptionKeyFunction}
{
    m_encryptThread = std::thread(&LogsProcessorClient::encryptMessagesLoop, this);
    m_sendMessagesThread = std::thread(&LogsProcessorClient::sendMessagesLoop, this);
}

LogsProcessorClient::~LogsProcessorClient() {
    m_socket.shutdown(asio::ip::tcp::socket::shutdown_send);
    m_socket.close();

    if (m_encryptThread.joinable()) m_encryptThread.join();
}

void LogsProcessorClient::logMessage(std::string_view message) {
    std::string copyOfMessage(message);

    const std::lock_guard<std::mutex> lock(m_encryptThreadMutex);
    m_normalMessages.emplace(message.data());
}

void LogsProcessorClient::encryptMessagesLoop() {
    D(std::cout << "[LogsProcessorClient][encryptMessagesLoop]" << D_threadID << "Started the encrypt messages loop" << std::endl);

    const std::string encryptionKey = m_getEncryptionKeyFunction();
    
    if (encryptionKey.size() != 40) {
        throw std::invalid_argument("Encryption key length must be 40");
    }

    CryptoPP::SecByteBlock key(reinterpret_cast<const CryptoPP::byte*>(&encryptionKey[0]), 32);
    CryptoPP::SecByteBlock iv(reinterpret_cast<const CryptoPP::byte*>(&encryptionKey[32]), 8);

    const CryptoPP::AlgorithmParameters params = 
        CryptoPP::MakeParameters(CryptoPP::Name::Rounds(), 8)
            (CryptoPP::Name::IV(), CryptoPP::ConstByteArrayParameter(iv, 8));

    CryptoPP::ChaCha::Encryption enc;    
    enc.SetKey(key, key.size(), params);

    std::size_t messagesToEncryptCount = 0;
    std::string *encryptedMessages = nullptr;
    while (m_runEncryptThread) {
        {
            const std::lock_guard<std::mutex> lock(m_encryptThreadMutex);
            messagesToEncryptCount = m_normalMessages.size();
        
            if (messagesToEncryptCount) {
                encryptedMessages = new std::string[messagesToEncryptCount];

                for (std::size_t i=0; i < messagesToEncryptCount; ++i) {
                    enc.Resynchronize(iv, iv.size());
                    CryptoPP::StringSource ss(
                        m_normalMessages.front(), 
                        true, 
                        new CryptoPP::StreamTransformationFilter(enc, new CryptoPP::StringSink(encryptedMessages[i]))
                    );

                    m_normalMessages.pop();
                }
            }
        }

        if (messagesToEncryptCount) {
            const std::lock_guard<std::mutex> lock(m_sendMessagesThreadMutex);

            for (std::size_t i=0; i < messagesToEncryptCount; ++i) {
                m_encryptedMessages.push(encryptedMessages[i]);
            }

            delete encryptedMessages;
        }

        std::this_thread::sleep_for(100ms);
    }
}

void LogsProcessorClient::sendMessagesLoop() {
    D(std::cout << "[LogsProcessorClient][sendMessagesLoop]" << D_threadID << "Started the sending messages to processor loop" << std::endl);

    bool isConnected = false;

    while (m_runSendMessagesThread) {
        if (!isConnected) {
            const auto connectionData = m_getConnectionDataFunction();

            std::string_view ip = std::get<0>(connectionData);
            std::string_view port = std::get<1>(connectionData);

            D(std::cout << "[LogsProcessorClient][sendMessagesLoop]" << D_threadID << "Connecting with ip:\"" + std::string(ip) + "\", port:\"" + std::string(port) + "\"" << std::endl);

            tcp::resolver resolver(m_IOContext);
            auto endpoints = resolver.resolve(ip, port);

            std::error_code errorCode;
            asio::connect(m_socket, endpoints, errorCode);

            if (errorCode) {
                std::cerr << "[LogsProcessorClient][sendMessagesLoop]Error connecting. Error [" + std::to_string(errorCode.value()) + "][" + errorCode.message() + "]" << std::endl;
            } else {
                isConnected = true;
            }
        } else {
            const std::lock_guard<std::mutex> lock(m_sendMessagesThreadMutex);

            for (std::size_t i=0; i < m_encryptedMessages.size(); ++i) {
                std::string_view messageToSend = m_encryptedMessages.front();

                std::error_code errorCode;
                m_socket.write_some(asio::buffer(messageToSend.data(), messageToSend.length()), errorCode);

                if (errorCode) {
                    std::cerr << "[LogsProcessorClient][sendMessagesLoop]Error sending message. Error [" + std::to_string(errorCode.value()) + "][" + errorCode.message() + "]" << std::endl;

                    if (errorCode.value() == 10054) {
                        isConnected = false;
                        break;
                    }
                } else {
                    m_encryptedMessages.pop();
                }
            }
        }

        std::this_thread::sleep_for(100ms);
    }
}