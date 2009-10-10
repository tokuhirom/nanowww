#line 1
package Module::Install::ForC::Env;
use strict;
use warnings;
use Storable ();         # first released with perl 5.007003
use Config;              # first released with perl 5.00307
use File::Temp ();       # first released with perl 5.006001
use POSIX;               # first released with perl 5
use Text::ParseWords (); # first released with perl 5
use IPC::Open3;          # first released with perl 5

sub DEBUG () { $ENV{DEBUG} }

sub new {
    my $class = shift;

    # platform specific vars
    my %platformvars = do {
        my %unix = (
            PREFIX        => $ENV{PREFIX} || '/usr/',
            LIBPREFIX     => 'lib',
            LIBSUFFIX     => '.a',
            SHLIBPREFIX   => 'lib',
            LDMODULEFLAGS => ['-shared'],
            CCDLFLAGS     => ['-fPIC'], # TODO: rename
        );
        my %win32 = (
            PREFIX      => $ENV{PREFIX} || 'C:\\',
            LIBPREFIX   => '',
            LIBSUFFIX   => '.lib',
            SHLIBPREFIX => '',
            CCDLFLAGS   => [], # TODO: rename
        );
        my %darwin = ( LDMODULEFLAGS => ['-dynamiclib'], );
        my %solaris = (
            CCDLFLAGS     => ['-kPIC'],
            LDMODULEFLAGS => ['-G'],
        );

          $^O eq 'MSWin32'  ? %win32
        : $^O eq 'darwin'   ? (%unix, %darwin)
        : $^O eq 'solaris'  ? (%unix, %solaris)
        : %unix;
    };

    my $opt = {
        CC  => $ENV{CC}  || $Config{cc}  || 'gcc',
        CPP => $ENV{CPP} || $Config{cpp} || 'cpp',
        CXX => $ENV{CXX} || (_has_gplusplus() ? 'g++' : $Config{cc}),
        LD            => $Config{ld},
        LDFLAGS       => '',
        CCFLAGS       => [],
        CPPPATH       => [],
        LIBS          => [],
        LIBPATH       => [],
        SHLIBSUFFIX   => '.' . $Config{so},
        RANLIB        => 'ranlib',
        PROGSUFFIX    => $Config{exe_ext} || '', # $Config{exe_ext} is 'exe' or ''
        CXXFILESUFFIX => [ '.c++', '.cc', '.cpp', '.cxx' ],
        CFILESUFFIX   => ['.c'],
        AR            => $Config{ar},
        %platformvars,
        @_
    };
    for my $key (qw/CPPPATH LIBS CLIBPATH LDMODULEFLAGS CCFLAGS/) {
        $opt->{$key} = [$opt->{$key}] unless ref $opt->{$key};
    }
    my $self = bless $opt, $class;

    # -g support
    if (scalar( grep{ $_ eq '-g' } @ARGV )) {
        $self->append('CCFLAGS' => '-g');
    }

    # fucking '.C' extension support.
    if ($^O eq 'Win32' || $^O eq 'darwin') {
        # case sensitive fs.Yes, I know the darwin supports case-sensitive fs.
        # But, also supports case-insensitive one :)
        push @{$self->{CFILESUFFIX}}, '.C';
    } else {
        push @{$self->{CXXFILESUFFIX}}, '.C';
    }

    return $self;
}

sub _is_gcc {
    return $Config{gccversion} ? 1 : 0;
}

sub _is_msvc {
    my $self = shift;
    return $self->{CC} =~ /\A cl \b /xmsi ? 1 : 0;
}

sub enable_warnings {
    my $self = shift;
    my $opt =     _is_gcc()  ? '-Wall -Wextra'
                : _is_msvc() ? '-W3'
                :              '';
    $self->append('CCFLAGS' => $opt);
}

# do you have g++?
# @return true if user can execute g++.
sub _has_gplusplus {
    my($wtr, $rdr, $err);
    if ($^O eq 'MSWin32') {
        return !system('g++ --version 2> NUL > NUL');
    } else {
        my $pid = open3($wtr, $rdr, $err, 'g++', '--version') or return 0;
        waitpid($pid, 0);
        return WIFEXITED($?) && WEXITSTATUS($?) == 0 ? 1 : 0;
    }
}

# pkg-config
sub parse_config {
    my ($self, $str) = @_;
    my @words = Text::ParseWords::shellwords($str);
    for (my $i=0; $i<@words; $i++) {
        local $_ = $words[$i];
        s/^-I// and do {
            $self->append('CPPPATH' => $_ || $words[++$i]);
            next;
        };
        s/^-L// and do {
            $self->append('LIBPATH' => $_ || $words[++$i]);
            next;
        };
        s/^-l// and do {
            $self->append('LIBS' => $_ || $words[++$i]);
            next;
        };
    }
    return $self;
}

sub install_bin {
    my ($self, $bin) = @_;
    $self->install($bin, 'bin');
}
sub install_lib {
    my ($self, $lib) = @_;
    $self->install($lib, 'lib');
}
sub install {
    my ($self, $target, $suffix) = @_;
    my $dst = File::Spec->catfile($self->{PREFIX}, $suffix);
    ($target =~ m{['"\n\{\}]}) and die "invalid file name for install: $target";
    ($suffix =~ m{['"\n\{\}]}) and die "invalid file name for install: $suffix";
    push @{$Module::Install::ForC::INSTALL{$suffix}}, "\$(PERL) -e 'use File::Copy; File::Copy::copy(q{$target}, q{$dst}) or die qq{Copy failed: $!}'";
}

sub try_cc {
    my ($self, $src) = @_;
    my ( $ch, $cfile ) = File::Temp::tempfile(
        'assertlibXXXXXXXX',
        SUFFIX => '.c',
        UNLINK => 1,
    );
    my $executable = File::Temp->new(UNLINK => 1);
    print $ch $src;
    my $cmd = "$self->{CC} -o $executable @{[ $self->_libs ]} @{[ $self->_cpppath ]} @{ $self->{CCFLAGS} } $cfile";
    print "$cmd\n" if DEBUG;
    my $exit_status = _quiet_system($cmd);
    WIFEXITED($exit_status) && WEXITSTATUS($exit_status) == 0 ? 1 : 0;
}

# code substantially borrowed from IPC::Run3                                                                                          
sub _quiet_system {
    my (@cmd) = @_;

    # save handles
    local *STDOUT_SAVE;
    local *STDERR_SAVE;
    open STDOUT_SAVE, ">&STDOUT" or die "CheckLib: $! saving STDOUT";
    open STDERR_SAVE, ">&STDERR" or die "CheckLib: $! saving STDERR";

    # redirect to nowhere
    local *DEV_NULL;
    open DEV_NULL, ">" . File::Spec->devnull
      or die "CheckLib: $! opening handle to null device";
    open STDOUT, ">&" . fileno DEV_NULL
      or die "CheckLib: $! redirecting STDOUT to null handle";
    open STDERR, ">&" . fileno DEV_NULL
      or die "CheckLib: $! redirecting STDERR to null handle";

    # run system command
    my $rv = system(@cmd);

    # restore handles
    open STDOUT, ">&" . fileno STDOUT_SAVE
      or die "CheckLib: $! restoring STDOUT handle";
    open STDERR, ">&" . fileno STDERR_SAVE
      or die "CheckLib: $! restoring STDERR handle";

    return $rv;
}


sub have_header {
    my ($self, $header,) = @_;
    _checking_for(
        "C header $header",
        $self->try_cc("#include <$header>\nint main() { return 0; }")
    );
}

sub _checking_for {
    my ($msg, $result) = @_;
    print "Checking for $msg ... @{[ $result ? 'yes' : 'no' ]}\n";
    return $result;
}

sub have_library {
    my ($self, $library,) = @_;
    _checking_for(
        "C library $library",
        $self->clone()->append( 'LIBS' => $library )->try_cc("int main(){return 0;}")
    );
}

sub clone {
    my ($self, ) = @_;
    return Storable::dclone($self);
}

sub append {
    my $self = shift;
    while (my ($key, $val) = splice(@_, 0, 2)) {
        if ((ref($self->{$key})||'') eq 'ARRAY') {
            $val = [$val] unless ref $val;
            push @{ $self->{$key} }, @{$val};
        } else {
            $self->{$key} = $val;
        }
    }
    return $self; # for chain
}

sub _objects {
    my ($self, $srcs) = @_;
    my @objects;
    my $regex = join('|', map { quotemeta($_) } @{$self->{CXXFILESUFFIX}}, @{$self->{CFILESUFFIX}});
    for my $src (@$srcs) {
        if ((my $obj = $src) =~ s/$regex/$Config{obj_ext}/) {
            push @objects, $obj;
        } else {
            die "unknown src file type: $src";
        }
    }
    @objects;
}

sub _libs {
    my $self = shift;
    return map { "-l$_" } @{$self->{LIBS}};
}

sub _libpath {
    my $self = shift;
    return join ' ', map { "-L$_" } @{$self->{LIBPATH}};
}

sub program {
    my ($self, $bin, $srcs, %specific_opts) = @_;
    $srcs = [$srcs] unless ref $srcs;
    my $clone = $self->clone()->append(%specific_opts);

    my $target = "$bin" . $clone->{PROGSUFFIX};
    _push_target($target);

    my @objects = $clone->_objects($srcs);

    my $ld = $clone->_ld(@$srcs);

    my $target_with_opt = ($self->{CC} =~ /^cl/ && $^O eq 'MSWin32') ? "/out:$target" : "-o $target";
    $self->_push_postamble(<<"...");
$target: @objects
    $ld $clone->{LDFLAGS} $target_with_opt @objects @{[ $clone->_libpath ]} @{[ $clone->_libs ]}

...

    $clone->_compile_objects($srcs, \@objects, '');

    return $target;
}

sub test {
    my ($self, $binary, $src, %specific_opts) = @_;
    my $test_file = "$binary\.t";
    my $test_executable = $self->program($binary, $src, %specific_opts);

    $self->_push_postamble(<<"...");
$test_file: $test_executable
    \$(PERL) -e 'print "exec q{$test_executable} or die \$!"' > $test_file

...

    push @Module::Install::ForC::TESTS, $test_file;

    return $test_file;
}

sub _is_cpp {
    my ($self, $src) = @_;
    my $pattern = join('|', map { quotemeta($_) } @{$self->{CXXFILESUFFIX}});
    $src =~ qr/$pattern$/ ? 1 : 0;
}

sub _push_postamble {
    (my $src = $_[1]) =~ s/^[ ]{4}/\t/gmsx;
    $Module::Install::ForC::POSTAMBLE .= $src;
}

sub _cpppath {
    my $self = shift;
    join ' ', map { "-I $_" } @{ $self->{CPPPATH} };
}

sub _compile_objects {
    my ($self, $srcs, $objects, $opt) = @_;
    $opt ||= '';

    for my $i (0..@$srcs-1) {
        next if $Module::Install::ForC::OBJECTS{$objects->[$i]}++ != 0;
        my $compiler = $self->_is_cpp($srcs->[$i]) ? $self->{CXX} : $self->{CC};
        my $object_with_opt = ($compiler =~ /^cl/ && $^O eq 'MSWin32') ? "-c -Fo$objects->[$i]" : "-c -o $objects->[$i]";
        $self->_push_postamble(<<"...");
$objects->[$i]: $srcs->[$i] Makefile
	$compiler $opt @{ $self->{CCFLAGS} } @{[ $self->_cpppath ]} $object_with_opt $srcs->[$i]

...
        if ($^O ne 'MSWin32' && $compiler ne 'tcc') {
            my $deps = `$compiler -MM $srcs->[$i]`;
            my $basedir = File::Basename::dirname($srcs->[$i]);
            $self->_push_postamble(<<"...") if $deps;
$basedir/$deps
...
        }
    }
}

sub _ld {
    my ($self, @srcs) = @_;
    (scalar(grep { $self->_is_cpp($_) } @srcs) > 0) ? $self->{CXX} : $self->{LD};
}

sub _push_target {
    my $target = shift;
    push @Module::Install::ForC::TARGETS, $target;
}

sub shared_library {
    my ($self, $lib, $srcs, %specific_opts) = @_;
    $srcs = [$srcs] unless ref $srcs;
    my $clone = $self->clone->append(%specific_opts);

    my $target = "$clone->{SHLIBPREFIX}$lib$clone->{SHLIBSUFFIX}";

    _push_target($target);

    my @objects = $clone->_objects($srcs);

    my $ld = $clone->_ld(@$srcs);
    $self->_push_postamble(<<"...");
$target: @objects Makefile
	$ld @{ $clone->{LDMODULEFLAGS} } @{[ $clone->_libpath ]} @{[ $clone->_libs ]} $clone->{LDFLAGS} -o $target @objects

...
    $clone->_compile_objects($srcs, \@objects, @{$self->{CCCDLFLAGS}});

    return $target;
}

sub static_library {
    my ($self, $lib, $srcs, %specific_opts) = @_;
    $srcs = [$srcs] unless ref $srcs;
    my $clone = $self->clone->append(%specific_opts);

    my $target = "$clone->{LIBPREFIX}$lib$clone->{LIBSUFFIX}";

    _push_target($target);

    my @objects = $clone->_objects($srcs);

    $self->_push_postamble(<<"...");
$target: @objects Makefile
	$clone->{AR} rc $target @objects
	$clone->{RANLIB} $target

...
    $clone->_compile_objects($srcs, \@objects, @{$self->{CCCDLFLAGS}});

    return $target;
}

sub have_type {
    my ($self, $type, $src) = @_;
    $src ||= '';

    $self->try_cc(<<"...");
$src

int main() {
    if ( ( $type * ) 0 ) return 0;
    if ( sizeof($type) ) return 0;
    return 0;
}
...
}

1;
__END__

#line 525
