# Release status — 24 October 2026

Windows 2.0.0 is published at https://benjilennon.gumroad.com/l/rkpck with update notes, the accepted Windows ZIP, and Gumroad license-key content. The public free-trial ZIP is artifacts/Prued-FX-2.0.0-Windows-x64.zip on this branch. It is the same binary; a key unlocks unlimited use. Creator accepted RC2 after the bypass and knob-drift fixes. Automated checks passed 7/7.

Windows binary SHA-256: a0ba41b6f60f6385ca687ac24dba0bd38e6ca6c6aad744cd6120a07c7db8c0e6
Windows ZIP SHA-256: 1ab73d74ebfd86fd09dd9085cb33dbfc4d98ec527f69c6c3e91eb1b230c9f5b1

## Continue on Mac

```bash
git clone --branch feature/prued-fx-2-performance-looper --single-branch https://github.com/yuaysoundrecords/prued-fx.git
cd prued-fx
git clone --branch 8.0.12 --depth 1 https://github.com/juce-framework/JUCE.git external/JUCE
bash scripts/build-macos.sh --juce "$PWD/external/JUCE"
```

Read AGENTS.md and TESTING-MACOS-TH.md first. Keep main and the original baseline unchanged. Preserve the released UI; the existing Stutter knob directly controls the looper. Source was recovered from the creator's original archive; untouched source copies are in recovery/original/Source. The source tree was imported byte-for-byte from Windows commit ffa498c; local Windows historical commit IDs are provenance references, not commits in this imported branch history.

macOS 2.0 still requires native Apple Silicon and Intel build/runtime checks, Keychain activation/persistence checks, DAW/preset/MIDI validation, and distribution signing/notarization. The existing Gumroad Mac binary is legacy, not 2.0. An independent positive real-key end-to-end test is not claimed; automated license response fixtures passed and the earlier seller test checkout was blocked by CAPTCHA.
