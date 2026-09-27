[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)] [string]$Version,
    [Parameter(Mandatory = $true)] [string]$OutputDirectory,
    [switch]$ExtractTestCopies,
    [int]$TimeoutSeconds = 90
)

$ErrorActionPreference = 'Stop'
$projectRoot = [System.IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$outputRoot = [System.IO.Path]::GetFullPath($OutputDirectory)
$projectPrefix = $projectRoot.TrimEnd('\') + '\'
if (-not $outputRoot.StartsWith($projectPrefix, [System.StringComparison]::OrdinalIgnoreCase)) {
    throw 'La cartella dei pacchetti deve trovarsi nel progetto.'
}

$checksumPath = Join-Path $outputRoot "SHA256SUMS-$Version.txt"
foreach ($line in Get-Content -LiteralPath $checksumPath) {
    if ($line -notmatch '^([0-9a-f]{64}) \*(.+)$') {
        throw "Riga checksum non valida: $line"
    }
    $actualHash = (Get-FileHash -LiteralPath (Join-Path $outputRoot $Matches[2]) -Algorithm SHA256).Hash.ToLowerInvariant()
    if ($actualHash -ne $Matches[1]) {
        throw "Checksum non corrispondente: $($Matches[2])"
    }
}

$packages = @(
    [pscustomobject]@{ Platform = 'Win32'; Label = 'win32' },
    [pscustomobject]@{ Platform = 'x64'; Label = 'x64' }
)

foreach ($package in $packages) {
    $baseName = "eMuleNext-$Version-$($package.Label)-portable"
    $archivePath = Join-Path $outputRoot "$baseName.zip"
    $manifestPath = Join-Path $outputRoot "$baseName.manifest.json"
    $sbomPath = Join-Path $outputRoot "$baseName.sbom.spdx.json"
    foreach ($requiredFile in @($archivePath, $manifestPath, $sbomPath)) {
        if (-not (Test-Path -LiteralPath $requiredFile -PathType Leaf)) {
            throw "Output mancante: $requiredFile"
        }
    }

    $verificationDirectory = [System.IO.Path]::GetFullPath(
        (Join-Path $outputRoot ('.verify-' + [guid]::NewGuid().ToString('N'))))
    $allowedVerificationPrefix = $outputRoot.TrimEnd('\') + '\.verify-'
    if (-not $verificationDirectory.StartsWith(
            $allowedVerificationPrefix, [System.StringComparison]::OrdinalIgnoreCase)) {
        throw "Cartella temporanea non valida: $verificationDirectory"
    }

    New-Item -ItemType Directory -Path $verificationDirectory | Out-Null
    try {
        Expand-Archive -LiteralPath $archivePath -DestinationPath $verificationDirectory
        $files = @(Get-ChildItem -LiteralPath $verificationDirectory -File -Recurse)
        $languages = @(Get-ChildItem -LiteralPath (Join-Path $verificationDirectory 'lang') -Filter '*.dll' -File)
        if ($languages.Count -ne 43) {
            throw "Numero di lingue errato per $($package.Platform): $($languages.Count)"
        }

        $verificationPrefix = $verificationDirectory.TrimEnd('\') + '\'
        $forbiddenFiles = @($files | Where-Object {
            $relativePath = $_.FullName.Substring($verificationPrefix.Length)
            $_.Name -match '^(preferences\.ini|known\.met|clients\.met)$|\.(dmp|log|pdb)$' -or
                $relativePath -match '(^|\\)config(\\|$)'
        })
        if ($forbiddenFiles.Count -ne 0) {
            throw "Il pacchetto $($package.Platform) contiene configurazioni o file temporanei."
        }

        $manifest = Get-Content -LiteralPath $manifestPath -Raw | ConvertFrom-Json
        if ($manifest.platform -ne $package.Platform -or
            $manifest.payloadFileCount -ne $manifest.files.Count) {
            throw "Manifest non coerente per $($package.Platform)."
        }
        foreach ($entry in $manifest.files) {
            $payloadPath = Join-Path $verificationDirectory $entry.path.Replace('/', '\')
            if (-not (Test-Path -LiteralPath $payloadPath -PathType Leaf)) {
                throw "File dichiarato ma assente: $($entry.path)"
            }
            $payloadHash = (Get-FileHash -LiteralPath $payloadPath -Algorithm SHA256).Hash.ToLowerInvariant()
            if ($payloadHash -ne $entry.sha256) {
                throw "Hash payload non valido: $($entry.path)"
            }
        }

        $internalManifest = Join-Path $verificationDirectory 'MANIFEST.json'
        $internalSbom = Join-Path $verificationDirectory 'SBOM.spdx.json'
        if ((Get-FileHash -LiteralPath $internalManifest -Algorithm SHA256).Hash -ne
                (Get-FileHash -LiteralPath $manifestPath -Algorithm SHA256).Hash -or
            (Get-FileHash -LiteralPath $internalSbom -Algorithm SHA256).Hash -ne
                (Get-FileHash -LiteralPath $sbomPath -Algorithm SHA256).Hash) {
            throw "Manifest o SBOM interni diversi dalle copie esterne per $($package.Platform)."
        }

        $testExecutable = Join-Path $verificationDirectory 'eMuleNext.exe'
        $process = Start-Process -FilePath $testExecutable -ArgumentList '-ci-self-test' `
            -WorkingDirectory $verificationDirectory -PassThru -WindowStyle Hidden
        if (-not $process.WaitForExit($TimeoutSeconds * 1000)) {
            $process.Kill()
            throw "Autotest scaduto per $($package.Platform)."
        }
        $process.Refresh()
        if ($process.ExitCode -ne 0) {
            $diagnosticPath = Join-Path $verificationDirectory 'ci-connection-layout.log'
            $detail = if (Test-Path -LiteralPath $diagnosticPath) {
                ' ' + (Get-Content -LiteralPath $diagnosticPath -Raw -Encoding Unicode).Trim()
            } else { '' }
            throw "Autotest fallito per $($package.Platform), codice $($process.ExitCode).$detail"
        }

        Write-Host "Verificato: $($package.Platform), $($files.Count) file, 43 lingue, $($manifest.payloadFileCount) elementi manifest."
    }
    finally {
        if ((Test-Path -LiteralPath $verificationDirectory) -and
            $verificationDirectory.StartsWith(
                $allowedVerificationPrefix, [System.StringComparison]::OrdinalIgnoreCase)) {
            Remove-Item -LiteralPath $verificationDirectory -Recurse -Force
        }
    }
}

if ($ExtractTestCopies) {
    foreach ($package in $packages) {
        $testDirectory = Join-Path $outputRoot $package.Platform
        if (Test-Path -LiteralPath $testDirectory) {
            throw "La cartella di prova esiste gia: $testDirectory"
        }
        $archivePath = Join-Path $outputRoot "eMuleNext-$Version-$($package.Label)-portable.zip"
        Expand-Archive -LiteralPath $archivePath -DestinationPath $testDirectory
        if (Test-Path -LiteralPath (Join-Path $testDirectory 'config')) {
            throw "La copia di prova $($package.Platform) non parte da zero."
        }
        Write-Host "Estratta copia pulita: $testDirectory"
    }
}

Write-Host 'Verifica completa dei pacchetti portatili: OK'
