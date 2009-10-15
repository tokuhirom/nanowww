use strict;
use warnings;
use Test::TCP;
use Test::More;
use POSIX;
use HTTP::Daemon;
use HTTP::Status;
use Test::Requires 'HTTP::Proxy';
use Test::SharedFork;

test_tcp(
    client => sub {
        my $httpd_port = shift;
        ok 1, "httpd port($httpd_port) is now works";

        test_tcp(
            client => sub {
                my $proxy_port = shift;
                die "bug in Test::TCP($proxy_port == $httpd_port)" if $proxy_port == $httpd_port;
                ok 1, "started http client($httpd_port, $proxy_port)!";
                my $res  = `./t/09_proxy $httpd_port $proxy_port`;
                ok POSIX::WIFEXITED($?),    "exit";
                ok !POSIX::WIFSIGNALED($?), "signal";
                is POSIX::WTERMSIG($?),     0, 'signal';
                is $res, "OK!!!\n";
                done_testing;
            },
            server => sub {
                my $proxy_port = shift;
                ok 1, "started PROXY($proxy_port)!";
                HTTP::Proxy->new( port => $proxy_port )->start();
            },
        );
    },
    server => sub {
        my $httpd_port = shift;

        my $d = HTTP::Daemon->new(ReuseAddr => 1, LocalPort => $httpd_port, LocalAddr => '0.0.0.0') || die;
        ok 1, "started httpd($httpd_port)!";
        while ( my $c = $d->accept ) {
            while ( my $r = $c->get_request ) {
                ok '100', '200 ok';
                $c->send_response(HTTP::Response->new(200, 'ok', [], "OK!!!"));
            }
            $c->close;
            undef($c);
        }
    },
);

