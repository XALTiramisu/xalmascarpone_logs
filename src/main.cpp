#include <iostream>
#include <list>
#include <asio/asio.hpp>
#include <ryml.hpp>
#include <ryml_std.hpp>
#include <fstream>
#include <string>
#include <chrono>

#include "Server.hpp"
#include "LogsProcessorClient.hpp"
#include "IOServicePool.hpp"

#include "Global.hpp"

using asio::ip::tcp;
using namespace std::chrono_literals;

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

    D(std::cout << "[main]Main thread ID:" << std::this_thread::get_id() << "\n" << std::flush);

    asio::io_context ioContext;
    IOServicePool ioContextPool(ioContext, threadsCount);

    LogsProcessorClient logsProcessorClient(
        ioContext,
        [&logsProcessorIP, &logsProcessorPort]()->std::tuple<std::string, std::string>{
            return std::make_tuple(logsProcessorIP, logsProcessorPort);
        },
        [&logsProcessorKey]()->std::string{
            return logsProcessorKey;
        }
    );

    Server server(ioContext, tcp::endpoint(tcp::v4(), port), logsProcessorClient);

    while (true) {
        // std::cout << std::this_thread::get_id() << "|Sleeping for 600s more\n" << std::flush;
        std::this_thread::sleep_for(600s);
    }

    return 0;
}