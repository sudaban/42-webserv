/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: omadali <omadali@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/18 00:35:00 by omadali          #+#    #+#             */
/*   Updated: 2026/01/18 00:35:00 by omadali         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <vector>
#include <map>

// TODO: Implement location block parsing
// TODO: Add error page configuration
// TODO: Handle multiple server blocks

struct LocationConfig
{
    std::string path;
    std::string root;
    std::string index;
    bool autoindex;
    std::vector<std::string> allowed_methods;
    
    LocationConfig() : autoindex(false) {}
};

struct ServerConfig
{
    int port;
    std::string server_name;
    std::string root;
    size_t client_max_body_size;
    std::map<int, std::string> error_pages;
    std::vector<LocationConfig> locations;
    
    ServerConfig() : port(8080), client_max_body_size(1048576) {}
};

class Config
{
public:
    Config();
    ~Config();
    
    bool load(const std::string& filepath);
    const std::vector<ServerConfig>& getServers() const;
    
private:
    std::vector<ServerConfig> _servers;
    
    void parseServerDirective(const std::string& line, ServerConfig& server);
    void parseLocationDirective(const std::string& line, LocationConfig& location);
};

#endif
