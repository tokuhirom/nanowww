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
        my $res  = `./t/08_basicauth $port`;
        ok POSIX::WIFEXITED($?),    "exit";
        ok !POSIX::WIFSIGNALED($?), "signal";
        is POSIX::WTERMSIG($?),     0, 'signal';
        is $res, "OK\n";
        done_testing;
    },
    server => sub {
        my $port = shift;

        my $d = HTTP::Daemon->new(ReuseAddr => 1, LocalPort => $port) || die;
        while ( my $c = $d->accept ) {
            while ( my $r = $c->get_request ) {
                my ($user, $pw) = $r->authorization_basic();
                if ($user eq 'dankogai' && $pw eq 'kogaidan') {
                    $c->send_response(HTTP::Response->new(200, 'ok', [], "OK"));
                } else {
                    $c->send_response(HTTP::Response->new(401, 'authoriazation required', [], "authorization required"));
                }
            }
            $c->close;
            undef($c);
        }
    },
);
