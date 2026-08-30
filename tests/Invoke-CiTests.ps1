[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)] [ValidateSet('Win32', 'x64')] [string]$Platform,
    [int]$TimeoutSeconds = 60
)

$ErrorActionPreference = 'Stop'
$projectRoot = [System.IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$releaseDirectory = Join-Path $projectRoot "srchybrid\$Platform\Release"
$executable = Join-Path $releaseDirectory 'eMuleNext.exe'
$languageDirectory = Join-Path $releaseDirectory 'lang'
$documentationDirectory = Join-Path $releaseDirectory 'docs'

foreach ($requiredPath in @($executable, $languageDirectory, $documentationDirectory)) {
    if (-not (Test-Path -LiteralPath $requiredPath)) {
        throw "Output Release mancante: $requiredPath"
    }
}

$languages = @(Get-ChildItem -LiteralPath $languageDirectory -File -Filter '*.dll' |
    Where-Object { $_.Name -ne 'eMuleNext-GraphicsTest.dll' })
if ($languages.Count -ne 43) {
    throw "Attese 43 lingue, trovate $($languages.Count) in $languageDirectory"
}

& (Join-Path $PSScriptRoot 'Test-BinarySecurity.ps1') -Executable $executable -Platform $Platform

$testRoot = [System.IO.Path]::GetFullPath((Join-Path $PSScriptRoot ('.ci-run-' + [guid]::NewGuid().ToString('N'))))
$allowedPrefix = [System.IO.Path]::GetFullPath($PSScriptRoot).TrimEnd('\') + '\.ci-run-'
if (-not $testRoot.StartsWith($allowedPrefix, [System.StringComparison]::OrdinalIgnoreCase)) {
    throw "Cartella temporanea CI non valida: $testRoot"
}

New-Item -ItemType Directory -Path $testRoot | Out-Null
try {
    Copy-Item -LiteralPath $executable -Destination (Join-Path $testRoot 'eMuleNext.exe')
    Copy-Item -LiteralPath (Join-Path $projectRoot 'packaging\portable\eMuleNext.portable') -Destination $testRoot
    Copy-Item -LiteralPath $languageDirectory -Destination (Join-Path $testRoot 'lang') -Recurse
    Copy-Item -LiteralPath $documentationDirectory -Destination (Join-Path $testRoot 'docs') -Recurse

    $configDirectory = Join-Path $testRoot 'config'
    if (Test-Path -LiteralPath $configDirectory) {
        throw 'Il test di primo avvio non parte da una configurazione vuota.'
    }

    $testExecutable = Join-Path $testRoot 'eMuleNext.exe'
    $process = Start-Process -FilePath $testExecutable -ArgumentList '-ci-self-test' `
        -WorkingDirectory $testRoot -PassThru
    if (-not $process.WaitForExit($TimeoutSeconds * 1000)) {
        $process.Kill()
        throw "Il test di avvio/chiusura non si e concluso entro $TimeoutSeconds secondi."
    }
    $process.Refresh()
    if ($process.ExitCode -ne 0) {
        throw "Il test interno eMule Next e fallito con codice $($process.ExitCode)."
    }

    Write-Host 'Primo avvio portable isolato: OK'
    Write-Host 'Hashing MD4/eD2K con vettori noti: OK'
    Write-Host 'Caricamento delle 43 lingue: OK'
    Write-Host 'Regole slot e upload illimitato: OK'
    Write-Host 'Chiusura pulita entro il limite: OK'
}
finally {
    if (Test-Path -LiteralPath $testRoot) {
        Remove-Item -LiteralPath $testRoot -Recurse -Force
    }
}
