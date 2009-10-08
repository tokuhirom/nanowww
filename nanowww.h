/*
 * Copyright (c) 2009, tokuhiro matsuno
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * * Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 * * Neither the name of the <ORGANIZATION> nor the names of its contributors
 *   may be used to endorse or promote products derived from this software
 *   without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef NANOWWW_H_
#define NANOWWW_H_

/**
 * Copyright (C) 2009 tokuhirom
 * modified BSD License.
 */

/**

=head1 SYNOPSIS

    #include "nanowww.h"
    nanowww::Client www;
    nanowww::Response;
    assert(www.send_get(&res, "http://google.com");
    printf("%s\n", res.content());

=head1 POLICY

=head2 WILL SUPPORTS

=head3 important thing

follow redirect

set content from FILE* fh for streaming upload.

=head3 not important thing

basic auth

=head2 WILL NOT SUPPORTS

=over 4

=item I/O multiplexing request

use thread, instead.

=back

=head2 MAY NOT SUPPORTS

I don't need it.But, if you write the patch, I'll merge it.

    KEEP ALIVE

    win32 port

*/

#include "picosocket/picosocket.h"
#include "picouri/picouri.h"
#include "picohttpparser/picohttpparser.h"
#include "picoalarm/picoalarm.h"
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <cstring>
#include <cassert>
#include <string>
#include <map>
#include <iostream>
#include <sstream>

#define NANOWWW_VERSION "0.01"
#define NANOWWW_USER_AGENT "NanoWWW/" NANOWWW_VERSION

#define NANOWWW_MAX_HEADERS 64

namespace nanowww {

    class Headers {
    private:
        std::map<std::string, std::string> _map;
    public:
        void set_header(const char *key, const char *val) {
            _map[key] = val;
        }
        std::string get_header(const char *key) {
            return _map[key];
        }
        std::string as_string() {
            std::map<std::string, std::string>::iterator iter;
            std::string res;
            for ( iter = _map.begin(); iter != _map.end(); ++iter ) {
                assert(
                    iter->second.find('\n') == std::string::npos
                    && iter->second.find('\r') == std::string::npos
                );
                res += iter->first + ": " + iter->second + "\r\n";
            }
            return res;
        }
    };

    class Response {
    private:
        int status_;
        std::string msg_;
        Headers hdr_;
        std::string content_;
    public:
        Response() {
            status_ = -1;
        }
        bool is_success() {
            return status_ == 200;
        }
        int status() { return status_; }
        void set_status(int _status) {
            status_ = _status;
        }
        std::string message() { return msg_; }
        void set_message(const char *str, size_t len) {
            msg_.assign(str, len);
        }
        Headers * headers() { return &hdr_; }
        void set_header(const std::string &key, const std::string &val) {
            hdr_.set_header(key.c_str(), val.c_str());
        }
        void add_content(const std::string &src) {
            content_.append(src);
        }
        void add_content(const char *src, size_t len) {
            content_.append(src, len);
        }
        std::string content() { return content_; }
    };

    class Uri {
    private:
        char * uri_;
        std::string host_;
        int port_;
        std::string path_query_;
    public:
        Uri(const char*src) {
            uri_ = strdup(src);
            assert(uri_);
            const char * scheme;
            size_t scheme_len;
            const char * _host;
            size_t host_len;
            const char *_path_query;
            int path_query_len;
            int ret = parse_uri(uri_, strlen(uri_), &scheme, &scheme_len, &_host, &host_len, &port_, &_path_query, &path_query_len);
            assert(ret == 0);  // TODO: throw
            host_.assign(_host, host_len);
            path_query_.assign(_path_query, path_query_len);
        }
        ~Uri() {
            if (uri_) { free(uri_); }
        }
        std::string host() { return host_; }
        int port() { return port_; }
        std::string path_query() { return path_query_; }
    };

    class Request {
    private:
        Headers headers_;
        std::string method_;
        std::string content_;
        Uri *uri_;
    public:
        Request(const char *_method, const char *a_uri, const char *_content) {
            method_ = _method;
            content_ = _content;
            uri_    = new Uri(a_uri);
            assert(uri_);
            this->set_header("User-Agent", NANOWWW_USER_AGENT);
            this->set_header("Host", uri_->host().c_str());

            // TODO: do not use sstream
            std::stringstream s;
            s << content_.size();
            this->set_header("Content-Length", s.str().c_str());
        }
        ~Request() {
            if (uri_) { delete uri_; }
        }
        void set_header(const char* key, const char *val) {
            this->headers_.set_header(key, val);
        }
        Headers *headers() { return &headers_; }
        Uri *uri() { return uri_; }
        std::string method() { return method_; }
        std::string content() { return content_; }
    };

    class Client {
    private:
        std::string errstr_;
        unsigned int timeout_;
    public:
        Client() {
            timeout_ = 60; // default timeout is 60sec
        }
        /**
         * @args tiemout: timeout in sec.
         * @return none
         */
        void set_timeout(unsigned int timeout) {
            timeout_ = timeout;
        }
        /**
         * @return string of latest error
         */
        std::string errstr() { return errstr_; }
        int send_get(Response *res, const char *uri) {
            Request req("GET", uri, "");
            return this->send_request(req, res);
        }
        int send_post(Response *res, const char *uri, const char *content) {
            Request req("POST", uri, content);
            return this->send_request(req, res);
        }
        int send_put(Response *res, const char *uri, const char *content) {
            Request req("PUT", uri, content);
            return this->send_request(req, res);
        }
        int send_delete(Response *res, const char *uri) {
            Request req("DELETE", uri, "");
            return this->send_request(req, res);
        }
        /**
         * @return return true if success
         */
        bool send_request(Request &req, Response *res) {
            picoalarm::Alarm alrm(this->timeout_); // RAII
            
            short port = req.uri()->port() == 0 ? 80 : req.uri()->port();
            picosocket::TCPSocket sock;
            if (!sock.connect(req.uri()->host().c_str(), port)) {
                errstr_ = sock.errstr();
                return false;
            }

            std::string hbuf =
                  req.method() + " " + req.uri()->path_query() + " HTTP/1.0\r\n"
                + req.headers()->as_string()
                + "\r\n"
            ;

            if (sock.write(hbuf.c_str(), hbuf.size()) != (int)hbuf.size()) {
                errstr_ = "error in writing header";
                return false;
            }
            if (sock.write(req.content().c_str(), req.content().size()) != (int)req.content().size()) {
                errstr_ = "error in writing body";
                return false;
            }

            // reading loop
            std::string buf;
            const int bufsiz = 4096;
            char read_buf[bufsiz];

            // read header part
            while (1) {
                int nread = sock.read(read_buf, sizeof(read_buf));
                if (nread == 0) { // eof
                    errstr_ = "EOF";
                    return false;
                }
                if (nread < 0) { // error
                    errstr_ = strerror(errno);
                    return false;
                }
                buf.append(read_buf, nread);

                int minor_version;
                int status;
                const char *msg;
                size_t msg_len;
                struct phr_header headers[NANOWWW_MAX_HEADERS];
                size_t num_headers = sizeof(headers) / sizeof(headers[0]);
                int last_len = 0;
                int ret = phr_parse_response(buf.c_str(), buf.size(), &minor_version, &status, &msg, &msg_len, headers, &num_headers, last_len);
                if (ret > 0) {
                    res->set_status(status);
                    res->set_message(msg, msg_len);
                    for (size_t i=0; i<num_headers; i++) {
                        res->set_header(
                            std::string(headers[i].name, headers[i].name_len),
                            std::string(headers[i].value, headers[i].value_len)
                        );
                    }
                    res->add_content(buf.substr(ret));
                    break;
                } else if (ret == -1) { // parse error
                    errstr_ = "http response parse error";
                    return false;
                } else if (ret == -2) { // request is partial
                    continue;
                }
            }

            // read body part
            while (1) {
                int nread = sock.read(read_buf, sizeof(read_buf));
                if (nread == 0) { // eof
                    break;
                } else if (nread < 0) { // error
                    errstr_ = strerror(errno);
                    return false;
                } else {
                    res->add_content(read_buf, nread);
                    continue;
                }
            }

            // TODO(tokuhirom): setsockopt O_NDELAY
            sock.close();
            return true;
        }
    };
};

#endif  // NANOWWW_H_
