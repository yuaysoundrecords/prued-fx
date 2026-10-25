# Mac 2.0.2 validation — 25 October 2026

- Regression failed before the DSP fix; passed after it on native arm64 and x86_64 via Rosetta.
- Native CTest 7/7 passed: PerformanceTests, FrozenWetMixRegression, TransportAutomationStress, BypassAndKnobRegression, LicensingTests, OriginalUIRegression, VST3Smoke.
- Actual VST3 load, stereo processing, host bypass, state and editor passed natively and under Rosetta.
- VST3/AU Universal arm64+x86_64 built; ad-hoc codesign verification passed; ZIP integrity passed.
- AU is compile/signature verified; Logic/auval untested. No physical Intel runtime, real-key/Keychain, DAW listening or distribution notarization is claimed.
- Installed plugins were not changed.

Before installing, close the DAW and back up the old Prued FX plugin. Replace the old bundle rather than adding a second copy, because IDs are preserved. Rescan and confirm version 2.0.2. Test Stutter at 15%, 42% and 75% with Mix 100% and Fix mode; use varying live input after capturing a loop. Keep Intensity at its default 100% if previously automated.
