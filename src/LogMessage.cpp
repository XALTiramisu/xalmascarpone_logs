#include "LogMessage.hpp"

#include "Global.hpp"

#include <charconv>
#include <string.h>

LogMessage::LogMessage() {
    m_data.get()[HeaderLength] = '\0';
}

LogMessage::LogMessage(std::string_view message) {
    m_bodyLength = message.length();
    m_data = std::shared_ptr<char[]>(new char[m_bodyLength + HeaderLength + 1]);
    message.copy(m_data.get()+HeaderLength, m_bodyLength);
    encodeHeader();
}

LogMessage::LogMessage(char* message, std::size_t length) {
    m_bodyLength = length;
    m_data = std::shared_ptr<char[]>(new char[m_bodyLength + HeaderLength + 1]);
    strncpy_s(m_data.get()+HeaderLength, m_bodyLength, message, m_bodyLength);
    encodeHeader();
}

char* LogMessage::getData() const { return m_data.get(); }
char* LogMessage::getHeader() const { return m_data.get(); }
char* LogMessage::getBody() const { return m_data.get()+HeaderLength; }
std::string_view LogMessage::getConstBody() const { return std::string_view(m_data.get() + HeaderLength, m_bodyLength); }

std::size_t LogMessage::getBodyLength() const { return m_bodyLength; }
std::size_t LogMessage::getDataLength() const { return m_bodyLength + HeaderLength; }

bool LogMessage::decodeHeader() {
    D(std::cout << "[LogMessage::decodeHeader]" << D_threadID << "Called to decode the string: \"" << m_data.get() << "\"\n" << std::flush);

    unsigned int bodyLength = atoi(m_data.get());

    if (bodyLength <= 0) {
        D(std::cout << D_threadID << "Decode header error2\n" << std::flush);
        return false;
    }

    if (bodyLength > MaxBodyLength) {
        D(std::cout << D_threadID << "Decode header error3\n" << std::flush);
        return false;
    }

    D(std::cout << "[LogMessage::decodeHeader]" << D_threadID << "Success decode the header. The length of body is: " << bodyLength << "\n" << std::flush);

    m_data = std::shared_ptr<char[]>(new char[bodyLength + HeaderLength + 1]);
    for (int i=0; i < HeaderLength; ++i) m_data.get()[i] = ' ';
    m_data.get()[bodyLength + HeaderLength] = '\0';
    m_bodyLength = bodyLength;

    return true;
}

void LogMessage::encodeHeader() {
    std::to_chars(m_data.get(), m_data.get()+HeaderLength, static_cast<int>(m_bodyLength));
}

void LogMessage::reset(bool hardReset) {
    if (hardReset) {
        m_data = std::shared_ptr<char[]>(new char[HeaderLength+1]);
    }

    m_data.get()[HeaderLength] = '\0';
}