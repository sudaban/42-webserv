/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: omadali <omadali@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/18 07:55:00 by omadali          #+#    #+#             */
/*   Updated: 2026/01/18 07:55:00 by omadali         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_H
#define REQUEST_H

#include <string>
#include <map>

class Request
{
public:
    Request();
    ~Request();

    // Parse raw HTTP request
    bool parse(const std::string& raw_request);
    
    // Getters
    const std::string& getMethod() const;
    const std::string& getPath() const;
    const std::string& getVersion() const;
    const std::string& getBody() const;
    std::string getHeader(const std::string& name) const;
    size_t getContentLength() const;
    bool isChunked() const;
    
    // Check if request is complete
    bool isComplete() const;
    bool hasBody() const;

private:
    std::string _method;
    std::string _path;
    std::string _version;
    std::string _body;
    std::map<std::string, std::string> _headers;
    bool _complete;
    bool _headersParsed;
    size_t _contentLength;
    bool _chunked;

    bool parseRequestLine(const std::string& line);
    bool parseHeaders(const std::string& headerSection);
    bool parseBody(const std::string& bodyData);
    
    // Helper
    static std::string toLower(const std::string& str);
};

#endif
