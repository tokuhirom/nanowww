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
#include <map>
#include <sys/types.h>
#include <cstring>
#include <cassert>
#include <sys/socket.h>
#include "picouri/picouri.h"

#define NANOWWW_VERSION "0.01"
#define NANOWWW_USER_AGENT "NanoWWW/" NANOWWW_VERSION

namespace nanowww {
    class response;

    class headers {
    private:
        std::map<std::string, std::string> _map;
    public:
        void set_header(const char *key, const char *val) {
            _map[key] = val;
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
        std::string get_host() {
            return host;
        }
        int get_port() {
            return port;
        }
        std::string get_path_query() {
            return path_query;
        }
    };

    class request {
    private:
        headers _headers;
        std::string method;
        uri *_uri;
    public:
        request(const char *_method, const char *a_uri) {
            method = _method;
            _uri    = new uri(a_uri);
            assert(_uri);
        }
        ~request() {
            if (_uri) { delete _uri; }
        }
        void set_header(const char* key, const char *val) {
            this->_headers.set_header(key, val);
        }
    };

    class client {
    private:
    public:
        client() {
        }
        response * get(const char *uri) {
            request req("GET", uri);
            return this->send_request(req);
        }
        response * send_request(request &req) {
            int sock;
            if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
                throw "err"; // TODO
            }
            // TODO: setsockopt O_
            /*
            connect();
            send();
            read();
            */
            close(sock);
        }
    };
};

#endif // NWNOWWW_H
