# Prued FX

## 2.0.2 Mac dry-leak fix (25 October 2026)

Fixed the processor passing Stutter amount as an extra wet/dry multiplier. With default Intensity and Dry/Wet at 100%, an engaged frozen loop now excludes new live dry input at every active knob position after its short transition. Explicit Intensity below 100%, bypass, demo expiry and initial capture preparation can still return/blend dry by design. The regression exercises the complete processor at five Stutter amounts, three mix values and three sample rates; it fails before the fix. Demo remains 30 minutes per instance.

## Prued FX 2.0 — Windows / macOS performance looper

Windows RC2 ผ่านการตรวจรับโดยผู้พัฒนาแล้วเมื่อ 23 ตุลาคม 2026 ดู CHANGELOG.md
สำหรับ Mac ให้ clone branch `feature/prued-fx-2-performance-looper` แล้วทำตาม
TESTING-MACOS-TH.md ยังไม่มีการอ้างว่า Mac 2.0 build/run ผ่านแล้ว
commit `c63d74c` ที่กล่าวถึงด้านล่างเป็น commit กู้ซอร์สในประวัติบน Windows
สำเนาไฟล์ต้นฉบับที่ไม่แก้ไขอยู่ `recovery/original/Source` เพื่อให้ตรวจสอบได้บน Mac
โดยไม่ต้องพึ่งประวัติ Git บน Windows

Source เดิมกู้คืนจาก `D:\JUICED\Yuay Stutter.rar` แล้วใน branch
`feature/prued-fx-2-performance-looper` โดยเก็บไฟล์เดิมและ hash ใน commit `c63d74c`
ก่อนเริ่มแก้ไข ดู `recovery/SOURCE_RECOVERY.json`

- ปุ่ม Stutter เดิมใช้ DSP แบบ performance loop โดยตรง ไม่มีโหมด Looper แยก
- ปุ่มเดียวควบคุมความเข้มและอัตราซ้ำ; เพิ่ม Beat Sync, Recapture และ crossfade ในการทำงานภายใน
- ฟรี 30 นาทีต่อ instance; ครบเวลาแล้ว fade กลับ dry ถอดแล้วใส่ใหม่เริ่มนับใหม่ ซื้อคีย์ Gumroad เพื่อปลดล็อกในไฟล์เดิม คลิกขวาเพื่อ Activate ดู [รายละเอียด](LICENSING.md)
- คง VST3 class IDs และ parameter IDs เดิม; เปิด MIDI input ที่เดิมถูกปิดใน build
- คืน UI เดิมจาก Source ที่กู้มา: artwork, knob filmstrips, ขนาด 718×506 และตำแหน่งเดิมทั้งหมด ปรับเฉพาะฟอนต์ readout ที่มีอยู่สามจุด
- [Build และทดสอบ Windows](TESTING-TH.md), [ผลตรวจ](reports/VALIDATION.md), [ภาพ UI](reports/ui-preview.png)
- สคริปต์ `.\scripts\build-windows.ps1` สร้าง Release x64, ทดสอบ และจัด ZIP ใน `delivery/`
- [Build macOS Universal](TESTING-MACOS-TH.md): `bash scripts/build-macos.sh --juce /path/to/JUCE`
- Windows x64 ผ่าน build/run ที่นี่; macOS arm64 + Intel เตรียม CMake/Xcode และ CI แล้ว ยังต้องมีผล build/run จริงบน Mac
- `.github/workflows/build-plugins.yml` เตรียม build/test และเก็บ artifacts แยก Windows, Apple Silicon และ Intel หลังนำ branch ขึ้น GitHub

รายละเอียดด้านล่างเป็นบันทึกการกู้ macOS baseline ก่อนพบ Source บน Windows
`BASELINE.json` และ archive macOS ยังคงเดิม

โปรเจกต์เดิมของ Yuay Sound Records — สำรองตัวปลั๊กอินที่พบในเครื่อง

## สถานะที่ยืนยันได้

- พบ Prued FX 1.0.0 ใน /Library/Audio/Plug-Ins/VST3/
- เป็น VST3 สำหรับ macOS arm64; bundle ID: com.YuaySoundRecords.PruedFX
- เก็บสำเนา bundle ใน artifacts/ พร้อมตรวจ SHA-256 ของแต่ละไฟล์หลังบีบอัด
- ตรวจความสมบูรณ์ลายเซ็น bundle ด้วย codesign ผ่าน ไม่ใช่ผลรับรอง notarization หรือการทดสอบใน DAW
- ผู้ใช้ระบุว่าตัวเดิมเคยทำสำเร็จแล้ว แต่รอบนี้ยังไม่ได้รันทดสอบเสียงหรือ GUI

## สถานะซอร์สโค้ด ณ ตอนสำรอง macOS เดิม

**ยังไม่พบซอร์สโค้ดที่ใช้ build ตัวนี้** รีโปตอนนี้เป็นสำเนาเก็บรักษาตัวติดตั้งที่มีอยู่ ไม่ใช่ซอร์สที่แก้หรือ build ใหม่ได้ ไม่สร้างโค้ดทดแทนแล้วอ้างว่าเป็นตัวเดิม

เมื่อได้ source ZIP หรือโฟลเดอร์โปรเจกต์ ให้ตรวจชื่อ รุ่น plugin ID และ build settings เทียบ BASELINE.json ก่อนนำเข้าประวัติ Git รักษาเวอร์ชันที่ใช้งานได้ไว้ และอย่าอ้างว่า rebuild ได้จนกว่าจะ compile/link และทดสอบจริง

## ไฟล์

- artifacts/Prued-FX-1.0.0-macOS-arm64.vst3.zip — สำเนา bundle เดิม (ตัดเฉพาะ .DS_Store)
- BASELINE.json — ข้อมูลรุ่นและ SHA-256 ของทุกไฟล์
- AGENTS.md — ข้อกำหนดสำหรับทำงานต่อ

สำเนานี้ไม่ได้แก้ไขไฟล์ปลั๊กอินที่ติดตั้งอยู่ และไม่ได้ติดตั้งทับในเครื่อง
