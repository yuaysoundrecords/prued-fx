# Release status — Mac 2.0.2 / Windows 2.0.1

25 October 2026. Creator authorized uploading and publishing the Mac 2.0.2 update after reporting dry leakage in Mac 2.0.1.

## macOS 2.0.2

Fixed STUTTER amount being multiplied into internal wet activation. With default Intensity and Mix 100%, settled frozen loops now exclude new live dry input at every active Stutter setting. Demo remains 30 minutes per instance. Original UI, parameter IDs and plugin IDs preserved.

- Native arm64 CTest: 7/7 passed, including a full-processor wet-isolation regression that fails before the fix.
- Wet-isolation and actual VST3 host load/processing/state/editor/bypass passed on x86_64 via Rosetta.
- Universal arm64+x86_64 VST3 and optional AU built with JUCE 8.0.12 and Xcode 26.2.
- Ad-hoc codesign verification and ZIP integrity passed. Developer ID signing/notarization are not included.
- AU is an optional preview; Logic/auval is untested. Physical Intel runtime, independent real-key/Keychain and real DAW listening are not claimed.
- Release package: Prued-FX-2.0.2-macOS-Universal.zip
- Release ZIP SHA-256: c98670f3ec8e9ea49c74d9625a2d50e2416dbf7adc56d2a349c85d90e04f7362
- VST3 binary SHA-256: 6ae5a51290f546030b2487fc6e913f1f1e713e72bcf581ebcf4310978e6d19d8

The dry-leak fix is in shared source. Windows 2.0.2 has NOT been built or published in this Mac session. Existing Windows release remains 2.0.1.

## Previous Windows release record

# Release status — Windows 2.0.1 / 30-minute demo

Updated 24 October 2026. TrialGate::durationSeconds is 1800 seconds (30 minutes) per plugin instance on Windows and in the shared Mac source. Remove/reinsert resets the demo. The same Gumroad product/key unlocks unlimited use. Main and original UI/plugin/parameter IDs are unchanged.

Windows 2.0.1 is uploaded to the existing Gumroad product: https://benjilennon.gumroad.com/l/rkpck . Select Prued-FX-2.0.1-Windows-x64; 2.0.0 is the previous 15-minute version. The fresh Windows suite passed all 6 available tests, including active processing after the old 15-minute cutoff, exact 30-minute expiry, preset/reload rules, paid unlock fixtures, UI/knob/bypass, DSP/MIDI stress and actual VST3 host loading. Earlier 2.0.0 creator acceptance and 7-test legacy-module coverage remain historical evidence.

Windows 2.0.1 binary SHA-256: 0c8bb1999ee44a23f98b5a1f4722453185afed4a96e55d7e73a28f4dca02d39e
Windows 2.0.1 ZIP SHA-256: 0316d569183d0dde23208aa589ad49c699ac2b482c93f0c0525b786ea9b97d82

## Continue on Mac

```bash
git clone --branch feature/prued-fx-2-performance-looper --single-branch https://github.com/yuaysoundrecords/prued-fx.git
cd prued-fx
git clone --branch 8.0.12 --depth 1 https://github.com/juce-framework/JUCE.git external/JUCE
bash scripts/build-macos.sh --juce "$PWD/external/JUCE"
```

Read AGENTS.md and TESTING-MACOS-TH.md first. Keep main and the original baseline unchanged. Preserve the released UI; the existing Stutter knob directly controls the looper. Source was recovered from the creator's original archive; untouched source copies are in recovery/original/Source. The source tree was imported byte-for-byte from Windows commit ffa498c; local Windows historical commit IDs are provenance references, not commits in this imported branch history.

macOS 2.0 still requires native Apple Silicon and Intel build/runtime checks, Keychain activation/persistence checks, DAW/preset/MIDI validation, and distribution signing/notarization. The existing Gumroad Mac binary is legacy, not 2.0. An independent positive real-key end-to-end test is not claimed; automated license response fixtures passed and the earlier seller test checkout was blocked by CAPTCHA.
