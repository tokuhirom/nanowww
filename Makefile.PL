use inc::Module::Install;

my $env = env_for_c(CCFLAGS => '-g -Wall -Wextra', CPPPATH => ['extlib']);
$env->test('t/01_simple', [qw{t/01_simple.cc}]);
$env->test('t/02_get', [qw{t/02_get.cc extlib/picohttpparser/picohttpparser.c}]);
$env->program('eg/post', [qw{eg/post.cc extlib/picohttpparser/picohttpparser.c}]);

tests 't/*.t';

WriteMakefileForC;
