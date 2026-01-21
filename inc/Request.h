/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sdaban <sdaban@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/21 13:52:13 by sdaban            #+#    #+#             */
/*   Updated: 2026/01/21 13:53:39 by sdaban           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_H
#define REQUEST_H

#include <string>
#include <map>

class Request {
public:
    Request();
    ~Request();


    const std::string& get_method() const;
    const std::string& get_uri() const;
    const std::string& get_version() const;
    const std::string& get_body() const;
    const std::string& get_query_string() const;
    const std::map<std::string, std::string>& get_headers() const;

private:
    std::string m_method;
    std::string m_uri;
    std::string m_path;
    std::string m_query_string;
    std::string m_version;
    std::string m_body;
    std::map<std::string, std::string> m_headers;

};

#endif