/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParser.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: omadali <omadali@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/18 03:10:00 by omadali          #+#    #+#             */
/*   Updated: 2026/01/18 03:10:00 by omadali         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/Config.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <cstdlib>

Config::Config() {}

Config::~Config() {}

// Helper function to trim whitespace
static std::string trim(const std::string& str)
{
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos)
        return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

// Helper to remove comments
static std::string removeComment(const std::string& line)
{
    size_t pos = line.find('#');
    if (pos != std::string::npos)
        return line.substr(0, pos);
    return line;
}

bool Config::load(const std::string& filepath)
{
    std::ifstream file(filepath.c_str());
    if (!file.is_open())
    {
        std::cerr << "Error: Cannot open config file: " << filepath << std::endl;
        return false;
    }

    std::string line;
    int lineNum = 0;
    bool inServerBlock = false;
    bool inLocationBlock = false;
    ServerConfig currentServer;
    LocationConfig currentLocation;

    while (std::getline(file, line))
    {
        lineNum++;
        line = removeComment(line);
        line = trim(line);
        
        if (line.empty())
            continue;

        // Check for server block start
        if (line == "server {" || line == "server{")
        {
            if (inServerBlock)
            {
                std::cerr << "Error: Nested server block at line " << lineNum << std::endl;
                return false;
            }
            inServerBlock = true;
            currentServer = ServerConfig();
            continue;
        }

        // Check for location block start
        if (line.find("location") == 0 && line.find('{') != std::string::npos)
        {
            if (!inServerBlock)
            {
                std::cerr << "Error: Location outside server block at line " << lineNum << std::endl;
                return false;
            }
            inLocationBlock = true;
            // Parse location path
            size_t pathStart = line.find(' ') + 1;
            size_t pathEnd = line.find('{');
            if (pathStart != std::string::npos && pathEnd != std::string::npos)
            {
                currentLocation = LocationConfig();
                currentLocation.path = trim(line.substr(pathStart, pathEnd - pathStart));
            }
            continue;
        }

        // Check for block end
        if (line == "}")
        {
            if (inLocationBlock)
            {
                currentServer.locations.push_back(currentLocation);
                inLocationBlock = false;
            }
            else if (inServerBlock)
            {
                _servers.push_back(currentServer);
                inServerBlock = false;
            }
            continue;
        }

        // Parse directives
        if (inLocationBlock)
        {
            parseLocationDirective(line, currentLocation);
        }
        else if (inServerBlock)
        {
            parseServerDirective(line, currentServer);
        }
    }

    file.close();
    
    if (_servers.empty())
    {
        std::cerr << "Error: No server blocks found in config" << std::endl;
        return false;
    }

    return true;
}

// TODO: Implement these parsing functions
void Config::parseServerDirective(const std::string& line, ServerConfig& server)
{
    std::istringstream iss(line);
    std::string directive;
    iss >> directive;

    // Remove trailing semicolon
    std::string value;
    std::getline(iss, value);
    value = trim(value);
    if (!value.empty() && value[value.size() - 1] == ';')
        value = value.substr(0, value.size() - 1);
    value = trim(value);

    if (directive == "listen")
    {
        server.port = std::atoi(value.c_str());
    }
    else if (directive == "server_name")
    {
        server.server_name = value;
    }
    else if (directive == "root")
    {
        server.root = value;
    }
    else if (directive == "client_max_body_size")
    {
        // TODO: Parse size units (K, M, G)
        server.client_max_body_size = std::atoi(value.c_str());
    }
    else if (directive == "error_page")
    {
        // TODO: Parse error page directive
        // Format: error_page 404 /errors/404.html;
    }
}

void Config::parseLocationDirective(const std::string& line, LocationConfig& location)
{
    std::istringstream iss(line);
    std::string directive;
    iss >> directive;

    std::string value;
    std::getline(iss, value);
    value = trim(value);
    if (!value.empty() && value[value.size() - 1] == ';')
        value = value.substr(0, value.size() - 1);
    value = trim(value);

    if (directive == "index")
    {
        location.index = value;
    }
    else if (directive == "root")
    {
        location.root = value;
    }
    else if (directive == "autoindex")
    {
        location.autoindex = (value == "on");
    }
    else if (directive == "allowed_methods")
    {
        // TODO: Parse multiple methods
        location.allowed_methods.push_back(value);
    }
}

const std::vector<ServerConfig>& Config::getServers() const
{
    return _servers;
}
