#!/usr/bin/perl -n
BEGIN {
  $c_s = qr(^\s*// <(\w+)>);
  $c_e = qr(^\s*// </(\w+)>);
  $xml_s = qr(^\s*<!-- <(\w+)> -->);
  $xml_e = qr(^\s*<!-- </(\w+)> -->);
  $err = 0
}
sub ateof() {
  if (defined $m) {
    warn "DocFx codetags: tag $m not closed at end of file $oldargv.\n";
    $err++
  }
}
if ($ARGV ne $oldargv) {
  $. = 1;
  ateof();
  $oldargv = $ARGV;
  $m = undef;

  if ($ARGV =~ /\.xa?ml$|\.html$/) {
    $s = $xml_s;
    $e = $xml_e;
  } else {
    $s = $c_s;
    $e = $c_e;
  }
}

if (defined $m) {
  if (m/$s/) {
    warn "DocFx codetags: nested tag $1 not supported in file $ARGV, line $.\n";
    $m = $1;
    $err++
  }
  if (m/$e/) {
    if ($1 ne $m) {
      warn "DocFx codetags: Unmatched tag $1 in file $ARGV, line $.\n";
      $err++
    }
    $m = undef;
  }
} else {
  if (m/$s/) {
    $m = $1;
  }
  if (m/$e/) {
    warn "DocFx codetags: Unexpected end tag $1 in file $ARGV, line $.\n";
    $err++
  }
}

END {
  ateof();
  exit 1 if $err > 0;
}
