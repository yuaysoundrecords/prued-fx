# Prued FX 2.0 — macOS Universal

เป้าหมาย: macOS 11 ขึ้นไป, Apple Silicon (arm64) และ Intel (x86_64)
ใช้ Source, DSP, VST3 IDs และฟอนต์ที่ฝังชุดเดียวกับ Windows
สถานะ: เตรียม build target แล้ว ยังไม่มีผล build/run Mac ใน session นี้

## Build บน Mac

ใช้ Xcode แบบเต็ม, command-line tools, CMake และ JUCE 8.0.12
จาก branch `feature/prued-fx-2-performance-looper` รัน:

```bash
git clone --branch 8.0.12 --depth 1 https://github.com/juce-framework/JUCE.git external/JUCE
bash scripts/build-macos.sh --juce "$PWD/external/JUCE"
```

สคริปต์ build Universal bundle, รัน DSP/preset/VST3-host tests บน architecture native
ของเครื่องที่ใช้, ตรวจ binary ทั้ง arm64/x86_64 ด้วย lipo, เซ็น ad-hoc สำหรับทดสอบ,
ตรวจความสมบูรณ์ codesign และจัด ZIP:
`delivery/Prued-FX-2.0.1-macOS-Universal-VST3-test.zip`

การมี binary ทั้งสองสถาปัตยกรรมไม่เท่ากับการทดสอบ runtime ทั้งสอง
workflow `.github/workflows/build-plugins.yml` จึงแยก Apple Silicon และ Intel runners
ต้องนำ branch ขึ้น GitHub และรัน workflow ก่อน จึงจะมีผล CI จริง
logs, ภาพ UI และ architecture/hash อยู่ `reports/macos/`
สคริปต์ไม่ติดตั้งหรือแก้ปลั๊กอินที่ติดตั้งอยู่

## ระบบฟรีและ License ในชุด Mac

ใช้ระบบ 30 นาทีต่อ instance และคีย์ Gumroad ชุดเดียวกับ Windows คลิกขวาเพื่อ
Activate; ชุด build รวม LicensingTests แล้ว และเตรียมเก็บ activation ใน macOS
Keychain (service `com.YuaySoundRecords.PruedFX.license`, account `Gumroad`)
ไม่มีคีย์ใน preset ต้องทดสอบ activate, เปิด instance ใหม่, offline grace และ
การเข้าถึง Keychain จาก DAW ที่ใช้งานจริงบน Mac ก่อนปล่อยขาย รายละเอียดอยู่ LICENSING.md
การเตรียม source/script บน Windows ไม่ใช่หลักฐานว่า Mac compile/run ผ่านแล้ว

## ทดลองใน DAW

1. สำรอง Prued FX เดิมก่อน เพราะใช้ VST3 class IDs และ bundle ID เดิม
2. ปิด DAW แล้วคัดลอกทั้ง `Prued FX.vst3` ไป `~/Library/Audio/Plug-Ins/VST3/`
3. เปิด DAW ที่รองรับ VST3 แล้ว rescan; ใช้โปรเจกต์สำเนาทดสอบ
4. ทำตามรายการทดสอบเสียง/MIDI/preset ใน `TESTING-TH.md` ตั้งแต่ข้อ DAW ที่ 3
5. ทดลอง Apple Silicon แบบ native และ Intel จริง ตรวจ UI, automation, BPM,
   play/stop/seek, recapture, CPU และ audio dropout
6. ลอง preset ข้าม Windows ↔ Mac; ลูปเสียงที่ capture ไว้ไม่บันทึกใน preset

## AU สำหรับ Logic Pro (ตัวเลือกเพิ่มเติม)

Logic Pro ต้องใช้ AU จึงเตรียม target เพิ่มเติมไว้:

```bash
bash scripts/build-macos.sh --juce "$PWD/external/JUCE" --with-au
```

เพิ่ม `Prued FX.component` ใน ZIP ชื่อ `macOS-Universal-VST3-AU-test.zip`
สำรอง AU เดิมก่อนติดตั้งใน `~/Library/Audio/Plug-Ins/Components/`
เป็น MIDI-controlled effect (`aumf`, manufacturer `Manu`, subtype `YUAY`)
เลือกชนิด MIDI-controlled effect และ route เสียง/sidechain ตาม DAW
AU ยังไม่ได้ทดสอบ auval/Logic หรือ preset ข้าม format AU/VST3
หลังติดตั้งเพื่อทดสอบ รัน:

```bash
auval -v aumf YUAY Manu
```

## การแจกจ่าย

Ad-hoc signing สำหรับทดสอบไม่ใช่ Developer ID signing, notarization หรือผลทดสอบ
Gatekeeper บน Mac ผู้ใช้ทั่วไป ต้องตรวจแยกก่อน release
