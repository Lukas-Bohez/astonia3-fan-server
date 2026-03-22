<#
Build and package Astonia 3 (server + launcher + client binary if available) on Windows.
Usage:
  .\scripts\release-windows.ps1 [-Configuration Release] [-Output artifacts]
#>
param(
    [string]$Configuration = 'Release',
    [string]$OutputDir = 'artifacts',
    [string]$MingwPath = 'C:\mingw32\mingw32',
    [string]$Msys2Prefix = 'C:\msys64\clang64',
    [string]$ServerDir = 'astonia_community_server3',
    [string]$ClientDir = 'astonia_community_server3\\astonia_community_client',
    [string]$LauncherDir = 'launcher'
)

Set-StrictMode -Version Latest

$root = Resolve-Path .
Push-Location $root

Write-Host "Building server..."
$serverMake = Join-Path $MingwPath 'bin\mingw32-make.exe'
if (-not (Test-Path $serverMake)) { throw "mingw32-make not found: $serverMake" }

$serverBuildResult = & $serverMake -f "$ServerDir\Makefile.win" CC="$MingwPath\bin\gcc"
if ($LASTEXITCODE -ne 0) { throw "Server build failed (exit code $LASTEXITCODE)" }

Write-Host "Building launcher..."
Push-Location "$LauncherDir"
$launcherBuildResult = & $serverMake -f "Makefile" CC="$MingwPath\bin\gcc"
$launcherExit = $LASTEXITCODE
Pop-Location
if ($launcherExit -ne 0) { throw "Launcher build failed (exit code $launcherExit)" }

Write-Host "Optional: build client (if you have MSYS2, may require SDL2, Rust, etc)."
$clientExecutable = Join-Path $ClientDir 'bin\moac.exe'
if (-not (Test-Path $clientExecutable)) {
    Write-Warning "Client binary not found in $ClientDir\bin. Build client with MSYS2 manually as needed."
} else {
    Write-Host "Client binary found: $clientExecutable"
}

# Package release
$stamp = Get-Date -Format yyyyMMddHHmmss
$releaseName = "astonia3-windows-$stamp"
$releaseDir = Join-Path $OutputDir $releaseName
New-Item -ItemType Directory -Path $releaseDir -Force | Out-Null

Copy-Item -Path "$ServerDir\server.exe" -Destination $releaseDir -Force
Copy-Item -Path "$LauncherDir\astonia-launcher.exe" -Destination $releaseDir -Force
if (Test-Path $clientExecutable) {
    Copy-Item -Path $clientExecutable -Destination $releaseDir -Force
}

# Copy required DLL dependencies for Windows runtime
$runtimeBins = @()
$runtimeBins += "$MingwPath\bin"
if (Test-Path 'C:\msys64\clang64\bin') { $runtimeBins += 'C:\msys64\clang64\bin' }
if (Test-Path 'C:\msys64\mingw64\bin') { $runtimeBins += 'C:\msys64\mingw64\bin' }

$requiredDlls = @(
    'libwinpthread-1.dll',
    'libgcc_s_dw2-1.dll',
    'libstdc++-6.dll',
    'SDL3.dll',
    'SDL3_mixer.dll',
    'zlib1.dll',
    'libpng16-16.dll',
    'libzip.dll',
    'dwarfstack.dll',
    'libmimalloc.dll'
)

$missingDeps = @()
foreach ($dll in $requiredDlls) {
    $found = $false
    foreach ($bin in $runtimeBins) {
        $src = Join-Path $bin $dll
        if (Test-Path $src) {
            Copy-Item -Path $src -Destination $releaseDir -Force
            $found = $true
            break
        }
    }
    if (-not $found) {
        Write-Warning "Dependency not found: $dll (skipped). Please install in system path or provide manually."
        $missingDeps += $dll
    }
}

if ($missingDeps.Count -gt 0) {
    throw "Missing required runtime DLLs: $($missingDeps -join ', '). Cannot create a fully functioning release package without them."
}

Copy-Item -Path "$ServerDir\run-server-client.ps1" -Destination $releaseDir -Force
Copy-Item -Path "README.md" -Destination $releaseDir -Force
Copy-Item -Path ".gitignore" -Destination $releaseDir -Force

$archive = Join-Path $OutputDir "$releaseName.zip"
if (Test-Path $archive) { Remove-Item $archive -Force }
Compress-Archive -Path "$releaseDir\*" -DestinationPath $archive -Force

Write-Host "Release package created: $archive"
Write-Host "Contents in: $releaseDir"

Pop-Location
