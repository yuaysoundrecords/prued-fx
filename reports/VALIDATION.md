# Prued FX 2.0 Windows validation — 2026-10-23

Current delivery priority is Windows. Mac build/run is deferred until Windows
Stutter behavior is accepted. No macOS 2.0 binary is claimed as tested.

## Cross-platform follow-up

Shared CMake now accepts an explicit JUCE path or JUCE_DIR environment variable.
macOS targets Universal arm64+x86_64, minimum deployment target 11.0, with an
optional MIDI-controlled AU. The header filename case was corrected and the
original UI uses its embedded typeface on both operating systems.
`scripts/build-macos.sh` and the GitHub Actions workflow are prepared but have
not run on a Mac in this session. No macOS 2.0 binary is represented as delivered.
The workflow tests natively on Apple Silicon and Intel separately; its runner
labels follow https://docs.github.com/en/actions/reference/runners/github-hosted-runners.
JUCE is fixed at 8.0.12: https://github.com/juce-framework/JUCE/releases/tag/8.0.12.

Branch: `feature/prued-fx-2-performance-looper`.
Recovered-source commit: `c63d74c`. Original Windows project sources were extracted
from the user-supplied RAR and match the existing D: project byte for byte.
The macOS baseline archive and BASELINE.json remain unchanged.

## Build

- Windows x64 Release, Visual Studio Community 2026 18.9, MSVC 19.51,
  Windows SDK 10.0.26100.0, JUCE 8.0.12.
- Recovered unmodified C++ source built successfully as 1.0.0 using a new CMake
  integration. This proves the recovered source builds, not a bit-identical
  reconstruction of the historical macOS binary.
- 2.0 uses the same manufacturer `Manu`, product code `YUAY`, and VST3 class IDs:
  `ABCDEF019182FAEB4D616E7559554159` / `ABCDEF011234ABCD4D616E7559554159`.
- Build entry point: `scripts/build-windows.ps1`.

## Automated coverage

- Legacy APVTS XML state migration; all missing new IDs receive factory defaults.
- Original parameter order and values; state roundtrip; creating the editor does
  not reset Stutter or Output Level.
- 44.1/48/96 kHz; stereo phase equality; finite bounded output; manual recapture.
- Frozen capture survives more than 16 seconds of new input without being overwritten.
- Intensity zero returns dry input after the fade.
- CC1 event at sample 300 does not affect samples before its timestamp; processing
  also covers a 511-sample host block after prepareToPlay with a maximum of 64.
- Beat-grid fixture at 48 kHz / 120 BPM commits at sample 6000 (next sixteenth).
- Continuous-input seam/recapture/release fixture is checked against its `0.06`
  adjacent-sample bound. This is a numerical regression fixture,
  not proof that every possible signal is perceptually click-free.
- Actual VST3 module scan, instantiation, stereo processing, state restoration,
  and editor construction in a JUCE host.
- State from the actual recovered 1.0.0 VST3 loads into 2.0 with the five original
  parameter IDs, order and values intact. Stutter intentionally uses updated DSP;
  audio equivalence to 1.0 is no longer an acceptance criterion. There is no
  PERFORMANCE_MODE parameter or separate looper activation.
- The original STUTTER parameter alone must capture/hold new input beyond the old
  ten-second ring and release when returned to zero.
- Rate selection has hysteresis to reject knob/controller boundary jitter;
  synced rate changes commit on the host sixteenth grid. Capture and slice
  transitions use smoothstep crossfades; loop seams retain their musical period.

## Visual reference and checks

Reviewed frames from the supplied 37.233-second LOOPERMAN recording and the supplied
Yuay MIDI Port image. The recording demonstrates the large Intensity control and
performance context; it does not expose the competitor's DSP algorithm.
`ui-preview.png` is rendered from the actual JUCE editor by the test executable.
The editor is restored from recovered-source commit c63d74c: original 718 x 506
dimensions, eight original children, background/bypass images, knob filmstrips,
and all visible control bounds. The invisible BYPASS hit region now covers its
existing label and lamp (330,455,58,42). Visible design changes are limited to the font size of the
three existing readouts. Artwork hashes are unchanged. Constructor preset resets
were removed and the existing timer initializes immediately to display saved values.
The original editor fixture is recovered from c63d74c (test-only class/include
renaming). Both editors use the current FilmStripKnob renderer; this layout/font
comparison is not a comparison of the historical faulty frame cropping. The
separate 127-frame regression checks the corrected animation. Direct JUCE editor renders avoid black native-window snapshots from
VST3 host wrappers. The UI regression verifies zero pixel changes outside the
three original readout rectangles; 589 changed pixels are confined to fonts.

## Licensing and trial checks

The earlier 2.0.0 local suite passed seven tests, including TransportAutomationStress
and LicensingTests. Stress covers 15 sample-rate/block-size combinations, tempo
changes, play/stop/seek and rapid MIDI CC automation with finite bounded output.
The actual processor expires after 1800 seconds of processed audio, fades to unity
dry, remains expired across prepare/preset reload and resumes processing after
injected accepted-key verification. Parsing tests reject wrong-product, refunded,
chargeback and disputed purchases. Keys are not serialized in presets.

macOS targets include LicensingTests and link the Security framework for Keychain
storage; native compile/run and Keychain/DAW checks remain pending.

Gumroad's existing paid Prued FX product now has a license-key content block.
The product ID in the plugin matches that block. The seller's no-charge test
checkout was blocked by Gumroad CAPTCHA verification; no real issued key has yet
passed the production HTTPS verifier. Windows encrypted-cache persistence and
real DAW listening still require end-to-end acceptance. These are release gates,
not passed checks. No new commercial binary has been uploaded to Gumroad.

## Windows control correction (RC2)

Filmstrip frame boundaries now use rounded positions from the full strip height.
The artwork heights are not divisible by 127; multiplying a truncated frame height
caused cumulative vertical crop drift as the knobs turned. Artwork files, visible
control positions and sizes are preserved. The two-state Fix/Yuay control ignores
drag events after a click, and popup gestures cannot change rotary values.

The original BYPASS parameter is exposed as the host bypass parameter. The existing
button reports a host gesture, its invisible target covers the existing label and
lamp, and host automation updates the editor toggle/indicator. The dedicated
BypassAndKnobRegression checks label hit testing, editor toggles, host mapping,
unity dry bypass, restored output gain and all 127 output-knob frame silhouettes.
VST3Smoke additionally checks host recognition of the bypass parameter and unity
dry output through the actual loaded VST3 module.

## Remaining release gates

No real DAW listening session, DAW automation stress test, audio-interface dropout
test, or Windows signing verification is represented as passed. This is an
unsigned test build. Follow TESTING-TH.md for installation and acceptance checks.
No installed plugin was replaced and main remains unchanged. Windows 2.0.0 was later accepted by the creator and published to Gumroad/GitHub on 24 October 2026; see reports/USER_ACCEPTANCE.md and RELEASE-STATUS.md on the published branch.

## 2.0.1 demo-duration update

Demo duration is now 1800 seconds (30 minutes) per instance. The licensing regression explicitly checks that processing remains enabled at the old 900-second limit, expires at 1800 seconds, and retains reload/preset/license behavior. This patch is built/tested separately; the earlier creator acceptance refers to 2.0.0 RC2. The optional actual recovered-1.0.0-module comparison is unavailable after the previous build cache cleanup; it passed for 2.0.0. Current legacy-state fixtures remain in the fresh suite.

The fresh Windows 2.0.1 Release build passed all 6 available tests, including the old-15-minute cutoff and exact-30-minute expiry regression, actual VST3 host bypass, original UI, legacy state, DSP/MIDI and transport stress. The installed plugin was not replaced.
