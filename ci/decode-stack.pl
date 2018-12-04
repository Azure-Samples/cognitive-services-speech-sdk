#!/usr/bin/perl -ln
BEGIN {
  print "Trying to decode any stacks";
  our %RE;
  $RE{hex} = qr/0x[0-9a-f]+/;
  $RE{backtrace} = qr(
    ^ (?<binary>\S+?)
    \( (?: (?<func>\S*) \+ (?<raddr>$RE{hex}) )? \) \s
    \[ (?<addr>$RE{hex}) \]
    \r?$
  )x;
}
END {
  print "Decoded $stackcnt stack(s). Note: not all necessarily fatal." if $stackcnt;
}
if (/CALL STACK BEGIN/ .. /CALL STACK END/) {
  $line ||= $.;
  push @a, $_;
} elsif (@a) {
  print "There's a stack, line $., depth $#a!";
  $stackcnt++;

  if ($#a < 1024) {
    my %bases;

    # Try to compute the base addresses of each binary
    for (@a) {
      my $base;
      if (m($RE{backtrace})) {
        if ($+{func} eq '') {
          if (defined $+{raddr}) {
            $base = hex($+{addr}) - hex($+{raddr});
          } else {
            $base = 0;
          }
          my $oldbase = $bases{$+{binary}} ||= $base;
          warn "Base mismatch $base != $oldbase for binary $+{binary}\n"
            if $base != $oldbase;
        }
      }
    }

    # Try to gather additional info via addr2line (and make pretty using c++filt)
    for (@a) {
      print;
      if (m($RE{backtrace}) and defined $bases{$+{binary}} and -x $+{binary}) {
        my $addr = sprintf '0x%x', hex($+{addr}) - $bases{$+{binary}};
        $addrToLine = `addr2line -i -f -e "$+{binary}" "$addr" 2>/dev/null | c++filt`;
        if ($? == 0) {
          for (split "\n", $addrToLine) {
            chomp;
            print "  # $_";
          }
        }
      }
    }
  } else {
    print for @a;
  }
  @a = ();
  $line = undef;
}
