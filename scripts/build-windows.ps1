param(
    [string]$JucePath = 'D:/JUICED/JUCE',
    [string]$BuildDirectory = 'build/windows',
    [ValidateSet('Debug','Release')][string]$Configuration = 'Release'
)
$ErrorActionPreference = 'Stop'
$projectRoot = Split-Path $PSScriptRoot -Parent
$vswhere = 'C:/Program Files (x86)/Microsoft Visual Studio/Installer/vswhere.exe'
$vsPath = & $vswhere -latest -products '*' -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath
if (-not $vsPath) { throw 'Visual Studio with Desktop development with C++ is required.' }
$vsVersion = & $vswhere -latest -products '*' -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationVersion
$vsMajor = ([version]$vsVersion).Major
if ($vsMajor -ge 18) { $generator = 'Visual Studio 18 2026' }
elseif ($vsMajor -eq 17) { $generator = 'Visual Studio 17 2022' }
else { throw 'Visual Studio 2022 or newer is required.' }
$cmake = Join-Path $vsPath 'Common7/IDE/CommonExtensions/Microsoft/CMake/CMake/bin/cmake.exe'
$ctest = Join-Path $vsPath 'Common7/IDE/CommonExtensions/Microsoft/CMake/CMake/bin/ctest.exe'
$buildPath = Join-Path $projectRoot $BuildDirectory
if (-not (Test-Path (Join-Path $JucePath 'modules/juce_core/juce_core.h'))) { throw 'Set -JucePath to JUCE 8.0.12.' }
& $cmake -S $projectRoot -B $buildPath -G $generator -A x64 "-DJUCE_DIR=$JucePath" -DBUILD_TESTING=ON
if ($LASTEXITCODE -ne 0) { throw 'CMake configuration failed.' }
# A single MSBuild worker also works inside the restricted Windows environment.
& $cmake --build $buildPath --config $Configuration --target PruedFX_VST3 PerformanceTests VST3Smoke LicensingTests -j 1
if ($LASTEXITCODE -ne 0) { throw 'Build failed.' }
& $ctest --test-dir $buildPath -C $Configuration --output-on-failure --timeout 60
if ($LASTEXITCODE -ne 0) { throw 'Tests failed; no delivery package was created.' }
$reports = Join-Path $projectRoot 'reports'
New-Item -ItemType Directory -Force $reports | Out-Null
$preview = Join-Path $reports 'ui-preview.png'
& (Join-Path $buildPath "PerformanceTests_artefacts/$Configuration/PerformanceTests.exe") $preview
if ($LASTEXITCODE -ne 0) { throw 'UI render test failed.' }
Copy-Item -LiteralPath (Join-Path $buildPath 'Testing/Temporary/LastTest.log') -Destination (Join-Path $reports 'automated-tests.log') -Force
$delivery = Join-Path $projectRoot 'delivery'
New-Item -ItemType Directory -Force $delivery | Out-Null
$plugin = Join-Path $buildPath "PruedFX_artefacts/$Configuration/VST3/Prued FX.vst3"
Copy-Item -LiteralPath $plugin -Destination $delivery -Recurse -Force
Copy-Item -LiteralPath (Join-Path $projectRoot 'TESTING-TH.md') -Destination $delivery -Force
Copy-Item -LiteralPath (Join-Path $projectRoot 'LICENSING.md') -Destination $delivery -Force
Copy-Item -LiteralPath $preview,(Join-Path $reports 'VALIDATION.md') -Destination $delivery -Force
Compress-Archive -LiteralPath (Join-Path $delivery 'Prued FX.vst3'),(Join-Path $delivery 'TESTING-TH.md'),(Join-Path $delivery 'LICENSING.md'),(Join-Path $delivery 'ui-preview.png'),(Join-Path $delivery 'VALIDATION.md') -DestinationPath (Join-Path $delivery 'Prued-FX-2.0.2-Windows-x64-test.zip') -Force
Copy-Item -LiteralPath (Join-Path $delivery 'Prued-FX-2.0.2-Windows-x64-test.zip') -Destination (Join-Path $delivery 'Prued-FX-2.0.2-Windows-x64-Demo-30min.zip') -Force
Copy-Item -LiteralPath (Join-Path $projectRoot 'INSTALL-WINDOWS.md'),(Join-Path $projectRoot 'CHANGELOG.md') -Destination $delivery -Force
Compress-Archive -LiteralPath (Join-Path $delivery 'Prued FX.vst3'),(Join-Path $delivery 'INSTALL-WINDOWS.md'),(Join-Path $delivery 'CHANGELOG.md'),(Join-Path $delivery 'LICENSING.md') -DestinationPath (Join-Path $delivery 'Prued-FX-2.0.2-Windows-x64.zip') -Force
Get-FileHash -LiteralPath (Join-Path $delivery 'Prued-FX-2.0.2-Windows-x64-test.zip') -Algorithm SHA256
Write-Host "Ready for DAW testing: $delivery"
