#!/bin/sh
# 
# Runs CMake to configure build in Git branch-specific directory,
# allows user to custom configuration.
#
# Copyright (C) 2013 Mateusz Loskot <mateusz@loskot.net>
#
# Uses parts and ideas adapted from Boost's bootstrap.sh script
# Copyright (C) 2005, 2006 Douglas Gregor.
# Copyright (C) 2006 The Trustees of Indiana University
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)
#
################################################################################
err()
{
  echo -e "error: $1"
  exit 1
}
msg()
{
    echo "$1"
}
msg_line()
{
    echo -n "$1..."
}
msg_done()
{
    if [[ -n "$1" ]]; then
        echo "$1"
    else
        echo "done"
    fi
}
# Check source directory
is_src_dir()
{
    [[ -f "$1/CMakeLists.txt" ]]
}
# Determine Git branch, if found
function get_git_branch
{
    ref=$(cd $SRC_DIR && exec git symbolic-ref HEAD 2> /dev/null) || return
    echo "${ref#refs/heads/}"
}
################################################################################
PREFIX=""
SRC_DIR="$PWD"
BUILD_DIR="$PWD/_build"
CMAKE_OPTIONS=""

for option
do
    case $option in

    -help | --help | -h)
      show_help=yes
      ;;
    -prefix=* | --prefix=*)
      PREFIX=`expr "x$option" : "x-*prefix=\(.*\)"`
      ;;
    -src-dir=* | --src-dir=*)
      SRC_DIR=`expr "x$option" : "x-*src-dir=\(.*\)"`
      ;;
    -build-dir=* | --build-dir=*)
      BUILD_DIR=`expr "x$option" : "x-*build-dir=\(.*\)"`
      ;;
    -D*)
        CMAKE_OPTION=`expr "x$option" : "x\(-D.*\)"`
        CMAKE_OPTIONS="${CMAKE_OPTIONS} ${CMAKE_OPTION}"
      ;;
    -*)
      err "unrecognized option: $option"
      ;; 

    esac
done
if [ "x$show_help" = "xyes" ]; then
  cat <<EOF
\`./configure.sh' runs CMake to configure build

Usage: $0 [OPTION]... 

Defaults for the options are specified in brackets.

EOF
fi
[[ -n "$show_help" ]] && exit 0

msg "Preparing build using "

msg_line "Looking for sources in '$SRC_DIR'"
if ! is_src_dir "$SRC_DIR"; then
    err "cannot find sources in '$SRC_DIR'"
fi
msg_done

msg_line "Detecting current Git branch"
BRANCH=$(get_git_branch)
if [ -n $BRANCH ]; then
    SUFFIX=$(echo $BRANCH | sed -e 's/\//\-/g')
    BUILD_DIR="$BUILD_DIR-$SUFFIX"
fi
msg_done $BRANCH

# Create build directory
if [ ! -d "$BUILD_DIR" ]; then
    msg_line "Creating build directory in '$BUILD_DIR'"
    mkdir -p $BUILD_DIR
    msg_done
fi
[[ ! -d "$BUILD_DIR" ]] && err "cannot find '$BUILD_DIR' directory"

# Run cmake
msg "Running CMake in build directoru '$BUILD_DIR'"
msg "CMake options:"
msg " ${CMAKE_OPTIONS}"

(cd $BUILD_DIR; cmake -DCMAKE_INSTALL_PREFIX:PATH="$PREFIX" $CMAKE_OPTIONS "$SRC_DIR")

