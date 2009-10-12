use inc::Module::Install;

my $env = env_for_c(CPPPATH => ['extlib']);
$env->test('t/01_simple', [qw{t/01_simple.cc}]);
$env->test('t/02_get', [qw{t/02_get.cc extlib/picohttpparser/picohttpparser.c}]);
$env->program('eg/post', [qw{eg/post.cc extlib/picohttpparser/picohttpparser.c}]);
$env->program('t/04_timeout', [qw{t/04_timeout.cc extlib/picohttpparser/picohttpparser.c}]);
$env->program('t/05_upload', [qw{t/05_upload.cc extlib/picohttpparser/picohttpparser.c}]);

test_requires 'HTTP::Daemon';
tests 't/*.t';

WriteMakefileForC;
