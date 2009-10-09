use inc::Module::Install;

my $env = env_for_c(CCFLAGS => '-g -Wall -Wextra', CPPPATH => ['extlib']);
$env->program('t/01_simple', [qw{t/01_simple.cc}]);
$env->program('t/02_get', [qw{t/02_get.cc extlib/picohttpparser/picohttpparser.c}]);
$env->_push_postamble('t/'.`gcc -MM t/02_get.cc`);

WriteMakefileForC;
