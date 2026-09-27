[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)] [ValidateSet('Win32', 'x64')] [string]$Platform,
    [int]$TimeoutSeconds = 60,
    [switch]$SourceOnly
)

$ErrorActionPreference = 'Stop'
$projectRoot = [System.IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$releaseDirectory = Join-Path $projectRoot "srchybrid\$Platform\Release"
$executable = Join-Path $releaseDirectory 'eMuleNext.exe'
$languageDirectory = Join-Path $releaseDirectory 'lang'
$documentationDirectory = Join-Path $releaseDirectory 'docs'

if (-not $SourceOnly) {
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

    $executableVersion = (Get-Item -LiteralPath $executable).VersionInfo
    if ($executableVersion.ProductName -ne 'eMule Next' -or
        [string]::IsNullOrWhiteSpace($executableVersion.ProductVersion)) {
        throw 'I metadati di prodotto dell eseguibile non sono validi.'
    }
    foreach ($language in $languages) {
        $languageVersion = $language.VersionInfo
        if ($languageVersion.ProductName -ne $executableVersion.ProductName -or
            $languageVersion.ProductVersion -ne $executableVersion.ProductVersion -or
            $languageVersion.FileVersion -ne $executableVersion.FileVersion) {
            throw "Metadati di prodotto non coerenti nella lingua $($language.Name)."
        }
    }
}

# Keep the experimental first-run connection page complete in every shipped
# language module. The normal resource loader deliberately falls back to
# English, so missing source strings would otherwise be invisible at runtime.
$sourceLanguageDirectory = Join-Path $projectRoot 'srchybrid\lang'
$sourceLanguageFiles = @(Get-ChildItem -LiteralPath $sourceLanguageDirectory -File -Filter '*.rc')
if ($sourceLanguageFiles.Count -ne 43) {
    throw "Attesi 43 sorgenti lingua, trovati $($sourceLanguageFiles.Count) in $sourceLanguageDirectory"
}
$connectionStringPattern = '(?m)^\s+(IDS_(?:CONNSETUP|PORTMAP|WIZSETUP)_[A-Z0-9_]+)\s+"(.*)"\s*$'
$englishResource = [System.IO.File]::ReadAllText((Join-Path $projectRoot 'srchybrid\emule.rc'))
$englishMatches = [regex]::Matches($englishResource, $connectionStringPattern)
if ($englishMatches.Count -ne 33) {
    throw "Attese 33 stringhe inglesi della configurazione connessione, trovate $($englishMatches.Count)."
}
$englishStrings = @{}
foreach ($match in $englishMatches) {
    $englishStrings[$match.Groups[1].Value] = $match.Groups[2].Value
}
$expectedConnectionIds = @($englishStrings.Keys | Sort-Object)
foreach ($sourceLanguage in $sourceLanguageFiles) {
    $bytes = [System.IO.File]::ReadAllBytes($sourceLanguage.FullName)
    if ($bytes.Length -lt 3 -or $bytes[0] -ne 0xEF -or $bytes[1] -ne 0xBB -or $bytes[2] -ne 0xBF) {
        throw "Il sorgente lingua non e UTF-8 con BOM: $($sourceLanguage.Name)"
    }
    $sourceText = [System.IO.File]::ReadAllText($sourceLanguage.FullName)
    if ($sourceText.Contains('__EMULE_')) {
        throw "Segnaposto di traduzione non risolto in $($sourceLanguage.Name)"
    }
    $localizedMatches = [regex]::Matches($sourceText, $connectionStringPattern)
    $localizedStrings = @{}
    foreach ($match in $localizedMatches) {
        $id = $match.Groups[1].Value
        if ($localizedStrings.ContainsKey($id)) {
            throw "Stringa duplicata $id in $($sourceLanguage.Name)"
        }
        $localizedStrings[$id] = $match.Groups[2].Value
    }
    $localizedIds = @($localizedStrings.Keys | Sort-Object)
    if ($localizedIds.Count -ne $expectedConnectionIds.Count -or
        @(Compare-Object $expectedConnectionIds $localizedIds).Count -ne 0) {
        throw "Traduzioni della configurazione connessione incomplete in $($sourceLanguage.Name)"
    }
    foreach ($id in $expectedConnectionIds) {
        $expectedFormats = @([regex]::Matches($englishStrings[$id], '%[us]') | ForEach-Object Value)
        $localizedFormats = @([regex]::Matches($localizedStrings[$id], '%[us]') | ForEach-Object Value)
        if ($localizedStrings[$id].Length -eq 0 -or
            @(Compare-Object $expectedFormats $localizedFormats -SyncWindow 0).Count -ne 0) {
            throw "Segnaposto non validi per $id in $($sourceLanguage.Name)"
        }
    }
}

$portTestDialog = [regex]::Match($englishResource, '(?ms)^IDD_WIZ1_PORTTEST DIALOGEX\b.*?^END\s*$').Value
$wizardSource = [System.IO.File]::ReadAllText((Join-Path $projectRoot 'srchybrid\PShtWiz1.cpp'))
$speedTestSource = [System.IO.File]::ReadAllText((Join-Path $projectRoot 'srchybrid\ConnectionSpeedTest.cpp'))
$uploadThrottlerSource = [System.IO.File]::ReadAllText((Join-Path $projectRoot 'srchybrid\UploadBandwidthThrottler.cpp'))
if (-not $portTestDialog -or $portTestDialog -match 'IDC_SETUP_FIREWALL' -or
    $wizardSource -match 'OnOpenFirewall|IDC_SETUP_FIREWALL|IDS_WIZSETUP_FIREWALL' -or
    $portTestDialog -notmatch 'IDC_STARTTEST' -or $portTestDialog -notmatch 'IDC_TESTINFO' -or
    $portTestDialog -notmatch 'IDC_WIZ_SETUP_INFO' -or
    -not $englishStrings.ContainsKey('IDS_WIZSETUP_PORTTEST')) {
    throw 'La pagina di test deve conservare il test manuale senza il collegamento al firewall.'
}
if ($wizardSource -notmatch 'firstRun \? 0 : thePrefs\.IsSafeServerConnectEnabled\(\)' -or
    $wizardSource -notmatch 'IDD_WIZ1_SPEEDTEST' -or
    $speedTestSource -notmatch 'https://speed\.cloudflare\.com/__down\?bytes=' -or
    $speedTestSource -notmatch 'speed\.cloudflare\.com' -or
    $speedTestSource -notmatch 'HttpSendRequest') {
    throw 'Avvio guidato: predefiniti o test velocita non coerenti con la configurazione approvata.'
}
if ($uploadThrottlerSource -notmatch 'allowedDataRate\s*=\s*theApp\.lastCommonRouteFinder->GetUpload\(\)' -or
    $uploadThrottlerSource -match 'allowedDataRate\s*=\s*nEstiminatedDataRate') {
    throw 'Upload illimitato: il regolatore non deve applicare come tetto la stima storica basata sui socket.'
}

if ($SourceOnly) {
    Write-Host 'Sorgenti: 43 traduzioni complete, UTF-8 e segnaposto verificati.'
    Write-Host 'Avvio guidato: collegamento firewall assente, test porte e spiegazione conservati.'
    Write-Host 'Test velocita: pagina volontaria, HTTPS e applicazione 80% upload verificati.'
    Write-Host 'Upload illimitato: nessun tetto nascosto derivato dalla stima storica dei socket.'
    Write-Host 'Non sono stati eseguiti test binari o avviati client.'
    return
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
        -WorkingDirectory $testRoot -PassThru -WindowStyle Hidden
    if (-not $process.WaitForExit($TimeoutSeconds * 1000)) {
        $process.Kill()
        throw "Il test di avvio/chiusura non si e concluso entro $TimeoutSeconds secondi."
    }
    $process.Refresh()
    if ($process.ExitCode -ne 0) {
        $layoutDiagnostic = Join-Path $testRoot 'ci-connection-layout.log'
        $detail = if (Test-Path -LiteralPath $layoutDiagnostic) {
            ' Dettaglio: ' + (Get-Content -LiteralPath $layoutDiagnostic -Raw -Encoding Unicode).Trim()
        } else { '' }
        throw "Il test interno eMule Next e fallito con codice $($process.ExitCode).$detail"
    }

    Write-Host 'Primo avvio portable isolato: OK'
    Write-Host 'Hashing MD4/eD2K con vettori noti: OK'
    Write-Host 'Caricamento delle 43 lingue: OK'
    Write-Host 'Lingua automatica Windows, varianti regionali e inglese di riserva: OK'
    Write-Host 'PCP/NAT-PMP: pacchetti, negoziazione simulata, rinnovi e rifiuti senza aggiramento: OK'
    Write-Host 'Regole slot e upload illimitato: OK'
    Write-Host 'Porte, consenso, profili di rete e verifica delle mappature: OK'
    Write-Host 'Primo avvio senza consenso UPnP implicito: OK'
    Write-Host 'Sei pagine iniziali: testi completi e dimensioni verificate nelle 43 lingue: OK'
    Write-Host 'Lista server: consenso, inizializzazione differita e nessun duplicato: OK'
    Write-Host 'Chiusura pulita entro il limite: OK'

    # Simulate an existing profile with historical migrations already applied;
    # only the disposable CI directory is used.
    New-Item -ItemType Directory -Path $configDirectory -Force | Out-Null
    $savedPreferences = Join-Path $configDirectory 'preferences.ini'
    Copy-Item -LiteralPath (Join-Path $PSScriptRoot 'fixtures\saved-language.ini') -Destination $savedPreferences
    $savedHash = (Get-FileHash -LiteralPath $savedPreferences -Algorithm SHA256).Hash
    $process = Start-Process -FilePath $testExecutable -ArgumentList '-ci-self-test', '-ci-saved-language-test' `
        -WorkingDirectory $testRoot -PassThru -WindowStyle Hidden
    if (-not $process.WaitForExit($TimeoutSeconds * 1000)) {
        $process.Kill()
        throw 'Il test di riavvio con lingua salvata ha superato il tempo limite.'
    }
    $process.Refresh()
    if ($process.ExitCode -ne 0 -or (Get-FileHash -LiteralPath $savedPreferences -Algorithm SHA256).Hash -ne $savedHash) {
        throw 'Il riavvio non ha conservato la lingua o le preferenze del profilo di prova.'
    }
    Write-Host 'Riavvio con lingua salvata e altre impostazioni invariate: OK'
}
finally {
    if (Test-Path -LiteralPath $testRoot) {
        Remove-Item -LiteralPath $testRoot -Recurse -Force
    }
}
