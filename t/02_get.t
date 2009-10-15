use strict;
use warnings;
use Test::TCP;
use Test::More;
use POSIX;
use HTTP::Daemon;
use HTTP::Status;

test_tcp(
    client => sub {
        my $port = shift;
        my $res  = `./t/02_get $port`;
        ok POSIX::WIFEXITED($?),    "exit";
        ok !POSIX::WIFSIGNALED($?), "signal";
        is POSIX::WTERMSIG($?),     0, 'signal';
        is($res, "YAY\n");
        done_testing;
    },
    server => sub {
        my $port = shift;

        my $d = HTTP::Daemon->new(ReuseAddr => 1, LocalPort => $port) || die;
        while ( my $c = $d->accept ) {
            while ( my $r = $c->get_request ) {
                if ( $r->method eq 'GET') {
                    $c->send_response(HTTP::Response->new(200, 'ok', [], "YAY"));
                }
                else {
                    warn 'anything else';
                    $c->send_error(RC_FORBIDDEN);
                }
            }
            $c->close;
            undef($c);
        }
    },
);
