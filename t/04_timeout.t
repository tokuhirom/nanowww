use strict;
use warnings;
use Test::TCP;
use POSIX;
use HTTP::Daemon;
use HTTP::Status;

test_tcp(
    client => sub {
        my $port = shift;
        my $res  = `./t/04_timeout $port`;
        print($res);
    },
    server => sub {
        my $port = shift;

        my $d = HTTP::Daemon->new(LocalPort => $port) || die;
        while ( my $c = $d->accept ) {
            while ( my $r = $c->get_request ) {
                sleep 1000;
            }
            $c->close;
            undef($c);
        }
    },
);
