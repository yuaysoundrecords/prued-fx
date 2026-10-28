param(
    [string]$BuildDirectory = 'build/windows',
    [string]$InnoCompiler = 'C:/Program Files (x86)/Inno Setup 6/ISCC.exe'
)
$ErrorActionPreference = 'Stop'
$root = Split-Path $PSScriptRoot -Parent
$build = Join-Path $root $BuildDirectory
$plugin = Join-Path $build 'PruedFX_artefacts/Release/VST3/Prued FX.vst3'
if (-not (Test-Path -LiteralPath $InnoCompiler)) { throw 'Install Inno Setup 6 from its official source.' }
if (-not (Test-Path -LiteralPath $plugin)) { throw 'Run build-windows.ps1 successfully first.' }
$info = Get-Content -Raw -LiteralPath (Join-Path $plugin 'Contents/Resources/moduleinfo.json')
if ($info -notmatch '"Version"\s*:\s*"2\.0\.4"') { throw 'Installer requires the validated 2.0.4 binary.' }
$testLog = Get-Content -Raw -LiteralPath (Join-Path $build 'Testing/Temporary/LastTest.log')
if ($testLog.Contains('Test Failed.') -or ([regex]::Matches($testLog,'Test Passed.').Count -lt 7)) { throw 'All seven Windows tests must pass before packaging.' }
& $InnoCompiler "/DPluginSource=$plugin" "/DOutputPath=$(Join-Path $root 'delivery')" (Join-Path $root 'packaging/windows/PruedFX.iss')
if ($LASTEXITCODE -ne 0) { throw 'Installer build failed.' }
Get-FileHash -Algorithm SHA256 -LiteralPath (Join-Path $root 'delivery/Prued-FX-2.0.4-Windows-x64-Setup.exe')

