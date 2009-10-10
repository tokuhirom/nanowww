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
        my $res  = `./eg/post $port`;
        ok POSIX::WIFEXITED($?),    "exit";
        ok !POSIX::WIFSIGNALED($?), "signal";
        is POSIX::WTERMSIG($?),     0, 'signal';
        like $res, qr/hoge=fuga/;
        like $res, qr/page=fuga%0a/;
        like $res, qr/moge=moge%3a%a2/;
        done_testing;
    },
    server => sub {
        my $port = shift;

        my $d = HTTP::Daemon->new(LocalPort => $port) || die;
        while ( my $c = $d->accept ) {
            while ( my $r = $c->get_request ) {
                if ( $r->method eq 'POST') {
                    $c->send_response(HTTP::Response->new(200, 'ok', [], $r->content));
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
