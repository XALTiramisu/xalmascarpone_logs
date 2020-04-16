#ifndef MASCARPONELOGS_LOGMESSAGE_HPP
#define MASCARPONELOGS_LOGMESSAGE_HPP

#include <cstdlib>
#include <memory>

class LogMessage
{
public:
    static const std::size_t HeaderLength = 4;
    static const std::size_t AppNameLength = 4;
    static const std::size_t MaxBodyLength = 9999;

public:
    LogMessage();
    LogMessage(std::string_view message);
    LogMessage(char* message, std::size_t length);

    char* getData() const;
    char* getHeader() const;
    char* getBody() const;
    std::string_view getConstBody() const;

    std::size_t getBodyLength() const;

    std::size_t getDataLength() const;

    bool decodeHeader();
    void encodeHeader();

    void reset(bool hardReset = false);
private:
    

private:
    std::shared_ptr<char[]> m_data = std::shared_ptr<char[]>(new char[HeaderLength+1]);
    std::size_t m_bodyLength = 0;
};

#endif // MASCARPONELOGS_LOGMESSAGE_HPP