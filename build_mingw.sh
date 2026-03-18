#!/bin/sh
export PATH=/c/mingw32/mingw32/bin:$PATH
export CC=/c/mingw32/mingw32/bin/i686-w64-mingw32-gcc
export CFLAGS='-Wall -Wshadow -Wno-pointer-sign -fno-strict-aliasing -O3 -g -DSTAFF -I/c/development/astionia3/astonia_community_server3/include'
export LDFLAGS='-L/c/mingw32/mingw32/lib -lws2_32'
export C_INCLUDE_PATH=/c/development/astionia3/astonia_community_server3/include
export CPLUS_INCLUDE_PATH=/c/development/astionia3/astonia_community_server3/include
/c/mingw32/mingw32/bin/mingw32-make CFLAGS="$CFLAGS" LDFLAGS="$LDFLAGS" CC=$CC -j1
