#!/usr/bin/env bash

NPROCS=$(grep -c ^processor /proc/cpuinfo)

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
PREFIX=$DIR/systemc-ams-dist
SYSTEMC_PREFIX=$DIR/systemc-dist

version=2.3
source=systemc-ams-$version.tar.gz

if [ ! -f "$source" ]; then
	echo "error: missing systemc-ams archive"
	exit
fi

tar xzf $source
cd systemc-ams-$version
mkdir build && cd build
../configure CXXFLAGS='-std=c++14' --prefix=$PREFIX --disable-systemc_compile_check --with-systemc=$SYSTEMC_PREFIX --with-arch-suffix=
make -j$NPROCS
make install

cd $DIR
