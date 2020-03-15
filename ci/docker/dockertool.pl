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

sub yumInstallWith {
   return 'yum-deps-begin', @_, 'yum-deps-end';
}

my %images = (
  dev_debian9_x64 => { # Only used for source build (OpenSSL 1.1)
    version => 1,
    spec => [
        # Stage 0
        qw/from-debian9-x64 stage_cmake_binary_ubuntu1604_x64/,
        # Stage 1
        qw/from-debian9-x64 stage_swig_ubuntu1604/,
        # Image
        qw/from-debian9-x64 copy-layer-01-usr-local/,
        aptInstallWith(qw/devcore_debian9_deps devjava_debian9_deps/),
        'builduser'],
  },
  dev_ubuntu1604_x64 => { # Not used yet
    version => 2,
    spec => [
        # Stage 0
        qw/from-ubuntu1604-x64 stage_cmake_binary_ubuntu1604_x64/,
        # Stage 1
        qw/from-ubuntu1604-x64 stage_swig_ubuntu1604/,
        # Image
        qw/from-ubuntu1604-x64 copy-layer-01-usr-local/,
        aptInstallWith(qw/devcore_ubuntu1604_deps devjava_ubuntu1604_deps/),
        'builduser'],
  },
  dev_ubuntu1604_x86 => {
    version => 3,
    spec => [
        # Stage 0
        qw/from-ubuntu1604-x86 stage_cmake_fromsource_ubuntu1604/,
        # Stage 1
        qw/from-ubuntu1604-x86 stage_swig_ubuntu1604/,
        # Image
        qw/from-ubuntu1604-x86 copy-layer-01-usr-local/,
        aptInstallWith(qw/devcore_ubuntu1604_deps devjava_ubuntu1604_deps/),
        'builduser'],
  },
  dev_ubuntu1604_arm32 => {
    version => 3,
    urls => [qw(https://github.com/multiarch/qemu-user-static/releases/download/v2.12.0-1/qemu-arm-static.tar.gz)],
    spec => [
        # Stage 0
        qw/from-ubuntu1604-arm32v7 stage_cmake_fromsource_ubuntu1604/,
        # Stage 1
        qw/from-ubuntu1604-arm32v7 stage_swig_ubuntu1604/,
        # Image
        qw/from-ubuntu1604-arm32v7 copy-layer-01-usr-local/,
        aptInstallWith(qw/devcore_ubuntu1604_deps devjava_ubuntu1604_deps/),
        'builduser'],
  },
  dev_ubuntu1604_arm64 => {
    version => 1,
    urls => [qw(https://github.com/multiarch/qemu-user-static/releases/download/v4.0.0/qemu-aarch64-static.tar.gz)],
    spec => [
        # Stage 0
        qw/from-ubuntu1604-arm64v8 stage_cmake_fromsource_ubuntu1604/,
        # Stage 1
        qw/from-ubuntu1604-arm64v8 stage_swig_ubuntu1604/,
        # Image
        qw/from-ubuntu1604-arm64v8 copy-layer-01-usr-local/,
        aptInstallWith(qw/devcore_ubuntu1604_deps devjava_ubuntu1604_deps/),
        'builduser'],
  },
  oobedevcpp_ubuntu1804_x64 => {
    version => 3,
    spec => ['from-ubuntu1804-x64', aptInstallWith(qw/oobedevcpp_ubuntu1604_deps oobe_ubuntu1604_deps oobe_ubuntu_gstreamer_deps test_deps_ubuntu1804/), 'builduser'],
  },
  oobedevcpp_ubuntu1604_x64 => {
    version => 4,
    spec => ['from-ubuntu1604-x64', aptInstallWith(qw/oobedevcpp_ubuntu1604_deps oobe_ubuntu1604_deps oobe_ubuntu_gstreamer_deps test_deps/), 'builduser'],
  },
  oobedevcpp_debian9_x64 => { # stretch
    version => 1,
    spec => ['from-debian9-x64', aptInstallWith(qw/oobedevcpp_ubuntu1604_deps oobe_debian9_deps oobe_ubuntu_gstreamer_deps test_deps/), 'builduser'],
  },
  oobedevcpp_fedora27_x64 => { # Note: not used yet, not published yet; just work-in-progress.
    version => 'TESTLOCAL',
    spec => ['from-fedora27-x64', yumInstallWith(qw/oobedevcpp_fedora oobe_fedora_deps test_deps/), 'builduser'],
  },
  oobedevcpp_centos8_x64 => {
    version => 3,
    spec => ['from-centos8-x64', yumInstallWith(qw/oobe_centos_deps oobedevcpp_centos_deps oobe_centos_gstreamer_deps test_deps/), 'builduser', 'set_env_centos'],
  },
  oobedevcpp_centos7_x64 => {
    version => 1,
    spec => [
        # Stage 0
        qw/from-centos7-x64 stage_gcc_fromsource_centos7_x64/,
        # Image
        qw/from-centos7-x64 copy-layer-0-gcc-usr-local/,
        yumInstallWith(qw/oobe_centos_deps oobedevcpp_centos_deps oobedevcpp_centos7_deps oobe_centos_gstreamer_deps test_deps/),
        'builduser',
        'set_env_centos'],
  },
  oobedevcpp_ubuntu1804_x86 => {
    version => 3,
    spec => ['from-ubuntu1804-x86', aptInstallWith(qw/oobedevcpp_ubuntu1604_deps oobe_ubuntu1604_deps oobe_ubuntu_gstreamer_deps test_deps_ubuntu1804/), 'builduser'],
  },
  oobedevcpp_ubuntu1604_x86 => {
    version => 3,
    spec => ['from-ubuntu1604-x86', aptInstallWith(qw/oobedevcpp_ubuntu1604_deps oobe_ubuntu1604_deps oobe_ubuntu_gstreamer_deps test_deps/), 'builduser'],
  },
  oobedevcpp_debian9_x86 => { # stretch
    version => 1,
    spec => ['from-debian9-x86', aptInstallWith(qw/oobedevcpp_ubuntu1604_deps oobe_debian9_deps oobe_ubuntu_gstreamer_deps test_deps/), 'builduser'],
  },
  oobejre_ubuntu1604_x64 => {
    version => 3,
    spec => ['from-ubuntu1604-x64', aptInstallWith(qw/oobe_ubuntu1604_deps oobejre_ubuntu1604_deps oobe_ubuntu_gstreamer_deps test_deps/), 'builduser'],
  },
  oobejre_ubuntu1804_x64 => {
    version => 3,
    spec => ['from-ubuntu1804-x64', aptInstallWith(qw/oobe_ubuntu1604_deps oobejre_ubuntu1604_deps oobe_ubuntu_gstreamer_deps test_deps_ubuntu1804/), 'builduser'],
  },
  oobejre_debian9_x64 => {
    version => 1,
    spec => ['from-debian9-x64', aptInstallWith(qw/oobe_debian9_deps oobejre_ubuntu1604_deps oobe_ubuntu_gstreamer_deps test_deps/), 'builduser'],
  },
  oobejre_centos8_x64 => {
    version => 3,
    spec => ['from-centos8-x64', yumInstallWith(qw/oobe_centos_deps oobejre_centos_deps oobe_centos_gstreamer_deps test_deps/), 'builduser', 'set_env_centos'],
  },
  oobejre_centos7_x64 => {
    version => 1,
    spec => [
        # Stage 0
        qw/from-centos7-x64 stage_gcc_fromsource_centos7_x64/,
        # Image
        qw/from-centos7-x64 copy-layer-0-gcc-usr-local/,
        yumInstallWith(qw/oobe_centos_deps oobejre_centos_deps oobe_centos_gstreamer_deps test_deps/),
        'builduser',
        'set_env_centos'],
  },
  oobedevdnc20_ubuntu1604_x64 => {
    version => 4,
    spec => ['from-ubuntu1604-x64', aptInstallWith(qw/oobe_ubuntu1604_deps oobe_ubuntu_gstreamer_deps test_deps/), 'oobedevdnc20_ubuntu1604_x64_deps', 'builduser'],
  },
  oobedevdnc21_ubuntu1804_x64 => {
    version => 3,
    spec => ['from-ubuntu1804-x64', aptInstallWith(qw/oobe_ubuntu1604_deps oobe_ubuntu_gstreamer_deps test_deps_ubuntu1804/), 'oobedevdnc21_ubuntu1804_x64_deps', 'builduser'],
  },
  oobedevdnc20_debian9_x64 => {
    version => 1,
    spec => ['from-debian9-x64', aptInstallWith(qw/oobe_debian9_deps oobe_ubuntu_gstreamer_deps test_deps/), 'oobedevdnc20_debian9_x64_deps', 'builduser'],
  },
  oobedevdnc21_centos8_x64 => {
    version => 3,
    spec => [
        # Stage 0
        qw/from-centos8-x64 stage_dotnet_binary_centos8_x64/,
        # Image
        qw/from-centos8-x64 copy-layer-0-dotnet-usr-local/,
        yumInstallWith(qw/oobe_centos_deps oobedevdnc21_centos8_deps oobe_centos_gstreamer_deps test_deps/),
        'builduser',
        'set_env_centos'],
  },
  oobedevdnc21_centos7_x64 => {
    version => 1,
    spec => [
        # Stage 0
        qw/from-centos7-x64 stage_gcc_fromsource_centos7_x64/,
        # Image
        qw/from-centos7-x64 copy-layer-0-gcc-usr-local/,
        'config_microsoft_packages_centos7',
        yumInstallWith(qw/oobe_centos_deps oobedevdnc21_centos7_deps oobe_centos_gstreamer_deps test_deps/),
        'builduser',
        'set_env_centos'],
  },
  # .NET Core development against official .NET Core images from the Microsoft Container Registry
  oobedevdnc21_mcr_x64 => {
    version => 1,
    # Note: libssl1.0.2 is already in this specific image...
    spec => ['from-mcrdncsdk21-x64', aptInstallWith(qw/oobe_debian9_deps oobe_ubuntu_gstreamer_deps test_deps/), 'builduser'],
  },
  oobedevpy35_debian9_x64 => {
    version => 1,
    spec => ['from-debian9-x64', aptInstallWith(qw/oobe_debian9_deps oobepython_ubuntu_deps test_deps/), 'oobepython_jupyter', 'builduser'],
  },
  oobedevpy35_ubuntu1604_x64 => {
    version => 3,
    spec => ['from-ubuntu1604-x64', aptInstallWith(qw/oobe_ubuntu1604_deps oobepython_ubuntu_deps test_deps/), 'oobepython_jupyter', 'builduser'],
  },
  oobedevpy36_ubuntu1804_x64 => {
    version => 3,
    spec => ['from-ubuntu1804-x64', aptInstallWith(qw/oobe_ubuntu1604_deps oobepython_ubuntu_deps test_deps_ubuntu1804/), 'oobepython_jupyter', 'builduser'],
  },
  oobedevpy37_debian9py37_x64 => {
    version => 1,
    # Note: libssl1.0.2 is already in this specific image...
    spec => ['from-python37-x64', aptInstallWith(qw/oobe_debian9_deps test_deps/), 'oobepython_jupyter', 'builduser'],
  },
  oobedevpy36_centos8_x64 => {
    version => 3,
    spec => ['from-centos8-x64', yumInstallWith(qw/oobe_centos_deps oobepython_centos_deps oobe_centos_gstreamer_deps test_deps/), 'oobepython_jupyter', 'builduser', 'set_env_centos'],
  },
  oobedevpy36_centos7_x64 => {
    version => 1,
    spec => [
        # Stage 0
        qw/from-centos7-x64 stage_gcc_fromsource_centos7_x64/,
        # Image
        qw/from-centos7-x64 copy-layer-0-gcc-usr-local/,
        yumInstallWith(qw/oobe_centos_deps oobepython_centos_deps oobe_centos_gstreamer_deps test_deps/),
        'oobepython_jupyter',
        'builduser',
        'set_env_centos'],
  },
  ubuntu1604_arm64cc => {
    version => 1,
    # Note: libssl1.0.2 is already in this specific image...
    spec => ['from-python37-x64', aptInstallWith(qw/oobe_debian9_deps test_deps/), 'oobepython_jupyter', 'builduser'],
    },
  ubuntu1604_arm32cc => {
    version => 1,
    # Note: cross-compile toolsand OpenSSL 1.1.1b is already in this specific image...
    spec => ['from-debian9-x86', aptInstallWith(qw/oobedevcpp_ubuntu1604_deps oobe_debian9_deps oobe_ubuntu_gstreamer_deps test_deps/), 'builduser'],
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

__END__

=head1 NAME

dockertool - helper for managing Speech SDK related Docker images

=head1 SYNOPSIS

B<dockertool>
B<build>
[B<--registry> I<reg>]
[B<--repository> I<repo>]
[B<--build-arg> I<name=value>]
[I<image>...]

B<dockertool>
B<imagetag>
[B<--registry> I<reg>]
[B<--repository> I<repo>]
[I<image>...]

B<dockertool>
B<push>
[B<--registry> I<reg>]
[B<--repository> I<repo>]
[I<image>...]

B<dockertool>
B<pull>
[B<--registry> I<reg>]
[B<--repository> I<repo>]
[B<--ignore-error>]
[B<--try-previous>]
[I<image>...]

=head1 DESCRIPTION

B<dockertool> is a helper script to manage Docker images related to Speech SDK development.
These images are defined internally to the script in a data structure named C<%images>.
For each image three keys are defined, I<version>, I<spec>, and, optionally, I<url>:

=over 4

=item

The version I<version> is a positive integer or the string constant C<'TESTLOCAL'>.
The version of a certain image is currently used both as definition (which version of the image to build) as well as a reference (which image version is required for the current commit of the Speech SDK, say, for testing samples).

Build tooling external to this script uses the I<TESTLOCAL> version value to indicate that an image should be built on-the-fly rather than pulling it from a registry.
This provides a cheap way to test image modifications (locally or in Azure DevOps) before pushing out final images to a Docker registry.

=item

The specification I<spec> is an array of references to snippet files stored in the F<snippets/> directory.
An image's I<Dockerfile> is formed by concatenating the contents of all referenced snippet files.

=item

I<urls> is an array of URL references to download and place next to the (generated) Dockerfile when building images.

=back

The full name (tag) of an image at the Docker level has the form I<REGISTRY/REPO/IMAGE:VERSION>.
By default, I<REGISTRY> is empty and I<REPO> is I<csspeech>,
use the B<--registry> and B<--repository> options to override in all commands.

=head1 COMMANDS

The first parameters to B<dockertool> designate a command to run.
Each command takes a number of options, and (optionally) a list of images to operate on.
If the list is omitted, the command operates on all images.

=over 4

=item B<build>

Build images locally.

=item B<imagetag>

List the full names of images (including registry, if specified, repository, and version).

=item B<push>

Push images to a registry.
You must be logged in to the registry already (for example using I<az acr login> for a private Azure Container Registry).

=item B<pull>

Pull images from a registry.
You must be logged in to the registry already (for example using I<az acr login> for a private Azure Container Registry).

=back

=head1 OPTIONS

=over 4

=item B<--build-arg> I<name=value>

When using the I<build> command, forward this as a B<--build-arg> option to Docker's B<build> command.
May be specified multiple times.

=item B<--ignore-error>

When using the I<pull> command, return with success even on errors when pulling images.

=item B<--try-previous>

When using the I<pull> command, also attempt to pull the previous version of an image if pulling the current one failed.

=item B<--editor> I<editor>

Use I<editor> to edit the matches.
If this option is not given the editor defaults to the one specified with environment variable I<EDITOR> or vim.

=item B<--registry> I<reg>

Specify a registry to associate with all images.
Defaults to empty if not specified.

=item B<--repository> I<repo>

Specify a repository to associate with all images.
Defaults to I<csspeech> if not specified.

=back

=head1 EXAMPLES

=over 4

=item Log into Speech SDK's private registry and pull all images

  az acr login --name csspeechsdkacr201809
  perl dockertool.pl pull --registry csspeechsdkacr201809.azurecr.io

=item Log into Speech SDK's private registry and push a new version of an image

  az acr login --name csspeechsdkacr201809
  perl dockertool.pl push --registry csspeechsdkacr201809.azurecr.io oobedevcpp_ubuntu1804_x64

=item Get full image names in the context of a registry

  perl dockertool.pl imagetag --registry csspeechsdkacr201809.azurecr.io

=item Log into Speech SDK's private registry and pull all images

  az acr login --name csspeechsdkacr201809
  perl dockertool.pl pull --registry csspeechsdkacr201809.azurecr.io

=item Build an image and run it locally

  fullImage=$(perl dockertool.pl imagetag --repository myuser oobedevcpp_ubuntu1604_x86)
  perl dockertool.pl build \
    --repository myuser oobedevcpp_ubuntu1604_x86 \
    --build-arg BUILD_UID=$(id -u) \
    --build-arg BUILD_USER=$(id -un)
  docker run --rm -ti $fullImage bash

=back

=head1 CAVEATS

This is simplistic by design.
The main purpose is to avoid redundancy when defining Dockerfiles for a number of images.
Alternate approaches using a real text templating engine would work as well.

=head1 COPYRIGHT

Copyright (c) Microsoft Corporation. All rights reserved.
Licensed under the MIT license.
