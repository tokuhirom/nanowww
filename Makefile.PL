use inc::Module::Install;

my $phr = 'extlib/picohttpparser/picohttpparser.c';

my $env = env_for_c(CPPPATH => ['extlib']);
$env->enable_warnings();
if ($^O eq 'solaris') {
    $env->append(LIBS => [qw/socket nsl/]);
}
if ($env->have_library('ssl')) {
    $env->append("CCFLAGS" => '-DHAVE_SSL', LIBS => ['ssl', 'crypto']);
    $env->test('t/06_ssl', [qw{t/06_ssl.cc extlib/picohttpparser/picohttpparser.c}]);
}
$env->test('t/01_simple', [qw{t/01_simple.cc}]);
$env->program('t/02_get', [qw{t/02_get.cc extlib/picohttpparser/picohttpparser.c}]);
$env->program('eg/post', [qw{eg/post.cc extlib/picohttpparser/picohttpparser.c}]);
$env->program('t/04_timeout', [qw{t/04_timeout.cc extlib/picohttpparser/picohttpparser.c}]);
$env->program('t/05_upload', [qw{t/05_upload.cc extlib/picohttpparser/picohttpparser.c}]);
$env->test('t/07_headers', [qw{t/07_headers.cc extlib/picohttpparser/picohttpparser.c}]);
$env->program('t/08_basicauth', [qw{t/08_basicauth.cc extlib/picohttpparser/picohttpparser.c}]);
$env->program('t/09_proxy', [qw{t/09_proxy.cc extlib/picohttpparser/picohttpparser.c}]);
if ($env->have_library('curl')) {
    my $cenv = $env->clone()->append(LIBS => 'curl', CCFLAGS => '-O2');
    $cenv->program('author/benchmark/simple', [qw(author/benchmark/simple.cc), $phr]);
}
{
    my $penv = $env->clone()->append(CCFLAGS => '-pg', LDFLAGS => '-pg');
    $penv->program('author/profile/simple', [qw(author/profile/simple.cc), $phr]);
}

test_requires 'Test::Requires';
test_requires 'Test::TCP';
test_requires 'Test::SharedFork';
test_requires 'HTTP::Daemon';
tests 't/*.t';

WriteMakefileForC;
