/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: omadali <omadali@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/18 07:55:00 by omadali          #+#    #+#             */
/*   Updated: 2026/01/18 07:55:00 by omadali         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/Request.h"
#include <sstream>
#include <cstdlib>
#include <iostream>

Request::Request() 
    : _complete(false), _headersParsed(false), _contentLength(0), _chunked(false) 
{}

Request::~Request() {}

std::string Request::toLower(const std::string& str)
{
    std::string result = str;
    for (size_t i = 0; i < result.size(); ++i)
    {
        if (result[i] >= 'A' && result[i] <= 'Z')
            result[i] = result[i] + 32;
    }
    return result;
}

bool Request::parseRequestLine(const std::string& line)
{
    std::istringstream iss(line);
    if (!(iss >> _method >> _path >> _version))
    {
        std::cerr << "Error: Invalid request line" << std::endl;
        return false;
    }
    
    // Validate method
    if (_method != "GET" && _method != "POST" && _method != "DELETE")
    {
        std::cerr << "Error: Unsupported method: " << _method << std::endl;
        return false;
    }
    
    // Validate HTTP version
    if (_version != "HTTP/1.1" && _version != "HTTP/1.0")
    {
        std::cerr << "Error: Unsupported HTTP version: " << _version << std::endl;
        return false;
    }
    
    return true;
}

bool Request::parseHeaders(const std::string& headerSection)
{
    std::istringstream stream(headerSection);
    std::string line;
    
    while (std::getline(stream, line))
    {
        // Remove \r if present
        if (!line.empty() && line[line.size() - 1] == '\r')
            line = line.substr(0, line.size() - 1);
        
        if (line.empty())
            continue;
            
        size_t colonPos = line.find(':');
        if (colonPos == std::string::npos)
            continue;
        
        std::string name = line.substr(0, colonPos);
        std::string value = line.substr(colonPos + 1);
        
        // Trim leading whitespace from value
        size_t start = value.find_first_not_of(" \t");
        if (start != std::string::npos)
            value = value.substr(start);
        
        // Store header (case-insensitive name)
        _headers[toLower(name)] = value;
    }
    
    // Check for Content-Length
    std::map<std::string, std::string>::iterator it = _headers.find("content-length");
    if (it != _headers.end())
    {
        _contentLength = std::atoi(it->second.c_str());
    }
    
    // Check for Transfer-Encoding: chunked
    it = _headers.find("transfer-encoding");
    if (it != _headers.end() && it->second.find("chunked") != std::string::npos)
    {
        _chunked = true;
    }
    
    _headersParsed = true;
    return true;
}

bool Request::parseBody(const std::string& bodyData)
{
    _body = bodyData;
    
    // Check if we have received complete body
    if (_chunked)
    {
        // TODO: Implement chunked body parsing
        // For now, just store as is
        if (_body.find("0\r\n\r\n") != std::string::npos)
            _complete = true;
    }
    else if (_contentLength > 0)
    {
        if (_body.size() >= _contentLength)
            _complete = true;
    }
    else
    {
        _complete = true;
    }
    
    return true;
}

bool Request::parse(const std::string& raw_request)
{
    // Find end of headers
    size_t headerEnd = raw_request.find("\r\n\r\n");
    if (headerEnd == std::string::npos)
        return false;
    
    std::string headerSection = raw_request.substr(0, headerEnd);
    
    // Parse request line (first line)
    size_t firstLineEnd = headerSection.find("\r\n");
    if (firstLineEnd == std::string::npos)
        return false;
    
    std::string requestLine = headerSection.substr(0, firstLineEnd);
    if (!parseRequestLine(requestLine))
        return false;
    
    // Parse remaining headers
    std::string headers = headerSection.substr(firstLineEnd + 2);
    if (!parseHeaders(headers))
        return false;
    
    // Parse body if present
    if (headerEnd + 4 < raw_request.size())
    {
        std::string body = raw_request.substr(headerEnd + 4);
        parseBody(body);
    }
    else if (!hasBody())
    {
        _complete = true;
    }
    
    return true;
}

// Getters
const std::string& Request::getMethod() const { return _method; }
const std::string& Request::getPath() const { return _path; }
const std::string& Request::getVersion() const { return _version; }
const std::string& Request::getBody() const { return _body; }

std::string Request::getHeader(const std::string& name) const
{
    std::map<std::string, std::string>::const_iterator it = _headers.find(toLower(name));
    if (it != _headers.end())
        return it->second;
    return "";
}

size_t Request::getContentLength() const { return _contentLength; }
bool Request::isChunked() const { return _chunked; }
bool Request::isComplete() const { return _complete; }

bool Request::hasBody() const
{
    return (_method == "POST" || _contentLength > 0 || _chunked);
}
