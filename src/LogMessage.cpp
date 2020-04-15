#include "LogMessage.hpp"

#include <charconv>

#include <iostream>
#include <thread>

LogMessage::LogMessage() {
    m_header.get()[HeaderLength] = '\0';
}

LogMessage::LogMessage(std::string_view message) {
    m_bodyLength = message.length();
    m_data = std::shared_ptr<char[]>(new char[m_bodyLength + HeaderLength]);
    message.copy(m_data.get()+HeaderLength, m_bodyLength);
    encodeHeader();
}

char* LogMessage::getData() const { return m_data.get(); }
char* LogMessage::getHeader() const { return m_header.get(); }
char* LogMessage::getBody() const { return m_data.get()+HeaderLength; }
std::string_view LogMessage::getConstBody() const { return std::string_view(m_data.get() + HeaderLength, m_bodyLength); }

std::size_t LogMessage::getBodyLength() const { return m_bodyLength; }
std::size_t LogMessage::getDataLength() const { return m_bodyLength + HeaderLength; }

bool LogMessage::decodeHeader() {
    std::cout << std::this_thread::get_id() << "|Decode header called\n" << std::flush;
    std::cout << std::this_thread::get_id() << "|Decode header m_header: \"" + std::string(m_header.get()) + "\"\n" << std::flush;

    unsigned int bodyLength = atoi(m_header.get());
    
    // if (
    //     auto [p, errorCode] = std::from_chars(m_header.get(), m_header.get()+HeaderLength+1, bodyLength); 
    //     errorCode == std::errc::invalid_argument
    // ) {
    //     std::cout << std::this_thread::get_id() << "|Decode header error1 " << bodyLength << "\n" << std::flush;
    //     return false;
    // }

    if (bodyLength <= 0) {
        std::cout << std::this_thread::get_id() << "|Decode header error2\n" << std::flush;
        return false;
    }

    if (bodyLength > MaxBodyLength) {
        std::cout << std::this_thread::get_id() << "|Decode header error3\n" << std::flush;
        return false;
    }

    std::cout << std::this_thread::get_id() << "|Decode header body length:" << bodyLength << "\n" << std::flush;

    m_data = std::shared_ptr<char[]>(new char[bodyLength + HeaderLength + 1]);
    m_data.get()[0] = ' ';
    m_data.get()[1] = ' ';
    m_data.get()[2] = ' ';
    m_data.get()[3] = ' ';
    m_data.get()[bodyLength + HeaderLength] = '\0';
    m_bodyLength = bodyLength;

    return true;
}

void LogMessage::encodeHeader() {
    std::to_chars(m_data.get(), m_data.get()+HeaderLength, static_cast<int>(m_bodyLength));
}