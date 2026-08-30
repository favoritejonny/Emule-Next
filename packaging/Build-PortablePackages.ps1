[CmdletBinding()]
param(
    [ValidateSet('All', 'Win32', 'x64')]
    [string]$Platform = 'All',
    [string]$Version = '1.0.0-alpha.1',
    [string]$OutputDirectory = ''
)

$ErrorActionPreference = 'Stop'
. (Join-Path $PSScriptRoot 'PackageMetadata.ps1')

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
$hashFile = Join-Path $outputRoot "SHA256SUMS-$Version.txt"
if (Test-Path -LiteralPath $hashFile) {
    throw "File di checksum gia presente: $hashFile. Scegli una nuova versione oppure spostalo prima di ricreare i pacchetti."
}

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
    if ($languageFiles.Count -ne 43) {
        throw "Pacchetto incompleto per $($package.Platform): attese 43 lingue, trovate $($languageFiles.Count)."
    }

    $archiveName = "eMuleNext-$Version-$($package.Label)-portable.zip"
    $archivePath = Join-Path $outputRoot $archiveName
    $metadataBaseName = "eMuleNext-$Version-$($package.Label)-portable"
    $manifestPath = Join-Path $outputRoot "$metadataBaseName.manifest.json"
    $sbomPath = Join-Path $outputRoot "$metadataBaseName.sbom.spdx.json"
    foreach ($newOutput in @($archivePath, $manifestPath, $sbomPath)) {
        if (Test-Path -LiteralPath $newOutput) {
            throw "Output gia presente: $newOutput. Scegli una nuova versione oppure spostalo prima di ricreare il pacchetto."
        }
    }

    $stageDirectory = [System.IO.Path]::GetFullPath(
        (Join-Path $outputRoot ('.stage-' + [guid]::NewGuid().ToString('N'))))
    $allowedStagePrefix = $outputRoot.TrimEnd('\') + '\.stage-'
    if (-not $stageDirectory.StartsWith($allowedStagePrefix, [System.StringComparison]::OrdinalIgnoreCase)) {
        throw "Cartella temporanea non valida: $stageDirectory"
    }
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

        New-EmuleNextPackageMetadata -StageDirectory $stageDirectory -ProjectRoot $projectRoot `
            -Version $Version -Platform $package.Platform -ArchiveName $archiveName `
            -ManifestPath $manifestPath -SbomPath $sbomPath
        $generatedManifest = Get-Content -LiteralPath $manifestPath -Raw | ConvertFrom-Json
        $generatedSbom = Get-Content -LiteralPath $sbomPath -Raw | ConvertFrom-Json
        if ($generatedManifest.payloadFileCount -ne $generatedManifest.files.Count -or
            $generatedManifest.files.Count -eq 0) {
            throw 'Manifest non valido: elenco dei file mancante o incompleto.'
        }
        if ($generatedSbom.spdxVersion -ne 'SPDX-2.3' -or $generatedSbom.files.Count -ne $generatedManifest.files.Count) {
            throw 'SBOM non valida: formato SPDX o elenco dei file non coerente.'
        }
        Copy-Item -LiteralPath $manifestPath -Destination (Join-Path $stageDirectory 'MANIFEST.json')
        Copy-Item -LiteralPath $sbomPath -Destination (Join-Path $stageDirectory 'SBOM.spdx.json')

        Compress-Archive -Path (Join-Path $stageDirectory '*') -DestinationPath $archivePath
        Add-Type -AssemblyName System.IO.Compression.FileSystem
        $archive = [System.IO.Compression.ZipFile]::OpenRead($archivePath)
        try {
            $entryNames = @($archive.Entries | ForEach-Object { $_.FullName.Replace('\', '/') })
            foreach ($requiredEntry in @('eMuleNext.exe', 'eMuleNext.portable', 'MANIFEST.json', 'SBOM.spdx.json')) {
                if ($entryNames -notcontains $requiredEntry) {
                    throw "Archivio non valido: manca $requiredEntry"
                }
            }
            if (@($entryNames | Where-Object { $_ -like 'lang/*.dll' }).Count -ne $languageFiles.Count) {
                throw "Archivio non valido: il numero delle lingue non corrisponde."
            }
        }
        finally {
            $archive.Dispose()
        }

        foreach ($outputFile in @($archivePath, $manifestPath, $sbomPath)) {
            $hash = (Get-FileHash -LiteralPath $outputFile -Algorithm SHA256).Hash.ToLowerInvariant()
            $hashLines.Add("$hash *$(Split-Path -Leaf $outputFile)")
        }
        Write-Host "Creato: $archivePath"
    }
    finally {
        if (Test-Path -LiteralPath $stageDirectory) {
            Remove-Item -LiteralPath $stageDirectory -Recurse -Force
        }
    }
}

[System.IO.File]::WriteAllLines($hashFile, [string[]]$hashLines, [System.Text.UTF8Encoding]::new($false))
Write-Host "Checksum SHA-256: $hashFile"
