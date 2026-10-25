# Prued FX 2.0.2 — dry leakage diagnosis

The full processor passed amount * INTENSITY into PerformanceLooper. The looper uses that argument as the interpolation weight between live dry and the frozen loop. Thus at DRY_WET=1 and INTENSITY=1, a 0.42 Stutter setting produced 0.58 live dry + 0.42 loop after settling. The UI mix was already full wet, but it received a preblended signal. This bug is shared by Windows/macOS source.

Fix: pass INTENSITY alone as wet activation; retain Stutter for engagement and repeat-rate selection. Keep the existing 5ms engagement/release fades. Preserve UI, parameter IDs and 30-minute demo. Explicitly lowering INTENSITY, bypass, demo expiration, and waiting for the initial capture can still produce dry as designed. This is not a claim of algorithm equivalence to LOOPERMAN.

Regression: complete processor at 44.1/48/96 kHz, five Stutter positions (0.03, 0.15, 0.42, 0.75, 1), three Mix values (0, 0.5, 1), unity output. Capture constant +0.5 then change input to -0.5, verify every output sample against the requested mix after settling; verify return to dry at Stutter zero. Before-fix execution fails with live dry leakage.

Source base: 2b5e49e; local changes from the earlier Mac build are retained. Source and resulting plugin version: 2.0.2.
