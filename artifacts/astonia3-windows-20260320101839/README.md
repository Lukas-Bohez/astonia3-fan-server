# Astonia3 Monorepo (Server + Client)

This repository contains both the **Astonia 3 server** and the **Astonia 3 community client**, plus a small GUI launcher to start them together.

## Structure

- `astonia_community_server3/` - Server source and build scripts
- `astonia_community_server3/astonia_community_client/` - Client source and build scripts (moved inside server folder)
- `launcher/` - Simple Windows GUI launcher that starts server + client

## Quick Start (Windows)

### 1) Build the server (MinGW)

```powershell
cd .\astonia_community_server3
..\winlibs-i686\mingw32\bin\mingw32-make.exe -f Makefile.win
```

### 2) Build the client (MSYS2 / Clang)

From within MSYS2 (clang shell):

```bash
cd /c/development/astionia3/astonia_community_server3/astonia_community_client
make
```

### 3) Build the launcher

```powershell
cd ..\..\launcher
..\winlibs-i686\mingw32\bin\mingw32-make.exe -f Makefile
```

### 4) Run server + client

```powershell
cd ..\astonia_community_server3
..\launcher\astonia-launcher.exe
```

The launcher lets you choose the server host/port and will start the server and client in one click.

## Notes

- The server supports a `-p <port>` option so the launcher can start the server on a known port.
- The launcher can also be used to connect to remote servers by setting the host.
- Built artifacts like `*.exe` are ignored by `.gitignore`.
