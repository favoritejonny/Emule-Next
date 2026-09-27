[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)]
    [string]$SignedDirectory
)

$ErrorActionPreference = 'Stop'
$projectRoot = [System.IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$signedRoot = [System.IO.Path]::GetFullPath($SignedDirectory)
$projectPrefix = $projectRoot.TrimEnd('\') + '\'
if (-not $signedRoot.StartsWith($projectPrefix, [System.StringComparison]::OrdinalIgnoreCase)) {
    throw 'La cartella firmata deve rimanere all interno del progetto.'
}

$platforms = @(
    [pscustomobject]@{ Name = 'Win32'; Release = 'srchybrid\Win32\Release' },
    [pscustomobject]@{ Name = 'x64'; Release = 'srchybrid\x64\Release' }
)
$verifiedFiles = New-Object System.Collections.Generic.List[System.IO.FileInfo]

foreach ($platform in $platforms) {
    $sourceDirectory = Join-Path $signedRoot $platform.Name
    $sourceExecutable = Join-Path $sourceDirectory 'eMuleNext.exe'
    $sourceLanguages = Join-Path $sourceDirectory 'lang'
    if (-not (Test-Path -LiteralPath $sourceExecutable -PathType Leaf) -or
        -not (Test-Path -LiteralPath $sourceLanguages -PathType Container)) {
        throw "Risultato SignPath incompleto per $($platform.Name)."
    }

    $languages = @(Get-ChildItem -LiteralPath $sourceLanguages -File -Filter '*.dll' |
        Sort-Object Name)
    if ($languages.Count -ne 43) {
        throw "Risultato SignPath: attese 43 lingue per $($platform.Name), trovate $($languages.Count)."
    }

    foreach ($file in @((Get-Item -LiteralPath $sourceExecutable)) + $languages) {
        $signature = Get-AuthenticodeSignature -LiteralPath $file.FullName
        if ($signature.Status.ToString() -ne 'Valid') {
            throw "Firma Authenticode non valida: $($file.FullName) - $($signature.Status)"
        }
        if ($null -eq $signature.SignerCertificate -or
            $signature.SignerCertificate.Subject -notmatch 'SignPath Foundation') {
            throw "Firmatario inatteso: $($file.FullName)"
        }
        if ($null -eq $signature.TimeStamperCertificate) {
            throw "Marca temporale assente: $($file.FullName)"
        }
        $verifiedFiles.Add($file)
    }
}

$allFiles = @(Get-ChildItem -LiteralPath $signedRoot -File -Recurse)
if ($allFiles.Count -ne 88 -or $verifiedFiles.Count -ne 88) {
    throw 'Il risultato SignPath contiene file mancanti o inattesi.'
}

foreach ($platform in $platforms) {
    $sourceDirectory = Join-Path $signedRoot $platform.Name
    $releaseDirectory = Join-Path $projectRoot $platform.Release
    Copy-Item -LiteralPath (Join-Path $sourceDirectory 'eMuleNext.exe') -Destination (Join-Path $releaseDirectory 'eMuleNext.exe') -Force
    Copy-Item -Path (Join-Path $sourceDirectory 'lang\*.dll') -Destination (Join-Path $releaseDirectory 'lang') -Force
}

Write-Host 'SignPath: 88 firme e marche temporali verificate.'
Write-Host 'I file Release temporanei del runner sono stati aggiornati con le copie firmate.'

