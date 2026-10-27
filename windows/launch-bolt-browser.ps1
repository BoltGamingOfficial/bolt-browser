$ErrorActionPreference = 'Stop'

# Opens the repository's BOLT start page in an existing Firefox installation.
# This script does not change Firefox policies, profiles, defaults, or settings.

$repoRoot = Split-Path -Parent $PSScriptRoot
$startPage = Join-Path $repoRoot 'startpage\index.html'
if (-not (Test-Path -LiteralPath $startPage -PathType Leaf)) {
    throw "BOLT Browser start page not found: $startPage"
}

$candidates = @()
$command = Get-Command 'firefox.exe' -ErrorAction SilentlyContinue
if ($command) { $candidates += $command.Source }
$candidates += @(
    (Join-Path $env:ProgramFiles 'Mozilla Firefox\firefox.exe'),
    (Join-Path ${env:ProgramFiles(x86)} 'Mozilla Firefox\firefox.exe'),
    (Join-Path $env:LOCALAPPDATA 'Mozilla Firefox\firefox.exe')
)
$firefox = $candidates | Where-Object { $_ -and (Test-Path -LiteralPath $_ -PathType Leaf) } | Select-Object -First 1
if (-not $firefox) {
    Write-Error 'Firefox for Windows was not found. Install it from https://www.mozilla.org/firefox/ and run this script again.'
    exit 1
}

$startPageUri = ([System.Uri]::new((Resolve-Path -LiteralPath $startPage).Path)).AbsoluteUri
Write-Host "Opening BOLT Browser start page in: $firefox"
Start-Process -FilePath $firefox -ArgumentList @('-new-tab', $startPageUri)
