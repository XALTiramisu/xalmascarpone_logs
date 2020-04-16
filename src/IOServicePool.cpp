#include "IOServicePool.hpp"

#include "Global.hpp"

#include <iostream>

IOServicePool::IOServicePool(asio::io_context& ioContext, std::size_t threadsCount) 
    : m_guard {asio::make_work_guard(ioContext)}
{
    D(std::cout << "[IOServicePool][constructor]" << D_threadID << "Called with threadsCount:" << threadsCount << std::endl);

    for (std::size_t i=0; i < threadsCount; ++i)
    {
        m_threads.emplace_back(
            [&ioContext]() {
                D(std::cout << "[IOServicePool][constructor::thread]" << D_threadID << "Started asio thread" << std::endl);

                asio::error_code errorCode;
                ioContext.run(errorCode);
                
                if (errorCode) {
                    std::cerr << "[IOServicePool][constructor::thread]Exit with error [" << errorCode << "][" << errorCode.message() << "]" << std::endl;
                }
            }
        );
    }
}

IOServicePool::~IOServicePool() {
    D(std::cout << "[IOServicePool][destructor]" << D_threadID << "Called" << std::endl);

    m_guard.reset();
    std::for_each(
        m_threads.begin(), 
        m_threads.end(), 
        [](std::thread& thread) {
            thread.join();
        }
    );
}