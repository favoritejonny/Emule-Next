[CmdletBinding()]
param(
    [string]$OutputDirectory = ''
)

$ErrorActionPreference = 'Stop'
$projectRoot = [System.IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
if ([string]::IsNullOrWhiteSpace($OutputDirectory)) {
    $OutputDirectory = Join-Path $projectRoot 'dist\signpath-input'
}
$outputRoot = [System.IO.Path]::GetFullPath($OutputDirectory)
$projectPrefix = $projectRoot.TrimEnd('\') + '\'
if (-not $outputRoot.StartsWith($projectPrefix, [System.StringComparison]::OrdinalIgnoreCase)) {
    throw 'La cartella SignPath deve rimanere all interno del progetto.'
}
if (Test-Path -LiteralPath $outputRoot) {
    throw "La cartella SignPath esiste gia: $outputRoot"
}

$platforms = @(
    [pscustomobject]@{ Name = 'Win32'; Release = 'srchybrid\Win32\Release' },
    [pscustomobject]@{ Name = 'x64'; Release = 'srchybrid\x64\Release' }
)

New-Item -ItemType Directory -Path $outputRoot | Out-Null
foreach ($platform in $platforms) {
    $releaseDirectory = Join-Path $projectRoot $platform.Release
    $executable = Join-Path $releaseDirectory 'eMuleNext.exe'
    $languageDirectory = Join-Path $releaseDirectory 'lang'
    if (-not (Test-Path -LiteralPath $executable -PathType Leaf) -or
        -not (Test-Path -LiteralPath $languageDirectory -PathType Container)) {
        throw "Output Release mancante per $($platform.Name)."
    }

    $languages = @(Get-ChildItem -LiteralPath $languageDirectory -File -Filter '*.dll' |
        Where-Object { $_.Name -ne 'eMuleNext-GraphicsTest.dll' } |
        Sort-Object Name)
    if ($languages.Count -ne 43) {
        throw "Attese 43 lingue per $($platform.Name), trovate $($languages.Count)."
    }

    $destination = Join-Path $outputRoot $platform.Name
    $destinationLanguages = Join-Path $destination 'lang'
    New-Item -ItemType Directory -Path $destinationLanguages -Force | Out-Null
    Copy-Item -LiteralPath $executable -Destination (Join-Path $destination 'eMuleNext.exe')
    Copy-Item -LiteralPath $languages.FullName -Destination $destinationLanguages
}

$files = @(Get-ChildItem -LiteralPath $outputRoot -File -Recurse)
if ($files.Count -ne 88 -or
    @($files | Where-Object { $_.Extension -notin @('.exe', '.dll') }).Count -ne 0) {
    throw 'Il payload SignPath non contiene esattamente i 2 eseguibili e le 86 DLL previste.'
}

$referenceVersion = (Get-Item -LiteralPath (Join-Path $outputRoot 'x64\eMuleNext.exe')).VersionInfo
foreach ($file in $files) {
    if ($file.VersionInfo.ProductName -ne 'eMule Next' -or
        $file.VersionInfo.ProductVersion -ne $referenceVersion.ProductVersion -or
        $file.VersionInfo.FileVersion -ne $referenceVersion.FileVersion) {
        throw "Metadati non coerenti nel payload SignPath: $($file.FullName)"
    }
}

Write-Host "Payload SignPath pronto: $outputRoot"
Write-Host 'Contenuto: 2 eseguibili e 86 DLL linguistiche, nessun dato utente.'

