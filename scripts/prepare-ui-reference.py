"""Recover a test-only editor fixture from the recorded original source commit."""
from pathlib import Path
import subprocess
root=Path(__file__).resolve().parents[1]
out=root/'tests/reference'
out.mkdir(exist_ok=True)
for ext in ['h','cpp']:
    source=subprocess.check_output(['git','-C',str(root),'show',f'c63d74c:Source/PluginEditor.{ext}']).decode('utf-8')
    source=source.replace('YuayStutterAudioProcessorEditor','RecoveredPruedEditor')
    source=source.replace('"PluginEditor.h"','"RecoveredPluginEditor.h"').replace('"FilmStripKnob.h"','"filmstripknob.h"')
    (out/f'RecoveredPluginEditor.{ext}').write_text(source,encoding='utf-8')
