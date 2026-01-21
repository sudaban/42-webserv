#include "../inc/Server.h"
#include "../inc/Config.h"
#include <iostream>
#include <cstdlib>

int main(int argc, char** argv)
{
    std::string conf_path = "webserv.conf"; // default
     if (argc > 2) {
        std::cerr << "Usage: " << argv[0] << " [configuration file]" << std::endl;
        return 1;
    }
    if (argc == 2) {
        conf_path = argv[1];
    }

    Config config;
    if (!config.parse(conf_path)) {
        std::cerr << "Error: Failed to parse configuration file: " << conf_path << std::endl;
        return 1;
    }
    else std::cout << "Config parsed succesfully" << std::endl;

    int port = config.get_servers()[0].m_port;
    std::string docroot = config.get_servers()[0].m_root; 

    Server server(port, docroot);
    if (!server.setup()) return 1;
    server.run();

    return 0;
}
