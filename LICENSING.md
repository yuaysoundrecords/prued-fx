# Prued FX 2.0 licensing and free trial

The Windows VST3 uses the same binary for free and licensed use. The main editor
keeps the released layout. Right-click the plugin to see remaining trial time,
activate a Gumroad key, or open the purchase page.

## Free use

Each plugin instance allows 900 seconds of processed audio. At expiry the effect
fades to unity dry input over 5 ms; it does not mute the track. Remove the plugin
and insert a new instance to start another 15 minutes. Closing the editor,
loading a preset, changing sample rate, or stopping/restarting processing does
not reset the existing instance. Offline rendering also consumes processed time.

## Paid use

Buy Prued FX at https://benjilennon.gumroad.com/l/rkpck and use the key from the
receipt. Right-click → Activate / License status. An accepted key immediately
restores unlimited effect processing, including after trial expiry.

Activation needs internet. The plugin sends the key and public product ID to
Gumroad's HTTPS license verification endpoint on a background thread. It does
not send audio, presets or a device fingerprint, and does not increment the
license usage counter. Wrong-product, refunded, chargeback and unresolved
disputed purchases are rejected. Gumroad handles purchase/customer data under
its own privacy policy. Existing eligible purchases receive keys retroactively.

On Windows the verified key is encrypted with Windows DPAPI for the current
Windows user, in `%APPDATA%/Yuay Sound Records/Prued FX/license.dat`. Keys are
excluded from presets, source control and test reports. After one hour the next
instance rechecks the license. A previously verified key allows up to seven days
offline from its last successful verification; after that, reconnect to verify.
A definitive rejection clears the cached license. Temporary network failure
does not revoke a still-valid offline cache. Removing the cache removes saved
activation. The macOS build stores the same activation record in the user's
Keychain under service `com.YuaySoundRecords.PruedFX.license`, account `Gumroad`.
Keychain queries fail without prompting if access is unavailable; re-activate in
that host if necessary. Mac Keychain persistence still needs runtime testing.
Implementation follows Apple's [generic password](https://developer.apple.com/documentation/security/ksecclassgenericpassword)
and [lookup](https://developer.apple.com/documentation/security/secitemcopymatching(_:_:)) APIs.

## Release gate

Automated fixtures exercise expiry, reset rules and accepted/rejected responses.
They do not replace a real Gumroad-issued-key activation or listening in a DAW.
Commercial release requires those checks to be recorded in reports/VALIDATION.md.
