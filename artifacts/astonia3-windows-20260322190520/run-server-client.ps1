<#
Simple launcher for running the Astonia server + client together on Windows.

Usage:
  .\run-server-client.ps1            # starts server (in a new window) and then starts the client
  .\run-server-client.ps1 -NoServer  # only launches the client
  .\run-server-client.ps1 -NoClient  # only launches the server
  .\run-server-client.ps1 -Port 3200  # override expected server port (default: 3200)

If you want external players to connect, run PowerShell as Administrator and use
`Set-PortForward -ListenPort 3200 -ConnectPort 3200`.
#>

param(
    [switch]$NoServer,
    [switch]$NoClient,
    [int]$Port = 3200,
    [int]$WaitSeconds = 10,
    [string]$ServerExe = "$PSScriptRoot\server.exe",
    [string]$ClientExe = "$PSScriptRoot\astonia_community_client\bin\moac.exe"
)

function Write-Status {
    param($Message)
    Write-Host "[run-server-client] $Message"
}

function Wait-ForPort {
    param(
        [int]$Port,
        [int]$TimeoutSeconds = 20
    )

    $start = Get-Date
    while ((Get-Date) -lt $start.AddSeconds($TimeoutSeconds)) {
        $res = Test-NetConnection -ComputerName 127.0.0.1 -Port $Port -WarningAction SilentlyContinue
        if ($res.TcpTestSucceeded) {
            return $true
        }
        Start-Sleep -Milliseconds 500
    }
    return $false
}

function Set-PortForward {
    param(
        [int]$ListenPort,
        [int]$ConnectPort,
        [string]$ListenAddress = '0.0.0.0',
        [string]$ConnectAddress = '127.0.0.1'
    )

    if (-not ([bool]((whoami.exe /groups | Select-String "S-1-5-32-544")))) {
        throw "Port forwarding requires Administrator privileges. Run PowerShell as Administrator."
    }

    Write-Status "Adding port proxy: ${ListenAddress}:${ListenPort} -> ${ConnectAddress}:${ConnectPort}"
    netsh interface portproxy add v4tov4 listenaddress=$ListenAddress listenport=$ListenPort connectaddress=$ConnectAddress connectport=$ConnectPort
    Write-Status "Done. Use 'netsh interface portproxy show all' to verify."
}

if (-not $NoServer) {
    if (-not (Test-Path $ServerExe)) {
        throw "Server executable not found: $ServerExe"
    }

    Write-Status "Starting server: $ServerExe"
    Start-Process -FilePath $ServerExe -WorkingDirectory $PSScriptRoot -WindowStyle Minimized

    Write-Status "Waiting for server to open port $Port..."
    if (-not (Wait-ForPort -Port $Port -TimeoutSeconds $WaitSeconds)) {
        Write-Status "Warning: server did not respond on port $Port after $WaitSeconds seconds."
    } else {
        Write-Status "Server listening on port $Port."
    }
}

if (-not $NoClient) {
    if (-not (Test-Path $ClientExe)) {
        throw "Client executable not found: $ClientExe"
    }

    Write-Status "Starting client: $ClientExe"
    Start-Process -FilePath $ClientExe -WorkingDirectory (Split-Path $ClientExe)
}
