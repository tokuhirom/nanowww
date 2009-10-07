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
#include <sys/socket.h>

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

    class request {
    private:
        headers _headers;
        std::string method;
        std::string uri;
        std::string host;
        std::string path_query;
        std::string port;
    public:
        request(const char *_method, const char *_uri) {
            method = _method;
            uri    = _uri;

            // parse uri
            int offset = 0;
            uri.substr(0, uri.find("://");
            if (strncmp(uri.c_str(), "http://", sizeof("http://")-1) == 0) {
                offset += sizeof("http://")-1;
                host = uri.substr(offset, uri.sfind("/", offset+1));
                host = uri.substr(offset, uri.sfind("/", offset+1));
            }
            // this->set_header("User-Agent", NANOWWW_USER_AGENT); TODO
        }
        ~request() {
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
            connect();
            send();
            read();
            close(sock);
        }
    };
};

#endif // NWNOWWW_H
