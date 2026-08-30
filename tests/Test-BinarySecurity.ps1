[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)] [string]$Executable,
    [Parameter(Mandatory = $true)] [ValidateSet('Win32', 'x64')] [string]$Platform
)

$ErrorActionPreference = 'Stop'

function Find-VisualStudioTool {
    param([Parameter(Mandatory = $true)] [string]$Name)

    $command = Get-Command $Name -ErrorAction SilentlyContinue
    if ($null -ne $command) {
        return $command.Source
    }

    $vswhere = Join-Path ${env:ProgramFiles(x86)} 'Microsoft Visual Studio\Installer\vswhere.exe'
    if (-not (Test-Path -LiteralPath $vswhere -PathType Leaf)) {
        throw "Impossibile trovare vswhere.exe per localizzare $Name."
    }

    $installations = @(& $vswhere -products * -all -property installationPath)
    foreach ($installation in $installations) {
        $tools = @(Get-ChildItem -LiteralPath (Join-Path $installation 'VC\Tools\MSVC') -Directory -ErrorAction SilentlyContinue |
            Sort-Object Name -Descending)
        foreach ($toolset in $tools) {
            $candidate = Join-Path $toolset.FullName "bin\Hostx64\x64\$Name"
            if (Test-Path -LiteralPath $candidate -PathType Leaf) {
                return $candidate
            }
        }
    }
    throw "Impossibile trovare $Name in Visual Studio."
}

function Assert-Contains {
    param([string]$Text, [string]$Pattern, [string]$Description)
    if ($Text -notmatch $Pattern) {
        throw "Protezione mancante nel binario: $Description."
    }
}

$Executable = [System.IO.Path]::GetFullPath($Executable)
if (-not (Test-Path -LiteralPath $Executable -PathType Leaf)) {
    throw "Eseguibile non trovato: $Executable"
}

$dumpbin = Find-VisualStudioTool -Name 'dumpbin.exe'
$headers = (& $dumpbin /nologo /headers $Executable 2>&1) -join [Environment]::NewLine
if ($LASTEXITCODE -ne 0) {
    throw "dumpbin /headers non e riuscito per $Executable"
}
$loadConfig = (& $dumpbin /nologo /loadconfig $Executable 2>&1) -join [Environment]::NewLine
if ($LASTEXITCODE -ne 0) {
    throw "dumpbin /loadconfig non e riuscito per $Executable"
}

if ($Platform -eq 'Win32') {
    Assert-Contains $headers 'machine \(x86\)' 'architettura x86'
    Assert-Contains $headers 'Application can handle large \(>2GB\) addresses' 'LARGEADDRESSAWARE'
}
else {
    Assert-Contains $headers 'machine \(x64\)' 'architettura x64'
    Assert-Contains $headers 'High Entropy Virtual Addresses' 'ASLR ad alta entropia'
}
Assert-Contains $headers 'Dynamic base' 'ASLR'
Assert-Contains $headers 'NX compatible' 'DEP/NX'
Assert-Contains $headers 'Terminal Server Aware' 'compatibilita Terminal Services'
Assert-Contains $headers '(Control Flow Guard|Guard CF)' 'Control Flow Guard nel PE header'
Assert-Contains $loadConfig 'Security Cookie' 'security cookie'
Assert-Contains $loadConfig 'CF instrumented' 'strumentazione Control Flow Guard'
Assert-Contains $loadConfig '[1-9A-Fa-f][0-9A-Fa-f]* Guard CF function count' 'tabella funzioni Control Flow Guard'

Write-Host "Sicurezza PE verificata: $Platform - ASLR, DEP, CFG e security cookie attivi."
