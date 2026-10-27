#!/bin/bash
# Yuay Sound Records — Prued FX. macOS /bin/bash 3.2 compatible.
set -u
PATH=/usr/bin:/bin:/usr/sbin:/sbin
export PATH
say() { printf '%s\n%s\n\n' "$1" "$2"; }
fail() { say "$1" "$2" >&2; exit 1; }
pause_exit() {
    if [ -t 0 ]; then
        printf '\nกด Enter เพื่อปิด / Press Enter to close: '
        IFS= read -r ignored || true
    fi
}
trap pause_exit EXIT
say 'Prued FX — Yuay Sound Records | ตัวช่วยอนุญาตเฉพาะปลั๊กอินนี้' \
    'Prued FX — Yuay Sound Records | Approval helper for this plug-in only'
say 'ใช้หลังติดตั้งและปิด DAW ทั้งหมดแล้ว ทำต่อเมื่อเชื่อถือแหล่งดาวน์โหลดเท่านั้น' \
    'Use after installation with all DAWs closed. Continue only if you trust the download source.'
say 'ตัวช่วยนี้ไม่ใช่ Developer ID หรือ Apple notarization ลายเซ็น ad-hoc ที่ตรวจผ่านไม่ได้ยืนยันตัวตนผู้พัฒนา' \
    'This is not Developer ID signing or Apple notarization. A valid ad-hoc signature does not identify the developer.'
say 'macOS อาจบล็อกไฟล์ .command นี้เอง และไม่รับประกันว่าจะใช้ได้กับทุกเครื่องหรือทุก DAW' \
    'macOS may block this .command file itself. Compatibility with every Mac or DAW is not guaranteed.'
[ "$(uname -s)" = Darwin ] || fail 'ใช้ได้เฉพาะ macOS เท่านั้น' 'This helper runs on macOS only.'
[ -n "${HOME:-}" ] && [ "${HOME#/}" != "$HOME" ] || fail 'ตำแหน่งโฟลเดอร์ผู้ใช้ไม่ถูกต้อง' 'Invalid user home directory.'

# Check every ancestor and every entry inside a bundle, not only the top level.
validate_bundle() {
    local bundle="$1" part="$1" links identifier
    while [ "$part" != / ]; do
        [ ! -L "$part" ] || fail "ปฏิเสธ symbolic link: $part" "Symbolic link refused: $part"
        part="$(dirname "$part")"
    done
    [ -d "$bundle" ] || fail "ไม่พบ bundle: $bundle" "Bundle not found: $bundle"
    links="$(/usr/bin/find "$bundle" -type l -print -quit)" || fail 'อ่านไฟล์ใน bundle ไม่สำเร็จ' 'Unable to inspect bundle entries.'
    [ -z "$links" ] || fail "ปฏิเสธ symbolic link ใน bundle: $links" "Symbolic link inside bundle refused: $links"
    [ -f "$bundle/Contents/Info.plist" ] || fail 'ไม่พบข้อมูลปลั๊กอิน หยุดทำงาน' 'Missing plug-in metadata; stopped.'
    identifier="$(/usr/libexec/PlistBuddy -c 'Print :CFBundleIdentifier' "$bundle/Contents/Info.plist" 2>/dev/null)" || fail 'อ่านข้อมูลปลั๊กอินไม่สำเร็จ' 'Cannot read plug-in metadata.'
    [ "$identifier" = com.YuaySoundRecords.PruedFX ] || fail "ไม่ใช่ผลิตภัณฑ์ Prued FX: $bundle" "Not the expected Prued FX product: $bundle"
    if ! /usr/bin/codesign --verify --deep --strict "$bundle"; then
        fail "ตรวจลายเซ็นไม่ผ่าน หยุดโดยไม่เปลี่ยนไฟล์: $bundle" "Signature verification failed; no changes made to this bundle: $bundle"
    fi
}

candidates=(
    '/Library/Audio/Plug-Ins/VST3/Prued FX.vst3'
    "$HOME/Library/Audio/Plug-Ins/VST3/Prued FX.vst3"
    '/Library/Audio/Plug-Ins/Components/Prued FX.component'
    "$HOME/Library/Audio/Plug-Ins/Components/Prued FX.component"
)
found=()
for bundle in "${candidates[@]}"; do
    if [ -e "$bundle" ] || [ -L "$bundle" ]; then
        say "พบ Prued FX: $bundle" "Found Prued FX: $bundle"
        validate_bundle "$bundle"
        found+=("$bundle")
    fi
done
[ "${#found[@]}" -gt 0 ] || fail 'ไม่พบ Prued FX ในตำแหน่ง AU/VST3 มาตรฐาน กรุณาติดตั้งก่อน' \
    'Prued FX was not found in the standard AU/VST3 locations. Install it first.'

for bundle in "${found[@]}"; do
    say "ปลั๊กอินที่จะดำเนินการ: Prued FX — $bundle" "Target plug-in: Prued FX — $bundle"
    say 'จะถอดสถานะ com.apple.quarantine เฉพาะ bundle ข้างต้น ไม่ปิด Gatekeeper และไม่แก้ทั้งโฟลเดอร์ Plug-Ins' \
        'Only com.apple.quarantine on the bundle above will be removed. Gatekeeper stays enabled; the Plug-Ins directory is not modified as a whole.'
    printf 'พิมพ์ YES เพื่อยืนยัน หรือกด Enter เพื่อยกเลิก / Type YES to confirm, or press Enter to cancel: '
    answer=''
    IFS= read -r answer || answer=''
    if [ "$answer" != YES ]; then
        say "ยกเลิก ไม่เปลี่ยนไฟล์: $bundle" "Cancelled; no changes to: $bundle"
        continue
    fi
    # Recheck after confirmation, immediately before the scoped operation.
    validate_bundle "$bundle"
    if [[ "$bundle" == /Library/Audio/Plug-Ins/* ]] && [ ! -w "$bundle/Contents/MacOS/Prued FX" ]; then
            say 'ตำแหน่งระบบอาจต้องใช้รหัสผ่าน Mac ขณะพิมพ์จะไม่แสดงตัวอักษรหรือจุด แล้วกด Enter' \
                'A system installation may require your Mac password. No letters or dots appear while typing; then press Enter.'
            if ! /usr/bin/sudo -- /usr/bin/xattr -dr com.apple.quarantine "$bundle"; then
                fail "เกิดข้อผิดพลาดในการอนุญาต หยุดทำงาน: $bundle" "Approval command failed; stopped: $bundle"
            fi
    else
            if ! /usr/bin/xattr -dr com.apple.quarantine "$bundle"; then
                fail "เกิดข้อผิดพลาดในการอนุญาต ไม่ได้ใช้ sudo กับตำแหน่งผู้ใช้: $bundle" "Approval failed; sudo was not used for a user installation: $bundle"
            fi
    fi
    validate_bundle "$bundle"
    attributes="$(/usr/bin/xattr -lr "$bundle")" || fail 'ตรวจผลหลังดำเนินการไม่สำเร็จ' 'Unable to verify the result.'
    if printf '%s\n' "$attributes" | /usr/bin/grep -q 'com.apple.quarantine:'; then
        fail 'ยังพบสถานะ quarantine หยุดโดยไม่รายงานว่าสำเร็จ' 'Quarantine remains; success is not reported.'
    fi
    say "ทำสำเร็จเฉพาะ bundle นี้: $bundle" "Completed for this bundle only: $bundle"
    say 'เปิด DAW แล้วสแกนปลั๊กอินใหม่ หากยังมีคำเตือนหรือค้าง ให้หยุดและติดต่อ Yuay Sound Records' \
        'Open your DAW and rescan plug-ins. If warnings or hangs remain, stop and contact Yuay Sound Records.'
done
