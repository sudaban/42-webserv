#include "../inc/Config.h"
#include <fstream>
#include <sstream>
#include <cstdlib>

LocationConfig::LocationConfig()
    : m_path("/")
    , m_root("")
    , m_index("index.html")
    , m_upload_path("")
    , m_redirect("")
    , m_cgi_extension("")
    , m_cgi_path("")
    , m_autoindex(false)
    , m_client_max_body_size(1048576)
{
    m_methods.push_back("GET");
}

ServerConfig::ServerConfig()
    : m_host("0.0.0.0")
    , m_port(8080)
    , m_server_name("localhost")
    , m_root("./www")
    , m_index("index.html")
    , m_client_max_body_size(1048576)
{
}

Config::Config() : m_pos(0) {}

Config::~Config() {}

bool Config::load_file(const std::string& filepath)
{
    std::ifstream file(filepath.c_str());
    if (!file.is_open())
        return false;
    std::stringstream buffer;
    buffer << file.rdbuf();
    m_content = buffer.str();
    file.close();
    return true;
}

void Config::skip_whitespace()
{
    while (m_pos < m_content.size()) {
        char c = m_content[m_pos];
        if (c == ' ' || c == '\t' || c == '\n' || c == '\r') {
            m_pos++;
        } else if (c == '#') {
            while (m_pos < m_content.size() && m_content[m_pos] != '\n')
                m_pos++;
        } else {
            break;
        }
    }
}

std::string Config::get_token()
{
    skip_whitespace();
    if (m_pos >= m_content.size())
        return "";
    
    if (m_content[m_pos] == '{' || m_content[m_pos] == '}' || m_content[m_pos] == ';') {
        return std::string(1, m_content[m_pos++]);
    }
    
    std::string token;
    while (m_pos < m_content.size()) {
        char c = m_content[m_pos];
        if (c == ' ' || c == '\t' || c == '\n' || c == '\r' || 
            c == '{' || c == '}' || c == ';' || c == '#') {
            break;
        }
        token += c;
        m_pos++;
    }
    return token;
}

bool Config::parse(const std::string& filepath)
{
    if (!load_file(filepath))
        return false;
    
    m_pos = 0;
    m_servers.clear();
    
    while (m_pos < m_content.size()) {
        std::string token = get_token();
        if (token.empty())
            break;
        if (token == "server") {
            if (!parse_server())
                return false;
        } else {
            return false;
        }
    }
    
    return !m_servers.empty();
}

bool Config::parse_server()
{
    std::string brace = get_token();
    if (brace != "{")
        return false;
    
    ServerConfig server;
    if (!parse_server_block(server))
        return false;
    
    m_servers.push_back(server);
    return true;
}

bool Config::parse_server_block(ServerConfig& server)
{
    while (m_pos < m_content.size()) {
        std::string token = get_token();
        if (token.empty())
            return false;
        if (token == "}")
            return true;
        
        if (token == "listen") {
            if (!parse_listen(server))
                return false;
        } else if (token == "server_name") {
            server.m_server_name = get_token();
            if (get_token() != ";")
                return false;
        } else if (token == "root") {
            server.m_root = get_token();
            if (get_token() != ";")
                return false;
        } else if (token == "index") {
            server.m_index = get_token();
            if (get_token() != ";")
                return false;
        } else if (token == "client_max_body_size") {
            std::string val = get_token();
            char unit = 0;
            if (!val.empty()) {
                unit = val[val.size() - 1];
                if (unit == 'k' || unit == 'K' || unit == 'm' || unit == 'M') {
                    val = val.substr(0, val.size() - 1);
                }
            }
            server.m_client_max_body_size = std::atol(val.c_str());
            if (unit == 'k' || unit == 'K')
                server.m_client_max_body_size *= 1024;
            else if (unit == 'm' || unit == 'M')
                server.m_client_max_body_size *= 1048576;
            if (get_token() != ";")
                return false;
        } else if (token == "error_page") {
            if (!parse_error_page(server))
                return false;
        } else if (token == "location") {
            if (!parse_location(server))
                return false;
        } else {
            return false;
        }
    }
    return false;
}

bool Config::parse_listen(ServerConfig& server)
{
    std::string val = get_token();
    size_t colon = val.find(':');
    if (colon != std::string::npos) {
        server.m_host = val.substr(0, colon);
        server.m_port = std::atoi(val.substr(colon + 1).c_str());
    } else {
        server.m_port = std::atoi(val.c_str());
    }
    return get_token() == ";";
}

bool Config::parse_error_page(ServerConfig& server)
{
    std::string code_str = get_token();
    std::string path = get_token();
    int code = std::atoi(code_str.c_str());
    server.m_error_pages[code] = path;
    return get_token() == ";";
}

bool Config::parse_location(ServerConfig& server)
{
    LocationConfig location;
    location.m_path = get_token();
    location.m_root = server.m_root;
    location.m_index = server.m_index;
    location.m_client_max_body_size = server.m_client_max_body_size;
    
    std::string brace = get_token();
    if (brace != "{")
        return false;
    
    if (!parse_location_block(location))
        return false;
    
    server.m_locations.push_back(location);
    return true;
}

bool Config::parse_location_block(LocationConfig& location)
{
    while (m_pos < m_content.size()) {
        std::string token = get_token();
        if (token.empty())
            return false;
        if (token == "}")
            return true;
        
        if (token == "root") {
            location.m_root = get_token();
            if (get_token() != ";")
                return false;
        }
		else if (token == "index") {
            location.m_index = get_token();
            if (get_token() != ";")
                return false;
        }
		else if (token == "autoindex") {
            std::string val = get_token();
            location.m_autoindex = (val == "on");
            if (get_token() != ";")
                return false;
        }
		else if (token == "methods" || token == "allow_methods") {
            location.m_methods.clear();
            while (true) {
                std::string method = get_token();
                if (method == ";")
                    break;
                location.m_methods.push_back(method);
            }
        }
		else if (token == "upload_path") {
            location.m_upload_path = get_token();
            if (get_token() != ";")
                return false;
        }
		else if (token == "return" || token == "redirect") {
            location.m_redirect = get_token();
            if (get_token() != ";")
                return false;
        }
		else if (token == "cgi_extension") {
            location.m_cgi_extension = get_token();
            if (get_token() != ";")
                return false;
        }
		else if (token == "cgi_path") {
            location.m_cgi_path = get_token();
            if (get_token() != ";")
                return false;
        }
		else if (token == "client_max_body_size") {
            std::string val = get_token();
            char unit = 0;
            if (!val.empty()) {
                unit = val[val.size() - 1];
                if (unit == 'k' || unit == 'K' || unit == 'm' || unit == 'M') {
                    val = val.substr(0, val.size() - 1);
                }
            }
            location.m_client_max_body_size = std::atol(val.c_str());
            if (unit == 'k' || unit == 'K')
                location.m_client_max_body_size *= 1024;
            else if (unit == 'm' || unit == 'M')
                location.m_client_max_body_size *= 1048576;
            if (get_token() != ";")
                return false;
        }
		else {
            return false;
        }
    }
    return false;
}

const std::vector<ServerConfig>& Config::get_servers() const {
    return m_servers;
}
