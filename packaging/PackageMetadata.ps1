Set-StrictMode -Version 2.0

function Write-JsonFile {
    param(
        [Parameter(Mandatory = $true)] [object]$Value,
        [Parameter(Mandatory = $true)] [string]$Path,
        [int]$Depth = 12
    )

    $json = $Value | ConvertTo-Json -Depth $Depth
    [System.IO.File]::WriteAllText($Path, $json + [Environment]::NewLine, [System.Text.UTF8Encoding]::new($false))
}

function Get-SourceRevision {
    param([Parameter(Mandatory = $true)] [string]$ProjectRoot)

    if (-not [string]::IsNullOrWhiteSpace($env:GITHUB_SHA)) {
        return $env:GITHUB_SHA
    }

    try {
        $revision = (& git -C $ProjectRoot rev-parse HEAD 2>$null | Select-Object -First 1)
        if (-not [string]::IsNullOrWhiteSpace($revision)) {
            return $revision.Trim()
        }
    }
    catch {
    }
    return 'unknown'
}

function Get-SourceTreeState {
    param([Parameter(Mandatory = $true)] [string]$ProjectRoot)

    try {
        $statusOutput = @(& git -C $ProjectRoot status --porcelain --untracked-files=normal 2>$null)
        $gitExitCode = $LASTEXITCODE
        if ($gitExitCode -ne 0) {
            return 'unknown'
        }
        if ($statusOutput.Count -eq 0) {
            return 'clean'
        }
        return 'dirty'
    }
    catch {
        return 'unknown'
    }
}

function New-EmuleNextPackageMetadata {
    [CmdletBinding()]
    param(
        [Parameter(Mandatory = $true)] [string]$StageDirectory,
        [Parameter(Mandatory = $true)] [string]$ProjectRoot,
        [Parameter(Mandatory = $true)] [string]$Version,
        [Parameter(Mandatory = $true)] [ValidateSet('Win32', 'x64')] [string]$Platform,
        [Parameter(Mandatory = $true)] [string]$ArchiveName,
        [Parameter(Mandatory = $true)] [string]$ManifestPath,
        [Parameter(Mandatory = $true)] [string]$SbomPath
    )

    $stageRoot = [System.IO.Path]::GetFullPath($StageDirectory).TrimEnd('\') + '\'
    $sourceRevision = Get-SourceRevision -ProjectRoot $ProjectRoot
    $sourceTreeState = Get-SourceTreeState -ProjectRoot $ProjectRoot
    $created = [DateTime]::UtcNow.ToString('yyyy-MM-ddTHH:mm:ssZ')
    $architecture = if ($Platform -eq 'Win32') { 'x86' } else { 'x86_64' }

    $payload = @(
        Get-ChildItem -LiteralPath $StageDirectory -File -Recurse |
            Sort-Object FullName |
            ForEach-Object {
                [pscustomobject]@{
                    path = $_.FullName.Substring($stageRoot.Length).Replace('\', '/')
                    size = $_.Length
                    sha256 = (Get-FileHash -LiteralPath $_.FullName -Algorithm SHA256).Hash.ToLowerInvariant()
                }
            }
    )

    $dependencies = @(
        [pscustomobject]@{ name = 'Crypto++';       version = '5.6.3'; license = 'BSL-1.0' },
        [pscustomobject]@{ name = 'id3lib';         version = '3.8.3'; license = 'LGPL-2.0-only' },
        [pscustomobject]@{ name = 'Mbed TLS';       version = '4.2.0'; license = 'GPL-2.0-or-later' },
        [pscustomobject]@{ name = 'MiniUPnPc';      version = '2.3.3'; license = 'BSD-3-Clause' },
        [pscustomobject]@{ name = 'NextResizable';  version = 'in-tree'; license = 'GPL-2.0-or-later' },
        [pscustomobject]@{ name = 'zlib';            version = '1.3.2'; license = 'Zlib' }
    )

    $manifest = [ordered]@{
        schemaVersion = 1
        product = 'eMule Next'
        version = $Version
        platform = $Platform
        architecture = $architecture
        packageType = 'portable-zip'
        archiveName = $ArchiveName
        sourceRepository = 'https://github.com/favoritejonny/Emule-Next'
        sourceRevision = $sourceRevision
        sourceTreeState = $sourceTreeState
        generatedAtUtc = $created
        payloadFileCount = $payload.Count
        files = $payload
        dependencies = $dependencies
    }
    Write-JsonFile -Value $manifest -Path $ManifestPath

    $spdxFiles = New-Object System.Collections.Generic.List[object]
    $relationships = New-Object System.Collections.Generic.List[object]
    $fileIds = New-Object System.Collections.Generic.List[string]
    for ($index = 0; $index -lt $payload.Count; ++$index) {
        $fileId = 'SPDXRef-File-{0:D4}' -f ($index + 1)
        $fileIds.Add($fileId)
        $spdxFiles.Add([ordered]@{
            fileName = './' + $payload[$index].path
            SPDXID = $fileId
            checksums = @([ordered]@{ algorithm = 'SHA256'; checksumValue = $payload[$index].sha256 })
            licenseConcluded = 'NOASSERTION'
            copyrightText = 'NOASSERTION'
        })
        $relationships.Add([ordered]@{
            spdxElementId = 'SPDXRef-Package-eMule-Next'
            relationshipType = 'CONTAINS'
            relatedSpdxElement = $fileId
        })
    }

    $spdxPackages = New-Object System.Collections.Generic.List[object]
    $spdxPackages.Add([ordered]@{
        name = 'eMule Next'
        SPDXID = 'SPDXRef-Package-eMule-Next'
        versionInfo = $Version
        downloadLocation = 'https://github.com/favoritejonny/Emule-Next'
        filesAnalyzed = $true
        licenseConcluded = 'GPL-2.0-or-later'
        licenseDeclared = 'GPL-2.0-or-later'
        copyrightText = 'NOASSERTION'
        primaryPackagePurpose = 'APPLICATION'
        hasFiles = [string[]]$fileIds
    })
    for ($index = 0; $index -lt $dependencies.Count; ++$index) {
        $dependency = $dependencies[$index]
        $dependencyId = 'SPDXRef-Dependency-{0:D2}' -f ($index + 1)
        $spdxPackages.Add([ordered]@{
            name = $dependency.name
            SPDXID = $dependencyId
            versionInfo = $dependency.version
            downloadLocation = 'NOASSERTION'
            filesAnalyzed = $false
            licenseConcluded = $dependency.license
            licenseDeclared = $dependency.license
            copyrightText = 'NOASSERTION'
            primaryPackagePurpose = 'LIBRARY'
        })
        $relationships.Add([ordered]@{
            spdxElementId = 'SPDXRef-Package-eMule-Next'
            relationshipType = 'DEPENDS_ON'
            relatedSpdxElement = $dependencyId
        })
    }

    $relationships.Insert(0, [ordered]@{
        spdxElementId = 'SPDXRef-DOCUMENT'
        relationshipType = 'DESCRIBES'
        relatedSpdxElement = 'SPDXRef-Package-eMule-Next'
    })

    $namespaceSuffix = [Uri]::EscapeDataString(
        "$Version-$Platform-$sourceRevision-$([guid]::NewGuid().ToString('N'))")
    $sbom = [ordered]@{
        spdxVersion = 'SPDX-2.3'
        dataLicense = 'CC0-1.0'
        SPDXID = 'SPDXRef-DOCUMENT'
        name = "eMuleNext-$Version-$Platform"
        documentNamespace = "https://github.com/favoritejonny/Emule-Next/sbom/$namespaceSuffix"
        creationInfo = [ordered]@{
            created = $created
            creators = @('Organization: eMule Next contributors', 'Tool: eMule Next PackageMetadata.ps1')
        }
        packages = $spdxPackages
        files = $spdxFiles
        relationships = $relationships
    }
    Write-JsonFile -Value $sbom -Path $SbomPath
}
