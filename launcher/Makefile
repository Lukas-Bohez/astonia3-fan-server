CC = C:/development/astionia3/winlibs-i686/mingw32/bin/gcc
CFLAGS = -O2 -mwindows

all: astonia-launcher.exe

astonia-launcher.exe: launcher.c
	$(CC) $(CFLAGS) -o $@ $< -lshlwapi

clean:
	rm -f astonia-launcher.exe
