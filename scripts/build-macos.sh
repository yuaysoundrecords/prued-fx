#!/bin/bash
# Build on macOS: bash scripts/build-macos.sh --juce /path/to/JUCE [--with-au]
set -euo pipefail
project_root="$(cd "$(dirname "$0")/.." && pwd)"
juce_path="${JUCE_DIR:-$project_root/external/JUCE}"
build_path="$project_root/build/macos"
with_au=OFF
while [[ $# -gt 0 ]]; do
    case "$1" in
        --juce) juce_path="${2:?Missing JUCE path}"; shift 2 ;;
        --build-dir) build_path="${2:?Missing build directory}"; shift 2 ;;
        --with-au) with_au=ON; shift ;;
        *) printf 'Unknown argument: %s\n' "$1" >&2; exit 2 ;;
    esac
done
[[ "$(uname -s)" == Darwin ]] || { echo 'Run this script on macOS with Xcode.' >&2; exit 1; }
[[ -f "$juce_path/modules/juce_core/juce_core.h" ]] || { echo 'JUCE 8.0.12 is required; use --juce /path/to/JUCE.' >&2; exit 1; }
command -v cmake >/dev/null || { echo 'Install CMake first.' >&2; exit 1; }
xcodebuild -version
targets=(PruedFX_VST3 PerformanceTests VST3Smoke LicensingTests)
[[ "$with_au" == OFF ]] || targets+=(PruedFX_AU)
cmake -S "$project_root" -B "$build_path" -G Xcode \
    "-DJUCE_DIR=$juce_path" -DBUILD_TESTING=ON "-DPRUED_BUILD_AU=$with_au" \
    '-DCMAKE_OSX_ARCHITECTURES=arm64;x86_64' -DCMAKE_OSX_DEPLOYMENT_TARGET=11.0
cmake --build "$build_path" --config Release --target "${targets[@]}" --parallel 2
ctest --test-dir "$build_path" -C Release --output-on-failure --timeout 120

plugin="$build_path/PruedFX_artefacts/Release/VST3/Prued FX.vst3"
binary="$plugin/Contents/MacOS/Prued FX"
# Verify both slices, rather than merely trusting the CMake architecture option.
lipo -verify_arch arm64 x86_64 "$binary"
# Local test signing only. Distribution Developer ID signing/notarization is separate.
codesign --force --deep --sign - "$plugin"
codesign --verify --deep --strict "$plugin"
"$build_path/VST3Smoke_artefacts/Release/VST3Smoke" "$plugin"

format_label=VST3
[[ "$with_au" == OFF ]] || format_label=VST3-AU
delivery="$project_root/delivery/macos-$format_label"
reports="$project_root/reports/macos"
mkdir -p "$delivery" "$reports"
"$build_path/PerformanceTests_artefacts/Release/PerformanceTests" "$reports/ui-preview.png"
cp "$build_path/Testing/Temporary/LastTest.log" "$reports/automated-tests.log"
ditto "$plugin" "$delivery/Prued FX.vst3"
if [[ "$with_au" == ON ]]; then
    au="$build_path/PruedFX_artefacts/Release/AU/Prued FX.component"
    lipo -verify_arch arm64 x86_64 "$au/Contents/MacOS/Prued FX"
    codesign --force --deep --sign - "$au"
    codesign --verify --deep --strict "$au"
    ditto "$au" "$delivery/Prued FX.component"
fi
cp "$project_root/TESTING-MACOS-TH.md" "$delivery/"
cp "$project_root/LICENSING.md" "$delivery/"
cp "$reports/ui-preview.png" "$delivery/"
archive="$project_root/delivery/Prued-FX-2.0.0-macOS-Universal-$format_label-test.zip"
ditto -c -k --sequesterRsrc --keepParent "$delivery" "$archive"
{
    printf 'Native test architecture: %s\n' "$(uname -m)"
    lipo -info "$binary"
    shasum -a 256 "$archive" "$binary"
} | tee "$reports/build-manifest.txt"
printf 'Ready for DAW testing: %s\n' "$archive"
