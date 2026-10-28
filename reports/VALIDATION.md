# Windows 2.0.4 validation — 28 October 2026

Built from shared Mac commit 31addc3624262510a934f3f2b9c614e7638f935b, with no DSP/editor changes in this Windows update. JUCE 8.0.12, Visual Studio 2026 Release x64.

- All 7/7 CTest checks passed: processor/presets, frozen wet isolation, transport/MIDI stress, bypass/filmstrip controls, unrestricted processing and unity default, UI comparison, actual VST3 host load/audio/state/editor/bypass.
- Original 718x506 layout retained; supplied Mac Ko-fi artwork appears at top right. Plugin and parameter IDs retained.
- Installer fresh installation and upgrade from 2.0.1 passed in an isolated workspace directory; all installed bundle files match the validated build hashes. The installed bundle passed VST3Smoke.
- Production installed plugin was not overwritten. Administrator/system-wide UAC installation was not run; isolated installer tests used /CURRENTUSER with an explicit test directory.
- Windows executable is not Authenticode-signed. Real DAW listening on 2.0.4 is not claimed; creator acceptance of 2.0.0 RC2 is historical.

Binary SHA256: 2835d3b74446c3576453ab390c9568410f4796979377be8ed409695c42adc24e
Setup SHA256: 655dce50f2eb3cd12f0fe458effddd1f6cfae5fa3f09fdae94d1428a982dbb68

See automated-tests.log, windows-2.0.4-installer.log, windows-2.0.4-upgrade.log and BUILD_MANIFEST.json. No trial or activation applies to this release.
