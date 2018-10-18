use v5.10;
use strict;
use warnings 'all';
use JSON::PP;
use IO::Handle;
use Getopt::Long;

my %o = qw /prefix SPEECHSDK/;
GetOptions(\%o, qw/input=s format=s prefix=s define|D=s% verbose|v/)
  or die "Error parsing options.\n";

die "Error: unexpected extra parameters: @ARGV\n" if @ARGV;
die "Error: Missing input option for file is unreadable\n" unless -r $o{input};
die "Error: Missing format option\n" unless $o{format};

my %print = (
  'bash-variable' => sub { return sprintf q(%s='%s'), @_; },
  'bash-environment' => sub { return sprintf q(export %s='%s'), @_; },
  'azuredevops-variable' => sub { return sprintf q(##vso[task.setvariable variable=%s]%s), @_; }
);

die "?Unsupported value '$o{format}' for format option\n" unless defined $print{$o{format}};

my $json = JSON::PP->new;
my $fh = IO::Handle->new();
open $fh, '<encoding(UTF-8)', $o{input}
  or die "Error: cannot open file '$o{input}' for input: $!\n";
my $data = $json->decode(join ' ', <$fh>)
  or die "Error: invalid JSON.\n";

sub p;

sub p {
  my ($p, $d) = @_;
  if (ref($d) eq 'HASH') {
    p "${p}_$_", $d->{$_} foreach keys %$d;
  } elsif (ref($d) eq 'ARRAY') {
      p "${p}_$_", $d->[$_] foreach 0..$#{$d};
  } elsif (ref($d) eq '') {
    die "Error: unsupported characters in name $p\n" if $p =~ /\W/;
    $d =~ s/(?:\\\$|\$\{([^}]+)\})/
          if (defined $1) {
            die "Error: no define '$1'\n" if not defined $o{define}{$1};
            $o{define}{$1};
          } else {
            '$';
          }
    /gex;
    die "Error: unsupported characters in value '$d'\n" if $d =~ /['\r\n]/; # TODO more?
    say $print{$o{format}}($p, $d);
    warn "Setting $p=$d\n" if $o{verbose};
  } else {
    die "Error: unexpected data $d\n";
  }
};

p $o{prefix}, $data;
