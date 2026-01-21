#ifndef REQUEST_H
#define REQUEST_H

#include <string>
#include <map>

class Request {
public:
    Request();
    ~Request();

    bool parse(const std::string& raw);
    void clear();

    const std::string& get_method() const;
    const std::string& get_uri() const;
    const std::string& get_version() const;
    const std::string& get_body() const;
    const std::string& get_query_string() const;
    std::string get_header(const std::string& key) const;
    const std::map<std::string, std::string>& get_headers() const;
    size_t get_content_length() const;
    bool is_chunked() const;

private:
    std::string m_method;
    std::string m_uri;
    std::string m_path;
    std::string m_query_string;
    std::string m_version;
    std::string m_body;
    std::map<std::string, std::string> m_headers;

    bool parse_request_line(const std::string& line);
    bool parse_headers(const std::string& header_section);
    void parse_uri();
    std::string trim(const std::string& str);
    std::string to_lower(const std::string& str);
};

#endif
