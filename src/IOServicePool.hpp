#ifndef MASCARPONELOGS_IOSERVICEPOOL_HPP
#define MASCARPONELOGS_IOSERVICEPOOL_HPP

#include <thread>
#include <cstdlib>
#include <asio/asio.hpp>

class IOServicePool {
public:
    IOServicePool(asio::io_context& ioContext, std::size_t threadsCount);
    ~IOServicePool();
    IOServicePool(const IOServicePool&) = delete;
    IOServicePool& operator=(const IOServicePool&) = delete;

private:
    std::vector<std::thread> m_threads;
    asio::executor_work_guard<asio::io_context::executor_type> m_guard;
};

#endif // MASCARPONELOGS_IOSERVICEPOOL_HPP