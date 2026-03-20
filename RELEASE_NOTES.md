# Astonia3 Fan Server Release v0.1.1

## What’s new
- Fixed MinGW build path and CI build failures (`create_account` now includes `<string.h>`).
- Added cross-platform `scripts/release-windows.ps1` to build and package a Windows release automatically.
- Added improved monorepo layout: `astonia_community_server3/astonia_community_client`, `launcher/` and top-level `README.md`.
- Added release helper script `run-server-client.ps1` that starts server + client and port-forwarding helper.

## How to use this release (Windows)
1. Download and extract `artifacts/astonia3-windows-<timestamp>.zip`.
2. Open PowerShell as administrator if you need external connections.
3. Start local server+client with GUI:
   - `launcher\astonia-launcher.exe`
   - Set server host (usually `127.0.0.1`) and port (default `5556`).
   - Press `Start Both`.
4. Or start server only:
   - `astonia_community_server3\run-server-client.ps1 -NoClient`
5. To forward port for remote players:
   - `Set-PortForward -ListenPort 5556 -ConnectPort 5556`

## Manual rebuild (optional)
- Server:
  - `cd astonia_community_server3`
  - `C:\mingw32\mingw32\bin\mingw32-make.exe -f Makefile.win`
- Launcher:
  - `cd launcher`
  - `C:\mingw32\mingw32\bin\mingw32-make.exe -f Makefile`
- Client: build using MSYS2 as described in `astonia_community_server3/astonia_community_client/readme.md`.

## Release bundle includes
- `astonia_community_server3/server.exe`
- `launcher/astonia-launcher.exe`
- `astonia_community_server3/run-server-client.ps1`
- `README.md`, `RELEASE_NOTES.md`

## Validation
- Build + package workflow executed with `scripts/release-windows.ps1`.
- `gh release create v0.1.1` performed and verified.
