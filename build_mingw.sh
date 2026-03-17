#!/bin/sh
export PATH=/c/mingw32/mingw32/bin:$PATH
export CC=/c/mingw32/mingw32/bin/i686-w64-mingw32-gcc
export CFLAGS='-Wall -Wshadow -Wno-pointer-sign -fno-strict-aliasing -O3 -g -DSTAFF'
export LDFLAGS='-L/c/mingw32/mingw32/lib'
/c/mingw32/mingw32/bin/mingw32-make CFLAGS='-Wall -Wshadow -Wno-pointer-sign -fno-strict-aliasing -O3 -g -DSTAFF' LDFLAGS='-L/c/mingw32/mingw32/lib' CC=/c/mingw32/mingw32/bin/i686-w64-mingw32-gcc -j1
