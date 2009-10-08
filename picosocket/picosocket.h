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

