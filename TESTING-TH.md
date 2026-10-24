# Prued FX 2.0 — Windows x64 VST3 test build

Source ที่กู้มาจาก `D:\JUICED\Yuay Stutter.rar` ถูกเก็บเป็น commit แยกก่อนพัฒนา
ดู `recovery/SOURCE_RECOVERY.json` สำหรับ SHA-256 และ plugin IDs
`BASELINE.json` เป็นหลักฐาน macOS 1.0.0 เดิม ห้ามตีความว่าเป็นผลทดสอบ Windows 2.0

## Build ด้วยตนเอง

ต้องมี Visual Studio 2022 หรือ 2026 / Desktop development with C++, Windows SDK และ JUCE 8.0.12
จากโฟลเดอร์ repo รันใน PowerShell:

```powershell
.\scripts\build-windows.ps1 -JucePath 'D:\JUICED\JUCE'
```

สคริปต์จะ configure, build Release x64, รันทดสอบ และสร้าง ZIP ใน `delivery`
ไม่ติดตั้งปลั๊กอินให้โดยอัตโนมัติ
สำหรับ Mac ใช้ Source เดียวกัน ดู `TESTING-MACOS-TH.md` สำหรับ Universal VST3 และตัวเลือก AU
ชุดที่ build ในรอบพัฒนานี้ใช้ `build/baseline` เป็น working build directory
สำเนา build ของ Source เดิมอยู่ `build/recovered-1.0.0`

## ทดลองใน DAW

Windows RC2 แก้การครอปเฟรมที่ทำให้ knob ลอย และแก้ BYPASS ให้พื้นที่ข้อความ/ไฟ
คลิกได้ครบพร้อมเชื่อม host bypass หลังเปลี่ยนไฟล์ให้ตรวจว่า Bypass คืนเสียง input
ระดับเดิมและกดซ้ำกลับมาเป็น effect หมุน Stutter, Output และ Dry/Wet ตลอดช่วง
เพื่อตรวจว่าภาพอยู่กับที่ โดยใช้ไฟล์ RC2 แทน RC1

1. สำรองปลั๊กอินเดิมและเปิดโปรเจกต์ DAW สำเนาสำหรับทดสอบ เนื่องจากใช้ VST3 IDs เดิม DAW จะมองว่าเป็นปลั๊กอินตัวเดียวกัน
2. แตก ZIP แล้วนำทั้งโฟลเดอร์ `Prued FX.vst3` ไปไว้ใน VST3 test location ที่ DAW ของคุณสแกนได้ หรือติดตั้งด้วยตนเองใน `C:\Program Files\Common Files\VST3` หลังสำรองรุ่นเดิม ปิด DAW ระหว่างเปลี่ยนไฟล์
3. Rescan แล้วตรวจชื่อ Prued FX และ version 2.0.0 เปิด preset/session เดิม ปิดและเปิดหน้าต่างซ้ำ ตรวจ Stutter, Bypass, Refresh, Output และ Dry/Wet ว่าไม่เปลี่ยน
4. ใช้หน้าตาและปุ่มเดิมทั้งหมด ไม่ต้องเปิดโหมดเพิ่มเติม เล่น input อย่างน้อยหนึ่ง beat แล้วหมุน Stutter ขึ้นจากศูนย์เพื่อจับลูป
5. Stutter ปุ่มเดิมปรับทั้งความเข้มและอัตราย่อย 1, 2, 4, 8, 16, 32 ตาม beat; กลับศูนย์เพื่อปล่อยลูปกลับเสียงสด
6. ใช้ Fix/Yuay เดิมสำหรับ Refresh/จับใหม่อัตโนมัติ; Output และ Dry/Wet ยังใช้ปุ่มเดิม
7. ถ้าต้องการตั้งค่าขั้นสูง ใช้ host parameter/automation หรือ MIDI: Intensity เป็นตัวคูณความเข้ม, Beat Sync, Loop Length และ Recapture ไม่มีการเพิ่ม control บนหน้าหลัก
8. ทดสอบ BPM 60/120/180, play/stop/seek, automation, 44.1/48/96 kHz และ buffers 64/128/512/1024 ตรวจเสียงแตก click dropout และ CPU meter
9. บันทึก session, ปิด DAW และเปิดใหม่ ตรวจค่าที่เรียกคืน ลูปเสียงที่จับไว้ไม่ได้บันทึกใน preset ต้องจับ input ใหม่

รอบ Windows ล่าสุดเพิ่มการกัน rate สลับไปมาระหว่างหมุนใกล้ขอบขั้น และเปลี่ยน rate บนขอบ 1/16 note เมื่อมี host sync พร้อม crossfade โค้งนุ่มในจังหวะจับใหม่และเปลี่ยนอัตราซ้ำ

## ทดสอบใช้ฟรีและปลดล็อก

- ฟรี 15 นาทีของเสียงที่ประมวลผลต่อ instance รวม offline render เมื่อครบจะ fade 5 ms กลับเสียง dry ระดับเดิมของ input ไม่ตัดเสียง
- ถอดปลั๊กอินแล้วใส่ใหม่เริ่ม 15 นาทีใหม่ ปิดหน้าต่างหรือโหลด preset ไม่เริ่มเวลาใหม่
- คลิกขวาในหน้าปลั๊กอินเพื่อดูเวลาคงเหลือหรือเลือก Activate / License status แล้วใส่คีย์จากใบเสร็จ Gumroad ต้องออนไลน์ครั้งแรก
- คีย์ถูกต้องต้องปลดล็อกทันที แม้ใช้ฟรีครบแล้ว และเปิด instance ใหม่ต้องจำการปลดล็อกได้ รายละเอียดการเก็บคีย์และ offline grace อยู่ใน LICENSING.md
- ตรวจคีย์ผิด คีย์คนละสินค้า และเน็ตขาดด้วย ต้องไม่ค้างเสียงหรือปิด DAW

## MIDI mapping

| CC | หน้าที่ |
|---|---|
| 1 | Stutter (ของเดิม) |
| 7 | Refresh / Auto Recapture, เปิดเมื่อค่า ≥64 (ของเดิม) |
| 16 | Intensity |
| 17 | Beat Sync, เปิดเมื่อค่า ≥64 |
| 18 | Recapture เมื่อเปลี่ยนจากค่าต่ำกว่า64เป็น ≥64; ส่งค่าต่ำอีกครั้งก่อน trigger ถัดไป |

MIDI รับทุก channel และประมวลผลตาม sample offset; เปิด routing MIDI ไปยัง effect ใน DAW ก่อนใช้งาน

## ขอบเขตที่ควรรู้

- Snapshot เตรียมแบบแบ่งงาน 64 frames ต่อ sample ไม่มี allocation ใน engine ระหว่างเล่น จากนั้นเริ่มลูปบนขอบ 1/16 note ถัดไปเมื่อมี PPQ และ transport กำลังเล่น การเริ่มจับจึงไม่ใช่ทันทีที่กด
- ไม่ใช่ time-stretch engine; เปลี่ยน BPM จะเตรียมจับลูปใหม่เพื่ออัปเดตระยะ beat ความยาวสูงสุด 8 วินาที (จึงจำกัดกรณี 4 beats ที่ tempo ต่ำกว่า30 BPM)
- Beat Sync ปิดใช้การปรับ repeat length ต่อเนื่อง; ไม่มี PPQ จะทำงานโดยไม่รอ grid มี tempo fallback 120 BPM
- Crossfade 5 ms สำหรับจับใหม่/เปลี่ยน slice/ปล่อย และ smoothing ของ mix, gain, bypass; รอยต่อลูปใช้ seam blend โดยคงจำนวน sample ต่อรอบ
- Stutter ใช้ snapshot แยกจึงค้างลูปได้นาน; ค่าและ IDs ของ preset เดิมยังอ่านได้ แต่เสียง Stutter ถูกปรับปรุงตามฟังก์ชัน 2.0 จึงไม่อ้างว่าเสียงเหมือน 1.0 แบบ bit-for-bit
- เป็น test build ไม่ได้เซ็น Windows Authenticode ผลทดสอบด้วย host อัตโนมัติและภาพ render ไม่ใช่การฟัง/ใช้งานจริงใน DAW
- ตรวจเฟรมวิดีโอ LOOPERMAN ที่ผู้ใช้ส่งและภาพ Yuay MIDI Port เพื่อเป็นแนวทาง ไม่อ้างว่า reverse-engineer หรือเหมือน DSP ของ LOOPERMAN ทุกประการ
