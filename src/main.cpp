#include <iostream>
#include <list>
#include <asio/asio.hpp>
#include <ryml.hpp>
#include <ryml_std.hpp>
#include <fstream>
#include <string>
#include <thread>
#include <chrono>

#include "Server.hpp"
#include "LogsProcessorClient.hpp"

using asio::ip::tcp;
using namespace std::chrono_literals;

class IOServicePool {
public:
    IOServicePool(asio::io_context& ioContext, std::size_t threadsCount) 
        : m_guard {asio::make_work_guard(ioContext)}
    {
        std::cout << std::this_thread::get_id() << "|IOServicePool::constructor()|threadsCount:" << threadsCount << "\n" << std::flush;

        for (std::size_t i=0; i < threadsCount; ++i)
        {
            m_threads.emplace_back(
                [&ioContext]() {
                    std::cout << std::this_thread::get_id() << "|Started asio thread\n" << std::flush;
                    asio::error_code er;
                    ioContext.run(er);
                    std::cout << "Error is:" << er.message() << "\n";
                }
            );
        }
    }

    ~IOServicePool() {
        m_guard.reset();
        std::for_each(
            m_threads.begin(), 
            m_threads.end(), 
            [](std::thread& thread) {
                thread.join();
            }
        );
    }

    IOServicePool(const IOServicePool&) = delete;
    IOServicePool& operator=(const IOServicePool&) = delete;

private:
    std::vector<std::thread> m_threads;
    asio::executor_work_guard<asio::io_context::executor_type> m_guard;
};

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: xalmascarpone_logs [config file path]\n";
        return -1;
    }

    std::string logsProcessorIP;
    std::string logsProcessorPort;
    std::string logsProcessorKey;

    unsigned int port;
    unsigned int threadsCount = 1;
    
    {
        ryml::Tree config;
        std::ifstream configFile(argv[1]);

        if (!configFile.good()) {
            std::cerr << "Can't open the given config file\n";
            return -1;
        }

        std::string content( 
            (std::istreambuf_iterator<char>(configFile)),
            (std::istreambuf_iterator<char>()) 
        );

        configFile.close();

        config = ryml::parse(c4::to_csubstr(content));

        if (!config["logs_processor"].has_key()) {
            std::cerr << "Config file is missing \"logs_procesor\"\n";
            return -1;
        }

        if (!config["logs_processor"]["ip"].has_val()) {
            std::cerr << "Config property \"logs_procesor\" is missing \"ip\"\n";
            return -1;
        }

        if (!config["logs_processor"]["port"].has_val()) {
            std::cerr << "Config property \"logs_procesor\" is missing \"port\"\n";
            return -1;
        }

        if (!config["logs_processor"]["key"].has_val()) {
            std::cerr << "Config property \"logs_procesor\" is missing \"key\"\n";
            return -1;
        }

        c4::from_chars(config["logs_processor"]["ip"].val(), &logsProcessorIP);
        c4::from_chars(config["logs_processor"]["key"].val(), &logsProcessorKey);
        c4::from_chars(config["logs_processor"]["port"].val(), &logsProcessorPort);

        if (!config["port"].has_key()) {
            std::cerr << "Config file is missing \"port\"\n";
            return -1;
        }

        std::string portString;
        c4::from_chars(config["port"].val(), &portString);
        port = std::stoi(portString);

        if (config["asio_threads"].has_val()) {
            std::string threadsCountString;
            c4::from_chars(config["asio_threads"].val(), &threadsCountString);
            threadsCount = std::stoi(threadsCountString);
        }
    }

    std::cout << "Main thread ID:" << std::this_thread::get_id() << "\n" << std::flush;

    asio::io_context ioContext;
    IOServicePool ioContextPool(ioContext, threadsCount);

    try {
        LogsProcessorClient logsProcessorClient(ioContext);
        // logsProcessorClient.connect(logsProcessorIP, logsProcessorPort, logsProcessorKey);

        Server server(ioContext, tcp::endpoint(tcp::v4(), port), logsProcessorClient);

        while (true) {
            // std::cout << std::this_thread::get_id() << "|Sleeping for 600s more\n" << std::flush;
            std::this_thread::sleep_for(600s);
        }
    }
    catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}