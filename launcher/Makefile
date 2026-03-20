CC = C:/mingw32/mingw32/bin/gcc
CFLAGS = -O2 -mwindows
RM = del /Q

all: astonia-launcher.exe

astonia-launcher.exe: launcher.c
	$(CC) $(CFLAGS) -o $@ $< -lshlwapi

clean:
	if exist astonia-launcher.exe $(RM) astonia-launcher.exe
