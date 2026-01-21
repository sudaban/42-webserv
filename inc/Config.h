/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sdaban <sdaban@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/21 11:26:48 by sdaban            #+#    #+#             */
/*   Updated: 2026/01/21 11:26:58 by sdaban           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <vector>
#include <map>

struct LocationConfig {
    std::string m_path;
    std::string m_root;
    std::string m_index;
    std::string m_upload_path;
    std::string m_redirect;
    std::string m_cgi_extension;
    std::string m_cgi_path;
    std::vector<std::string> m_methods;
    bool m_autoindex;
    size_t m_client_max_body_size;

    LocationConfig();
};

struct ServerConfig {
    std::string m_host;
    int m_port;
    std::string m_server_name;
    std::string m_root;
    std::string m_index;
    size_t m_client_max_body_size;
    std::map<int, std::string> m_error_pages;
    std::vector<LocationConfig> m_locations;

    ServerConfig();
};

class Config {
public:
    Config();
    ~Config();

    bool parse(const std::string& filepath);
    const std::vector<ServerConfig>& get_servers() const;

private:
    std::vector<ServerConfig> m_servers;
    std::string m_content;
    size_t m_pos;

    bool load_file(const std::string& filepath);
    void skip_whitespace();
    std::string get_token();
    bool parse_server();
    bool parse_server_block(ServerConfig& server);
    bool parse_location(ServerConfig& server);
    bool parse_location_block(LocationConfig& location);
    bool parse_error_page(ServerConfig& server);
    bool parse_listen(ServerConfig& server);
};

#endif // CONFIG_H