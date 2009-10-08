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
#ifndef PICOSOCKET_H_
#define PICOSOCKET_H_

#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>

namespace picosocket {
    /**
     * The abstraction class of TCP Socket.
     */
    class TCPSocket {
    private:
        std::string errstr_;
        int fd_;
    public:
        TCPSocket() {
            fd_ = -1;
        }
        /**
         * connect socket to the server.
         * @return true if success to connect.
         */
        bool connect(const char *host, short port) {
            if ((fd_ = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
                errstr_ = strerror(errno);
                return false;
            }

            struct hostent * servhost = gethostbyname(host);
            if (!servhost) {
                errstr_ = std::string("error in gethostbyname: ") + host;
                return false;
            }

            struct sockaddr_in addr;
            addr.sin_family = AF_INET;
            addr.sin_port = htons( port );
            memcpy(&addr.sin_addr, servhost->h_addr, servhost->h_length);

            if (::connect(fd_, (struct sockaddr *)&addr, sizeof(addr)) == -1){
                errstr_ = strerror(errno);
                return false;
            }

            return true;
        }
        int write(const char *buf, size_t siz) {
            return ::write(fd_, buf, siz);
        }
        int read(char *buf, size_t siz) {
            return ::read(fd_, buf, siz);
        }
        int close() {
            return ::close(fd_);
        }
        /**
         * return latest error message.
         */
        std::string errstr() { return errstr_; }
        int fd() { return fd_; }
    };
}

#endif // PICOSOCKET_H_

