#!/usr/bin/perl
#
# Copyright (c) Microsoft Corporation. All rights reserved.
# Licensed under the MIT license.
#
use strict;
use warnings qw/all/;
use IO::Handle;
use Text::Wrap;
my $level;
my (%RE, %H);
my $verbose = 0;

# TODO something special for constructor? identify by return type 'instancetype _Nullable':'id' ?
# TODO inheritance: show full change? change formatting?
# TODO proper escaping, e.g., for underscores
# TODO include header?

# Helper regular expression. All non-capturing.
$H{hex} = qr/0x[0-9a-f]{12}/;
$H{loc} = qr/line:\d+:\d+/;
$H{col} = qr/col:\d+/;
$H{range} = qr/<(?:source.*:\d+:\d+, (?:$H{loc}|$H{col})|$H{col}|$H{col}, $H{loc}|$H{loc}|$H{col}, $H{col}|$H{loc}, $H{col}|$H{loc}, $H{loc})>/;

# Regular expression for terminal matches. Named captures.
$RE{_EnumExtensibilityAttr} = qr/EnumExtensibilityAttr/; # ignored
$RE{_HTMLEndTagComment} = qr/HTMLEndTagComment $H{hex} $H{range} Name="(?<name>.*?)"\r?$/;
$RE{_HTMLStartTagComment} = qr/HTMLStartTagComment $H{hex} $H{range} Name="(?<name>\w*?)"(?: Attrs:  "(?<attrs>.*))?\r?$/; # no closing quote, it seems
$RE{_ParmVarDecl} = qr/ParmVarDecl $H{hex} $H{range} $H{col} (?<name>\w+) '(?<type>.*?)':'(?<basetype>.*?)'\r?$/;
$RE{_TextComment} = qr/TextComment $H{hex} $H{range} Text="(?<text>.*)"\r?$/;
$RE{_super} = qr/super ObjCInterface $H{hex} '(?<name>\w+)'\r?$/;

# Regular expression for non-terminal matches. Named captures.
$RE{BlockCommandComment} = qr/BlockCommandComment $H{hex} $H{range} Name="(?<name>.*?)"\r?$/;
$RE{EnumConstantDecl} = qr/EnumConstantDecl $H{hex} $H{range} $H{col} (?<name>\w+) '(?<type>.*?)':'(?<basetype>.*?)\r?/;
$RE{EnumDecl} = qr/EnumDecl $H{hex}(?<prev> prev $H{hex})? <.*?> (?:$H{col}|$H{loc}) (?<name>\w+) '(?<type>.*?)':'(?<basetype>.*?)\r?/;
$RE{FullComment} = qr/FullComment $H{hex} (?:$H{range})\r?$/;
$RE{ObjCInterfaceDecl} = qr/ObjCInterfaceDecl $H{hex} $H{range} $H{loc} (?<name>\w+)\r?$/;
$RE{ObjCMethodDecl} = qr/
  ObjCMethodDecl
  \s$H{hex}
  \s$H{range}
  \s$H{col}
  \s((?<implicit>implicit)\s)?
  (?<visibility>[+-])
  \s(?<name>\S+)
  \s'(?<type>.*?)'(?::'(?<basetype>.*?)')?
/x;
$RE{ObjCPropertyDecl} = qr/
  ObjCPropertyDecl
  \s$H{hex}
  \s$H{range}
  \s$H{col}
  \s(?<name>\w+)
  \s'(?<type>.*?)':'(?<basetype>.*?)'
  (?:\s(?<flags>(?:readwrite|readonly|copy|atomic|nonatomic))(?:\s(?:readwrite|readonly|copy|atomic|nonatomic))*)?
  \r?$
/x;
$RE{ObjCProtocolDecl} = qr/ObjCProtocolDecl $H{hex} $H{range} $H{loc} (?<name>\w*)\r?$/;
$RE{ParagraphComment} = qr/ParagraphComment $H{hex} (?:$H{range}|$H{range})\r?$/;
$RE{ParamCommandComment} = qr/ParamCommandComment $H{hex} $H{range} \[in\] implicitly Param="(?<name>\w*)" ParamIndex=(?<index>\d+)\r?$/;
$RE{TranslationUnitDecl} = qr/TranslationUnitDecl/;
$RE{TypedefDecl} = qr/TypedefDecl $H{hex} $H{range} $H{col} (?:(?<referenced>referenced) )?(?<name>\w*) '(?<type>.*?)'(?::'(?<basetype>.*?)')?\r?$/;

sub trim {
  my $a = shift or die "Missing parameter to trim()\n";
  $a =~ s/^\s+|\s+$//g;
  return $a
}

my @stack;
my @data;
my %matches;
my $oldlevel;
my @interfaces;
my @enums;
my @protocols;
my @typedefs;

sub pop1 {
  my $x = pop @stack;
  my $y = pop @data;

  if ($x eq "ParagraphComment") {
    $data[-1]{description} = trim($y->{text});
  } elsif ($x eq "ObjCMethodDecl") {
    push @{$data[-1]{method}}, $y;
  } elsif ($x eq "EnumDecl") {
    push @enums, $y if defined $y->{enumerators};
  } elsif ($x eq "EnumConstantDecl") {
    # Skip empty
    push @{$data[-1]{enumerators}}, $y;
  } elsif ($x eq "ObjCPropertyDecl") {
    push @{$data[-1]{property}}, $y;
  } elsif ($x eq "ObjCInterfaceDecl") {
    push @interfaces, $y;
  } elsif ($x eq "ObjCProtocolDecl") {
    push @protocols, $y;
  } elsif ($x eq "TypedefDecl") {
    # Note: skipping separate typedef for enums.
    push @typedefs, $y unless $y->{type} =~ /enum/;
  } elsif ($x eq "FullComment") {
    $data[-1]{fullcomment} = $y;
  } elsif ($x eq "BlockCommandComment") {
    push @{$data[-1]{block}}, $y;
  } elsif ($x eq "ParamCommandComment") {
    push @{$data[-1]{param}}, $y;
  }
}

my $seen; # set to one once we've seen the first line for our headers
my $ignoreUntil; # use to ignore a subtree of certain level
while (<>) {
  chomp;

  next if (not defined $seen) and index($_, "/objective-c/public/") == -1;
  $seen = 1;

  m/^([| `-]*)/ or die "?Cannot determine level, line $.\n";

  $level = length($1) / 2;

  if (defined $ignoreUntil) {
    if ($level <= $ignoreUntil) {
      $ignoreUntil = undef;
    } else {
      next;
    }
  }

  my $m = undef;
  foreach my $t (keys %RE) {
    if (/$RE{$t}/) {
      $m = $t;
      %matches = %+;
      last;
    };
  }

  if (not defined $oldlevel) {
    # TODO first line
  } elsif ($oldlevel == $level) {
    # same
  } elsif ($level < $oldlevel) {
    # close level(s)
    pop1 foreach $level+1..$oldlevel;
  } elsif ($oldlevel + 1 == $level) {
    # open 1 level
  } else {
    warn "Unexpected level change $oldlevel $level line: $.\n"
  }

  # Ignore some stuff under typedef
  if (/ImplicitCastExpr|BlockPointerType|ElaboratedType/) {
    $ignoreUntil = $level;
    $oldlevel = $level;
    warn "Ignoring element starting at line $., level $level\n" if $verbose;
    next;
  }

  if (not defined $m) {
    die "No match?! line:$. $_\n";
  }

  if (index($m, '_') == 0) {
    # Terminal
    if ($m eq "_TextComment") {
      warn "Unexpected stack: @stack" unless $stack[-1] eq 'ParagraphComment';
      $data[-1]{text} .= $matches{text};
    } elsif ($m eq "_HTMLStartTagComment") {
      warn "Unexpected stack: @stack" unless $stack[-1] eq 'ParagraphComment';
      $data[-1]{text} .= sprintf '<%s %s>', $matches{name}, ($matches{attrs} ? $matches{attrs} : '');
    } elsif ($m eq "_HTMLEndTagComment") {
      warn "Unexpected stack: @stack" unless $stack[-1] eq 'ParagraphComment';
      $data[-1]{text} .= sprintf '</%s>', $matches{name};
    } elsif ($m eq "_super") {
      warn "Unexpected stack: @stack" unless $stack[-1] eq 'ObjCInterfaceDecl';
      $data[-1]{super} = $matches{name};
    } elsif ($m eq "_ParmVarDecl") {
      warn "Unexpected stack: @stack" unless $stack[-1] eq 'ObjCMethodDecl';
      push @{$data[-1]{vars}}, { %matches };
    }
  } else {
    # Non-terminal
    push @stack, $m;
    if ($m eq "ObjCMethodDecl") {
      $matches{paramcount} = $matches{name} =~ tr/://;
    }
    push @data, { %matches };
  }
  $oldlevel = $level;
}

# Finish up parsing
pop1 foreach 0..$#stack;

sub section {
  my ($header, $aref, $cref) = @_;
  my $s = '';
  if (defined $aref and @{$aref}) {
    $s = $header;
    foreach my $x (@{$aref}) {
      $s .= sprintf '%s', $cref->($x);
    }
  }
  return $s;
}

sub docSignature {
  my $m = shift;

  my $signature = '';

  $signature = sprintf '%s (%s)', $m->{visibility}, $m->{type};

  if ($m->{paramcount} == 0) {
    $signature .= $m->{name};
  } else {
    my @parts = split ':', $m->{name};
    my $params = $m->{vars};

    # First param
    $signature .= sprintf '%s:(%s)%s',
      $parts[0],
      $params->[0]{type},
      $params->[0]{name};

    my $columns = length($signature);

    # All others
    for (my $i = 1; $i < @parts; $i++) {
      my $newPart .= sprintf '%s:(%s)%s',
        $parts[$i],
        $params->[$i]{type},
        $params->[$i]{name};

      # Break line if too long
      if ($columns + 1 + length($newPart) > 100) {
        my $indent = "    ";
        $signature .= "\n$indent";
        $columns = 1 + length $indent;
      } else {
        $signature .= " ";
        $columns++;
      }
      $signature .= $newPart;
      $columns += length($newPart);
    }
  }

  return "\n```objc\n$signature\n```\n\n";
}

sub docMethod {
  my $m = shift;
  my $s = "\n### $m->{name}\n\n";
  if ($m->{fullcomment}{description}) {
    $s .= "$m->{fullcomment}{description}\n";
  } else {
    warn "Warning: method $m->{name}: missing documentation\n"; # TODO include class
  }

  $s .= docSignature $m;

  $s .= section("\n#### Parameters\n\n", $m->{fullcomment}->{param}, sub {
      my $x = shift;
      return sprintf "* `%s` - %s\n", $x->{name}, $x->{description};
    });

  my @returns = grep { $_->{name} eq 'return' } @{$m->{fullcomment}->{block}};

  if (@returns) {
    $s .= "\n#### Returns\n\n$returns[0]->{description}\n\n";
  }

  return $s;
}

my $fh = IO::Handle->new();

open $fh, '>', "index.md" or die "open failed\n";
print $fh "# Speech SDK for Objective-C Reference\n\n";

print $fh "\n## Classes\n\n";
foreach my $if (sort { $a->{name} cmp $b->{name} } @interfaces) {
  print $fh "* [$if->{name} class]($if->{name}.md)\n";
}

print $fh "\n## Protocols\n\n";
foreach my $if (sort { $a->{name} cmp $b->{name} } @protocols) {
  print $fh "* [$if->{name} protocol]($if->{name}.md)\n";
}

print $fh "\n## Enums\n\n";
foreach my $if (sort { $a->{name} cmp $b->{name} } @enums) {
  print $fh "* [$if->{name} enum]($if->{name}.md)\n";
}

print $fh "\n## Typedefs\n\n";
foreach my $if (sort { $a->{name} cmp $b->{name} } @typedefs) {
  print $fh "* [$if->{name} typedef]($if->{name}.md)\n";
}

close $fh;

open $fh, '>', "TOC.md" or die "open failed\n";
print $fh "# [Speech SDK for Objective-C Reference](index.md)\n";
foreach my $if (sort { $a->{name} cmp $b->{name} } (@interfaces, @enums, @protocols, @typedefs)) {
  print $fh "## [$if->{name}]($if->{name}.md)\n";
}
close $fh;

foreach my $e (@enums) {
  open $fh, '>', "$e->{name}.md" or die "open failed\n";
  print $fh "# Enum $e->{name}\n\n";
  printf $fh "%s\n", $e->{fullcomment}{description};

  print $fh section("\n| Name | Description |\n|:-- |:-- |\n", $e->{enumerators}, sub {
      my $x = shift;
      return sprintf "| %s | %s |\n", $x->{name}, $x->{fullcomment}{description};
    });
  close $fh;
}

foreach my $p (@protocols) {
  open $fh, '>', "$p->{name}.md" or die "open failed\n";
  print $fh "# Protocol $p->{name}\n\n";

  print $fh "## Declaration\n\n";
  printf $fh "```objc\n\@protocol %s\n```\n\n", $p->{name};
  printf $fh "%s\n", $p->{fullcomment}{description};

  print $fh section("\n## Methods\n", $p->{method}, \&docMethod);

  close $fh;
}

foreach my $p (@typedefs) {
  my $filename = "$p->{name}.md";
  open $fh, '>', $filename
    or die "Cannot open $filename for writing\n";
  print $fh "# Typedef $p->{name}\n\n";

  print $fh "## Declaration\n\n";
  printf $fh "```objc\n%s\n```\n\n", $p->{type};
  if ($p->{fullcomment}{description}) {
    printf $fh "%s\n", $p->{fullcomment}{description};
  } else {
    warn "Warning: typedef $p->{name}: missing documentation\n";
  }

  close $fh;
}

foreach my $if (@interfaces) {
  open $fh, '>', "$if->{name}.md" or die "open failed\n";
  print $fh "# Class $if->{name}\n\n";

  print $fh "## Declaration\n\n";
  printf $fh "```objc\n\@class %s;\n```\n", $if->{super} ? "$if->{name} : $if->{super}" : $if->{name};

  if ($if->{fullcomment}->{description}) {
    printf $fh "\n## Description\n\n%s\n\n", $if->{fullcomment}->{description};
  }

  # Filter implicit methods (for properties)
  my @explicitMethods = grep { not defined $_->{implicit} } @{$if->{method}};
  my @explicitClassMethods = grep { $_->{visibility} eq '+' } @explicitMethods;
  my @explicitInstanceMethods = grep { $_->{visibility} eq '-' } @explicitMethods;

  print $fh section("\n## Class Methods\n", \@explicitClassMethods, \&docMethod);

  print $fh section("\n## Methods\n", \@explicitInstanceMethods, \&docMethod);

  if (defined $if->{property}) {
    print $fh "\n## Properties\n";
    foreach my $m (@{$if->{property}}) {
      printf $fh "### %s\n\n```objc\n\@property (%s) %s %s;\n```\n\n%s\n\n",
        $m->{name},
        $m->{flags},
        $m->{type},
        $m->{name},
        $m->{fullcomment}{description};
    }
  }
  close $fh;
}
