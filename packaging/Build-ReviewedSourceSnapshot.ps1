[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)] [string]$Version,
    [Parameter(Mandatory = $true)] [string]$OutputDirectory
)

$ErrorActionPreference = 'Stop'
$projectRoot = [IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$outputRoot = [IO.Path]::GetFullPath($OutputDirectory)
if (-not $outputRoot.StartsWith($projectRoot.TrimEnd('\') + '\', [StringComparison]::OrdinalIgnoreCase)) {
    throw 'La destinazione deve trovarsi nel progetto.'
}
if ($Version -notmatch '^[A-Za-z0-9][A-Za-z0-9._-]*$') { throw 'Versione non valida.' }

# Snapshot of the working files, not git archive HEAD: the current test build
# includes reviewed modifications which have not been published or committed.
$tracked = @(git -C $projectRoot -c core.quotepath=false ls-files)
if ($LASTEXITCODE -ne 0 -or $tracked.Count -eq 0) { throw 'Elenco sorgenti Git non disponibile.' }
$untracked = @(git -C $projectRoot -c core.quotepath=false ls-files --others --exclude-standard)
if ($LASTEXITCODE -ne 0) { throw 'Elenco nuovi sorgenti non disponibile.' }
$reviewedNewFiles = @(
    '.github/workflows/signpath-release.yml', 'CODE_SIGNING_POLICY.md',
    'docs/CONNECTION-SETUP.md', 'docs/LANGUAGE-STARTUP.md', 'docs/PORT-MAPPING.md',
    'docs/PROVA-AVVIO-GUIDATO.md', 'docs/PROVA-CONNESSIONE.md', 'docs/SIGNPATH-SETUP.md',
    'packaging/Install-SignPathOutput.ps1', 'packaging/New-SignPathInput.ps1',
    'packaging/Verify-PortablePackages.ps1', 'packaging/Build-ReviewedSourceSnapshot.ps1',
    'srchybrid/ConnectionSetup.cpp', 'srchybrid/ConnectionSetup.h',
    'srchybrid/ConnectionSetupPolicy.h', 'srchybrid/ConnectionSpeedTest.cpp',
    'srchybrid/ConnectionSpeedTest.h', 'srchybrid/KadBootstrap.h',
    'srchybrid/LanguageSelection.h',
    'srchybrid/PortMappingProtocol.h', 'srchybrid/PortMappingTests.cpp',
    'srchybrid/PrivateDiagnostics.cpp', 'srchybrid/PrivateDiagnostics.h',
    'srchybrid/ServerBootstrap.h', 'srchybrid/UPnPImplPcpNatPmp.cpp',
    'srchybrid/UPnPImplPcpNatPmp.h', 'tests/PortMappingPolicyCases.h',
    'tests/PortMappingPolicyMain.cpp', 'tests/fixtures/saved-language.ini'
)
foreach ($relative in $untracked) {
    if ($relative -match '^tests/\.ci-' -or $relative -eq 'docs/media/sourceforge-preview-4x3-v1.png') { continue }
    if ($reviewedNewFiles -notcontains $relative) { throw "File nuovo da esaminare prima dell'invio: $relative" }
}
$files = @(@($tracked) + @($untracked | Where-Object { $reviewedNewFiles -contains $_ }) | Sort-Object -Unique)
$binaryPaths = @(
    (Join-Path $projectRoot 'srchybrid/Win32/Release/eMuleNext.exe'),
    (Join-Path $projectRoot 'srchybrid/x64/Release/eMuleNext.exe')
)
$earliestBinary = ($binaryPaths | ForEach-Object { (Get-Item -LiteralPath $_).LastWriteTimeUtc } | Sort-Object | Select-Object -First 1)
$inventory = [Collections.Generic.List[object]]::new()
foreach ($relative in $files) {
    if ($relative -match '(^|/)(\.git|\.codex|\.vs|config|Logs|Temp|Debug|Release|Dynamic)(/|$)' -or
        $relative -match '\.(dmp|pdb|obj|tlog|user|suo)$') {
        throw "File estraneo ai sorgenti: $relative"
    }
    $full = [IO.Path]::GetFullPath((Join-Path $projectRoot $relative))
    if (-not $full.StartsWith($projectRoot.TrimEnd('\') + '\', [StringComparison]::OrdinalIgnoreCase)) { throw 'Percorso sorgente non valido.' }
    $item = Get-Item -LiteralPath $full
    if ($item.PSIsContainer -or ($item.Attributes -band [IO.FileAttributes]::ReparsePoint)) { throw "Sorgente non regolare: $relative" }
    if ($relative -match '^(srchybrid|cryptopp|id3lib|mbedtls|miniupnpc|zlib)/' -and
        $relative -match '\.(cpp|c|h|hpp|rc|rc2|vcxproj|sln|props|targets|ico|bmp|gif|png|manifest|asm)$' -and
        $item.LastWriteTimeUtc -gt $earliestBinary) {
        throw "Sorgente modificato dopo la compilazione: $relative"
    }
    $inventory.Add([pscustomobject]@{ path = $relative; size = $item.Length; sha256 = (Get-FileHash -LiteralPath $full -Algorithm SHA256).Hash.ToLowerInvariant() })
}

New-Item -ItemType Directory -Path $outputRoot -Force | Out-Null
$archivePath = Join-Path $outputRoot "eMuleNext-$Version-sources.zip"
$hashPath = Join-Path $outputRoot "SHA256-SOURCES-$Version.txt"
if ((Test-Path -LiteralPath $archivePath) -or (Test-Path -LiteralPath $hashPath)) { throw 'Archivio o checksum gia esistente: non sovrascrivo.' }
$revision = (git -C $projectRoot rev-parse HEAD).Trim()
if ($LASTEXITCODE -ne 0) { throw 'Revisione Git non disponibile.' }
$snapshot = [ordered]@{
    version = $Version
    baseRevision = $revision
    sourceTreeState = 'working-tree-snapshot'
    note = 'Includes current reviewed working files and dependency sources; not an archive of the base revision alone. Do not substitute the older public alpha source tag.'
    files = $inventory.ToArray()
}
Add-Type -AssemblyName System.IO.Compression
$outputStream = [IO.File]::Open($archivePath, [IO.FileMode]::CreateNew, [IO.FileAccess]::Write, [IO.FileShare]::None)
try {
    $archive = [IO.Compression.ZipArchive]::new($outputStream, [IO.Compression.ZipArchiveMode]::Create, $true)
    try {
        foreach ($file in $inventory) {
            $entry = $archive.CreateEntry($file.path, [IO.Compression.CompressionLevel]::Optimal)
            $inputStream = [IO.File]::OpenRead((Join-Path $projectRoot $file.path))
            try { $entryStream = $entry.Open(); try { $inputStream.CopyTo($entryStream) } finally { $entryStream.Dispose() } }
            finally { $inputStream.Dispose() }
        }
        $entry = $archive.CreateEntry('SOURCE-SNAPSHOT.json', [IO.Compression.CompressionLevel]::Optimal)
        $writer = [IO.StreamWriter]::new($entry.Open(), [Text.UTF8Encoding]::new($false))
        try { $writer.Write(($snapshot | ConvertTo-Json -Depth 6)) } finally { $writer.Dispose() }
    } finally { $archive.Dispose() }
} finally { $outputStream.Dispose() }

# Read and hash EVERY archived file; a successful ZIP creation is not enough.
$inputArchive = [IO.Compression.ZipFile]::OpenRead($archivePath)
try {
    if ($inputArchive.Entries.Count -ne $inventory.Count + 1) { throw 'Numero file ZIP non valido.' }
    foreach ($file in $inventory) {
        $entry = $inputArchive.GetEntry($file.path)
        if ($null -eq $entry -or $entry.Length -ne $file.size) { throw "File ZIP mancante o troncato: $($file.path)" }
        $stream = $entry.Open(); $sha = [Security.Cryptography.SHA256]::Create()
        try { $actual = [Convert]::ToHexString($sha.ComputeHash($stream)).ToLowerInvariant() }
        finally { $sha.Dispose(); $stream.Dispose() }
        if ($actual -ne $file.sha256) { throw "Hash sorgente non corrispondente: $($file.path)" }
    }
} finally { $inputArchive.Dispose() }
$archiveHash = (Get-FileHash -LiteralPath $archivePath -Algorithm SHA256).Hash.ToLowerInvariant()
[IO.File]::WriteAllText($hashPath, "$archiveHash *$([IO.Path]::GetFileName($archivePath))`n", [Text.UTF8Encoding]::new($false))
Write-Host "Sorgenti verificati: $($inventory.Count) file e manifest; $((Get-Item -LiteralPath $archivePath).Length) byte."
Write-Host "SHA-256: $archiveHash"
Write-Host "Archivio: $archivePath"
