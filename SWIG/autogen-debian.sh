#! /bin/sh

# Bootstrap the development environment - add extra files needed to run configure. 
# Note autoreconf should do what this file achieves, but it has a bug when working with automake!
# The latest config.guess and config.sub should be copied into Tools/config and checked into CVS
# when upgrading the autotools. Otherwise this script will ensure the latest is copied from 
# your autotool installation.

set -x
test -d Tools/config || mkdir Tools/config
aclocal-1.7 -I Tools/config
autoheader2.50
automake-1.7 --add-missing --copy --force-missing
autoconf2.50
(cd Tools; autoconf2.50)

