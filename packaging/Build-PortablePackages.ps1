[CmdletBinding()]
param(
    [ValidateSet('All', 'Win32', 'x64')]
    [string]$Platform = 'All',
    [string]$Version = '1.0.0-alpha.1',
    [string]$OutputDirectory = ''
)

$ErrorActionPreference = 'Stop'

$projectRoot = [System.IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
if ([string]::IsNullOrWhiteSpace($OutputDirectory)) {
    $OutputDirectory = Join-Path $projectRoot 'dist\portable'
}
$outputRoot = [System.IO.Path]::GetFullPath($OutputDirectory)
$projectRootWithSeparator = $projectRoot.TrimEnd('\') + '\'
if (-not $outputRoot.StartsWith($projectRootWithSeparator, [System.StringComparison]::OrdinalIgnoreCase)) {
    throw "La cartella di output deve rimanere all'interno della cartella del progetto."
}

New-Item -ItemType Directory -Path $outputRoot -Force | Out-Null

$portableMarker = Join-Path $PSScriptRoot 'portable\eMuleNext.portable'
$portableReadme = Join-Path $PSScriptRoot 'PORTABLE.md'
$legalFiles = @(
    [pscustomobject]@{ Source = (Join-Path $projectRoot 'LICENSE'); Destination = 'LICENSE' },
    [pscustomobject]@{ Source = (Join-Path $projectRoot 'NOTICE.md'); Destination = 'NOTICE.md' },
    [pscustomobject]@{ Source = (Join-Path $projectRoot 'THIRD_PARTY_NOTICES.md'); Destination = 'THIRD_PARTY_NOTICES.md' },
    [pscustomobject]@{ Source = (Join-Path $projectRoot 'CHANGES.md'); Destination = 'CHANGES.md' },
    [pscustomobject]@{ Source = (Join-Path $projectRoot 'PRIVACY.md'); Destination = 'PRIVACY.md' },
    [pscustomobject]@{ Source = (Join-Path $projectRoot 'SOURCE-CODE.md'); Destination = 'SOURCE-CODE.md' },
    [pscustomobject]@{ Source = (Join-Path $projectRoot 'WINDOWS-SMARTSCREEN.md'); Destination = 'WINDOWS-SMARTSCREEN.md' },
    [pscustomobject]@{ Source = (Join-Path $projectRoot 'cryptopp\License.txt'); Destination = 'licenses\CryptoPP-Boost-1.0.txt' },
    [pscustomobject]@{ Source = (Join-Path $projectRoot 'id3lib\COPYING'); Destination = 'licenses\id3lib-LGPL-2.0.txt' },
    [pscustomobject]@{ Source = (Join-Path $projectRoot 'mbedtls\LICENSE'); Destination = 'licenses\MbedTLS-dual-license.txt' },
    [pscustomobject]@{ Source = (Join-Path $projectRoot 'miniupnpc\LICENSE'); Destination = 'licenses\miniupnpc-BSD-3-Clause.txt' },
    [pscustomobject]@{ Source = (Join-Path $projectRoot 'zlib\LICENSE'); Destination = 'licenses\zlib.txt' }
)
foreach ($requiredFile in @($portableMarker, $portableReadme) + $legalFiles.Source) {
    if (-not (Test-Path -LiteralPath $requiredFile -PathType Leaf)) {
        throw "File richiesto non trovato: $requiredFile"
    }
}

$packages = @(
    [pscustomobject]@{ Platform = 'Win32'; Label = 'win32'; ReleaseDirectory = 'srchybrid\Win32\Release' },
    [pscustomobject]@{ Platform = 'x64';   Label = 'x64';   ReleaseDirectory = 'srchybrid\x64\Release' }
)
if ($Platform -ne 'All') {
    $packages = $packages | Where-Object { $_.Platform -eq $Platform }
}

$hashLines = New-Object System.Collections.Generic.List[string]
foreach ($package in $packages) {
    $releaseDirectory = Join-Path $projectRoot $package.ReleaseDirectory
    $executable = Join-Path $releaseDirectory 'eMuleNext.exe'
    $languageDirectory = Join-Path $releaseDirectory 'lang'
    $documentationDirectory = Join-Path $releaseDirectory 'docs'

    foreach ($requiredPath in @($executable, $languageDirectory, $documentationDirectory)) {
        if (-not (Test-Path -LiteralPath $requiredPath)) {
            throw "Output Release mancante per $($package.Platform): $requiredPath"
        }
    }
    $languageFiles = @(Get-ChildItem -LiteralPath $languageDirectory -File -Filter '*.dll' |
        Where-Object { $_.Name -ne 'eMuleNext-GraphicsTest.dll' })
    if ($languageFiles.Count -eq 0) {
        throw "Nessuna lingua distribuibile trovata per $($package.Platform)."
    }

    $archiveName = "eMuleNext-$Version-$($package.Label)-portable.zip"
    $archivePath = Join-Path $outputRoot $archiveName
    if (Test-Path -LiteralPath $archivePath) {
        throw "Archivio gia presente: $archivePath. Scegli una nuova versione oppure spostalo prima di ricreare il pacchetto."
    }

    $stageDirectory = Join-Path $outputRoot ('.stage-' + [guid]::NewGuid().ToString('N'))
    New-Item -ItemType Directory -Path $stageDirectory | Out-Null
    try {
        Copy-Item -LiteralPath $executable -Destination (Join-Path $stageDirectory 'eMuleNext.exe')
        Copy-Item -LiteralPath $portableMarker -Destination (Join-Path $stageDirectory 'eMuleNext.portable')
        Copy-Item -LiteralPath $portableReadme -Destination (Join-Path $stageDirectory 'README-PORTABLE.md')
        foreach ($legalFile in $legalFiles) {
            $destination = Join-Path $stageDirectory $legalFile.Destination
            $destinationDirectory = Split-Path -Parent $destination
            if (-not (Test-Path -LiteralPath $destinationDirectory)) {
                New-Item -ItemType Directory -Path $destinationDirectory -Force | Out-Null
            }
            Copy-Item -LiteralPath $legalFile.Source -Destination $destination
        }
        $stageLanguageDirectory = Join-Path $stageDirectory 'lang'
        New-Item -ItemType Directory -Path $stageLanguageDirectory | Out-Null
        Copy-Item -LiteralPath $languageFiles.FullName -Destination $stageLanguageDirectory
        Copy-Item -LiteralPath $documentationDirectory -Destination (Join-Path $stageDirectory 'docs') -Recurse

        Compress-Archive -Path (Join-Path $stageDirectory '*') -DestinationPath $archivePath
        $hash = (Get-FileHash -LiteralPath $archivePath -Algorithm SHA256).Hash.ToLowerInvariant()
        $hashLines.Add("$hash *$archiveName")
        Write-Host "Creato: $archivePath"
    }
    finally {
        if (Test-Path -LiteralPath $stageDirectory) {
            Remove-Item -LiteralPath $stageDirectory -Recurse -Force
        }
    }
}

$hashFile = Join-Path $outputRoot "SHA256SUMS-$Version.txt"
if (Test-Path -LiteralPath $hashFile) {
    throw "File di checksum gia presente: $hashFile. Scegli una nuova versione oppure spostalo prima di ricreare i pacchetti."
}
[System.IO.File]::WriteAllLines($hashFile, [string[]]$hashLines, [System.Text.UTF8Encoding]::new($false))
Write-Host "Checksum SHA-256: $hashFile"
