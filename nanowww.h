#ifndef NANOWWW_H
#define NWNOWWW_H

/**
 * Copyright (C) 2009 tokuhirom
 * modified BSD License.
 */

/**

=head1 SYNOPSIS

    #include "nanowww.h"
    nanowww::client www;
    nanowww::response * res = www.get("http://google.com");
    printf("%s\n", res.content());

=head1 POLICY

=head2 WILL SUPPORTS

GET, POST, PUT, DELETE request.

basic auth

timeout

=head2 WILL NOT SUPPORTS

=over 4

=item I/O multiplexing request

use thread, instead.

=back

=head2 MAY NOT SUPPORTS

I don't need it.But, if you write the patch, I'll merge it.

    KEEP ALIVE

*/

#include <string>
#include <stdlib.h>
#include <netdb.h>
#include <map>
#include <sys/types.h>
#include <cstring>
#include <cassert>
#include <sys/socket.h>
#include "picouri/picouri.h"
#include <iostream>

#define NANOWWW_VERSION "0.01"
#define NANOWWW_USER_AGENT "NanoWWW/" NANOWWW_VERSION

namespace nanowww {

    class headers {
    private:
        std::map<std::string, std::string> _map;
    public:
        void set_header(const char *key, const char *val) {
            _map[key] = val;
        }
        std::string as_string() {
            std::map<std::string,std::string>::iterator iter;
            std::string res;
            for( iter = _map.begin(); iter != _map.end(); ++iter ) {
                assert(iter->second.find('\n') == std::string::npos && iter->second.find('\r') == std::string::npos);
                res += iter->first + ": " + iter->second + "\r\n";
            }
            return res;
        }
    };

    class response {
    private:
        int code;
        const char *msg;
        headers hdr;
        const char *content;
    public:
        bool is_success() {
            return code == 200;
        }
    };

    class uri {
    private:
        char * _uri;
        std::string host;
        int port;
        std::string path_query;
    public:
        uri(const char*src) {
            _uri = strdup(src);
            assert(_uri);
            const char * scheme;
            size_t scheme_len;
            const char * _host;
            size_t host_len;
            const char *_path_query;
            int path_query_len;
            int ret = parse_uri(_uri, strlen(_uri), &scheme, &scheme_len, &_host, &host_len, &port, &_path_query, &path_query_len);
            assert(ret == 0); // TODO: throw
            host.assign(_host, host_len);
            path_query.assign(_path_query, path_query_len);
        }
        ~uri() {
            if (_uri) { free(_uri); }
        }
        std::string get_host() { return host; }
        int get_port() { return port; }
        std::string get_path_query() { return path_query; }
    };

    class request {
    private:
        headers _headers;
        std::string method;
        std::string content;
        uri *_uri;
    public:
        request(const char *_method, const char *a_uri, const char *_content) {
            method = _method;
            content = _content;
            _uri    = new uri(a_uri);
            assert(_uri);
            this->set_header("User-Agent", NANOWWW_USER_AGENT);
            this->set_header("Host", _uri->get_host().c_str());
        }
        ~request() {
            if (_uri) { delete _uri; }
        }
        void set_header(const char* key, const char *val) {
            this->_headers.set_header(key, val);
        }
        headers *get_headers() { return &_headers; }
        uri *get_uri() { return _uri; }
        std::string get_method() { return method; }
        std::string get_content() { return content; }
    };

    class client {
    private:
    public:
        client() {
        }
        void get(const char *uri, response *res) {
            request req("GET", uri, "");
            this->send_request(req, res);
        }
        void send_request(request &req, response *res) {
            int sock;
            if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
                throw "err"; // TODO
            }

            struct hostent * servhost = gethostbyname(req.get_uri()->get_host().c_str());
            assert(servhost); // TODO

            struct sockaddr_in server;
            memset(&server, 0, sizeof(sockaddr_in));
            server.sin_family = AF_INET;
            memcpy(servhost->h_addr, (char *)&server.sin_addr, servhost->h_length);
            server.sin_port = htons( req.get_uri()->get_port() == 0 ? 80 : req.get_uri()->get_port() );

            if ( connect(sock, (struct sockaddr *)&server, sizeof(server)) == -1 ){
                assert("connect" && 0); // TODO
            }

            std::string hbuf = (
                  req.get_method() + " " + req.get_uri()->get_path_query() + " HTTP/1.0\r\n"
                + req.get_headers()->as_string()
                + "\r\n"
            );

            assert(write(sock, hbuf.c_str(), hbuf.size()) == hbuf.size());
            assert(write(sock, req.get_content().c_str(), req.get_content().size()) == req.get_content().size());

            // TODO: setsockopt O_
            close(sock);
        }
    };
};

#endif // NWNOWWW_H
