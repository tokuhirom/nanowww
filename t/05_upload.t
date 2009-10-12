use strict;
use warnings;
use Test::TCP;
use Test::More;
use POSIX;
use HTTP::Daemon;
use HTTP::Status;
use Data::Dumper;

sub slurp {
    my $fname = shift;
    open my $fh, '<', $fname or die $!;
    my $content = do { local $/; <$fh>; };
    close $fh;
    $content;
}

test_tcp(
    client => sub {
        my $port = shift;
        my $res  = `./t/05_upload $port`;
        ok POSIX::WIFEXITED($?),    "exit";
        ok !POSIX::WIFSIGNALED($?), "signal";
        is POSIX::WTERMSIG($?),     0, 'signal';
        is $res, "OK\n";
        done_testing;
    },
    server => sub {
        my $port = shift;

        my $d = HTTP::Daemon->new(LocalPort => $port) || die;
        while ( my $c = $d->accept ) {
            while ( my $r = $c->get_request ) {
                if ( $r->method eq 'POST') {
                    my @parts = $r->parts;
                    is $parts[0]->header('Content-Disposition'), 'form-data; name="hoge"';
                    is $parts[0]->content, 'fuga';
                    is $parts[1]->header('Content-Disposition'), 'form-data; name="poke"';
                    is $parts[1]->content, 'take';
                    is $parts[2]->header('Content-Disposition'), 'form-data; name="upload"; filename="t/dat/d1.dat"';
                    is $parts[2]->content, 'foo';
                    is $parts[3]->header('Content-Disposition'), 'form-data; name="nanowww"; filename="nanowww.h"';
                    is $parts[3]->content, slurp('nanowww.h');
                    $c->send_response(HTTP::Response->new(200, 'ok', [], "OK"));
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
