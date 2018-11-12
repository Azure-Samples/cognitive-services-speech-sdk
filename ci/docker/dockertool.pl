#!/usr/bin/perl
#
# Copyright (c) Microsoft Corporation. All rights reserved.
# Licensed under the MIT license.
#

use v5.10;
use strict;
use warnings qw/all/;
use IO::Handle;
use Getopt::Long;
use Data::Dumper;
use File::Temp qw/tempdir/;
use Cwd qw/abs_path/;
use File::Basename;
use LWP::Simple;

my ($scriptfile, $scriptdir) = fileparse(abs_path($0));
my @dockerRmiAtExit;

sub dockerBuild {
  my $tag = shift or die "Error: missing tag\n";
  my $directory = shift or die "Error: missing directory\n";
  my $oldImage = qx/docker images -q "$tag"/; # ignore $?

  my @cmd = (qw/docker build --force-rm --tag/, $tag, @_, $directory);
  system(@cmd) == 0
    or die "Failed to run '@cmd', exitcode $?\n";

  my $newImage = qx/docker images -q "$tag"/; # ignore $?

  # Image was updated, remember to delete the old version
  if (defined $oldImage and $oldImage ne $newImage) {
    push @dockerRmiAtExit, $oldImage;
  }
}

sub slurp {
  my $file = shift or die "Error: missing file\n";
  local $/;
  open my $fh, '<', $file
    or die "Cannot open '$file': $!\n";
  my $result = <$fh>;
  close $fh;
  return $result;
}

sub outputDockerfile {
  my ($image, $imageDir) = @_;
  my $fh = IO::Handle->new();
  open $fh, '>', "$imageDir/Dockerfile"
    or die "Error: cannot open file for writing: $!\n";
  for my $snippetname (@{$image->{spec}}) {
    my $path = $scriptdir . "snippets/$snippetname.txt";
    print $fh slurp($path);
  }
  close $fh;
}

sub aptInstallWith {
   return 'apt-deps-begin', @_, 'apt-deps-end';
}

my %images = (
  dev_ubuntu1604_x86 => {
    version => 1,
    spec => [qw/from-ubuntu1604-x86 deps swig builduser/],
  },
  dev_ubuntu1604_arm32 => {
    version => 1,
    spec => [qw/from-ubuntu1604-arm32v7 deps swig builduser/],
    urls => [qw(https://github.com/multiarch/qemu-user-static/releases/download/v2.12.0-1/qemu-arm-static.tar.gz)],
  },
  oobedevcpp_ubuntu1604_x64 => {
    version => 2,
    spec => ['from-ubuntu1604-x64', aptInstallWith(qw/oobedevcpp_ubuntu1604_deps oobe_ubuntu1604_deps test_deps/), 'builduser'],
  },
  oobedevcpp_ubuntu1604_x86 => {
    version => 1,
    spec => ['from-ubuntu1604-x86', aptInstallWith(qw/oobedevcpp_ubuntu1604_deps oobe_ubuntu1604_deps test_deps/), 'builduser'],
  },
  oobejre_ubuntu1604_x64 => {
    version => 1,
    spec => ['from-ubuntu1604-x64', aptInstallWith(qw/oobe_ubuntu1604_deps oobejre_ubuntu1604_deps test_deps/), 'builduser'],
  },
  oobedevdnc20_ubuntu1604_x64 => {
    version => 2,
    spec => ['from-ubuntu1604-x64', aptInstallWith(qw/oobe_ubuntu1604_deps test_deps/), 'oobedevdnc20_ubuntu1604_x64_deps', 'builduser'],
  },
);

sub imagetag {
  my ($optref, $imagename, $image, $imageversion) = @_;
  my $tag = $optref->{registry} ? "$optref->{registry}/" : '';
  $imageversion ||= $image->{version};
  $tag .= "$optref->{repository}/$imagename:$imageversion";
  return $tag;
}

my @commonopts = qw/registry=s repository=s/;

my %commands = (
  build => {
    options => [@commonopts, 'build-arg=s'],
    command => sub {
      my $optref = shift;
      my $tempdir = tempdir('dockertoolXXXXXX', CLEANUP => 1)
        or die "Cannot create temporary directory\n";

      my @images = @{$optref->{extras}} ? @{$optref->{extras}} : sort keys %images;
      for my $imagename (@images) {
        my $image = $images{$imagename};

        my $imagedir = "$tempdir/$imagename";
        mkdir $imagedir or die "Error: cannot create directory '$imagedir': $!\n";
        outputDockerfile $image, $imagedir;

        # Get file dependencies
        for my $url (@{$image->{urls}}) {
          my ($file) = $url =~ m{([^/]+$)}
              or die "Cannot determine file name from URL '$url'.\n";
          my $rc = getstore($url, "$imagedir/$file");
          is_success($rc)
              or die "Download URL '$url' failed with error $rc.\n";
        }

        my $tag = imagetag $optref, $imagename, $image;
        dockerBuild $tag, $imagedir,
          map { ("--build-arg", $_) } @{$optref->{'build-arg'}};
      }

      # Best effort at cleaning up on local machine
      for my $imageToRemove (@dockerRmiAtExit) {
        system qw/docker rmi/, $imageToRemove; # ignore $?
      }
    }
  },
  imagetag => {
    options => [@commonopts],
    command => sub {
      my $optref = shift;
      my @images = @{$optref->{extras}} ? @{$optref->{extras}} : sort keys %images;
      say imagetag($optref, $_, $images{$_}) for @images;
    }
  },
  push => {
    options => [@commonopts],
    command => sub {
      my $optref = shift;
      my @images = @{$optref->{extras}} ? @{$optref->{extras}} : sort keys %images;
      my @errors;
      for my $imagename (@images) {
        my $image = $images{$imagename};
        my $tag = imagetag $optref, $imagename, $image;
        push @errors, $tag if system(qw/docker push/, $tag) != 0;
      }
      warn ("Failed pushing image(s):\n" . join '', (map { "  $_\n" } @errors)) if @errors;
      exit 1 if @errors;
    }
  },
  pull => {
    options => [@commonopts, 'ignore-error', 'try-previous'],
    command => sub {
      my $optref = shift;
      my @images = @{$optref->{extras}} ? @{$optref->{extras}} : sort keys %images;
      my @errors;
      for my $imagename (@images) {
        my $image = $images{$imagename};
        my $tag = imagetag $optref, $imagename, $image;
        my $exitcode = system(qw/docker pull/, $tag);
        if ($exitcode != 0 and $optref->{'try-previous'} and $image->{version} > 1) {
          my $tag2 = imagetag $optref, $imagename, $image, $image->{version} - 1;
          $exitcode = system(qw/docker pull/, $tag2);
          push @errors, $tag2 if $exitcode != 0;
        }
        push @errors, $tag if $exitcode != 0;
      }
      warn ("Failed pulling image(s):\n" . join '', (map { "  $_\n" } @errors)) if @errors;
      exit 1 if @errors and not defined $optref->{'ignore-error'};
    }
  },
);

my $command = shift or die "Error: missing command.\n";
die "Error: unknown command '$command'" unless defined $commands{$command};

# Option hash with defaults.
my %options = (
  repository => 'csspeech',
  'build-arg' => []
);

GetOptions(\%options, @{$commands{$command}{options}}) or die "Stopping.\n";

# Store extra arguments
$options{extras} = \@ARGV;

$commands{$command}{command}(\%options);
