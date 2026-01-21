#include "../inc/Request.h"
#include <sstream>
#include <cstdlib>

Request::Request() {}

Request::~Request() {}

void Request::clear() {
    m_method.clear();
    m_uri.clear();
    m_path.clear();
    m_query_string.clear();
    m_version.clear();
    m_body.clear();
    m_headers.clear();
}

bool Request::parse(const std::string& raw) {
    clear();
    
    size_t header_end = raw.find("\r\n\r\n");
    if (header_end == std::string::npos)
        return false;
    
    std::string header_section = raw.substr(0, header_end);
    m_body = raw.substr(header_end + 4);
    
    size_t first_line_end = header_section.find("\r\n");
    if (first_line_end == std::string::npos)
        return false;
    
    std::string request_line = header_section.substr(0, first_line_end);
    if (!parse_request_line(request_line))
        return false;
    
    std::string headers_only = header_section.substr(first_line_end + 2);
    if (!parse_headers(headers_only))
        return false;
    
    parse_uri();
    
    return true;
}

bool Request::parse_request_line(const std::string& line) {
    std::istringstream iss(line);
    if (!(iss >> m_method >> m_uri >> m_version))
        return false;
    return true;
}

bool Request::parse_headers(const std::string& header_section) {
    std::istringstream iss(header_section);
    std::string line;
    
    while (std::getline(iss, line)) {
        if (!line.empty() && line[line.size() - 1] == '\r')
            line.erase(line.size() - 1);
        
        if (line.empty())
            continue;
        
        size_t colon = line.find(':');
        if (colon == std::string::npos)
            continue;
        
        std::string key = to_lower(trim(line.substr(0, colon)));
        std::string value = trim(line.substr(colon + 1));
        m_headers[key] = value;
    }
    
    return true;
}

void Request::parse_uri() {
    m_path = m_uri;
    size_t query_pos = m_uri.find('?');
    if (query_pos != std::string::npos) {
        m_path = m_uri.substr(0, query_pos);
        m_query_string = m_uri.substr(query_pos + 1);
    }
}

std::string Request::trim(const std::string& str) {
    size_t start = 0;
    while (start < str.size() && (str[start] == ' ' || str[start] == '\t'))
        start++;
    
    size_t end = str.size();
    while (end > start && (str[end - 1] == ' ' || str[end - 1] == '\t'))
        end--;
    
    return str.substr(start, end - start);
}

std::string Request::to_lower(const std::string& str) {
    std::string result = str;
    for (size_t i = 0; i < result.size(); i++) {
        if (result[i] >= 'A' && result[i] <= 'Z')
            result[i] = result[i] + 32;
    }
    return result;
}

const std::string& Request::get_method() const {
    return m_method;
}

const std::string& Request::get_uri() const {
    return m_uri;
}

const std::string& Request::get_version() const {
    return m_version;
}

const std::string& Request::get_body() const {
    return m_body;
}

const std::string& Request::get_query_string() const {
    return m_query_string;
}

std::string Request::get_header(const std::string& key) const {
    std::string lower_key = key;
    for (size_t i = 0; i < lower_key.size(); i++) {
        if (lower_key[i] >= 'A' && lower_key[i] <= 'Z')
            lower_key[i] = lower_key[i] + 32;
    }
    
    std::map<std::string, std::string>::const_iterator it = m_headers.find(lower_key);
    if (it != m_headers.end())
        return it->second;
    return "";
}

const std::map<std::string, std::string>& Request::get_headers() const {
    return m_headers;
}

size_t Request::get_content_length() const {
    std::string val = get_header("content-length");
    if (val.empty())
        return 0;
    return std::atol(val.c_str());
}

bool Request::is_chunked() const {
    std::string val = get_header("transfer-encoding");
    return val.find("chunked") != std::string::npos;
}
