"""Compare actual baseline/candidate renders; permit only existing readout fonts."""
from pathlib import Path
from PIL import Image, ImageChops
import hashlib, json, subprocess

root=Path(__file__).resolve().parents[1]
old=Image.open(root/'reports/ui-original.png').convert('RGB')
new=Image.open(root/'reports/ui-corrected.png').convert('RGB')
assert old.size==new.size==(718,506)
allowed=[(151,372,74,32),(590,193,70,24),(590,328,70,24)]
pixels=ImageChops.difference(old,new).load()
outside=inside=0
for y in range(506):
    for x in range(718):
        if pixels[x,y] != (0,0,0):
            if any(a<=x<a+w and b<=y<b+h for a,b,w,h in allowed): inside+=1
            else: outside+=1
assert outside==0, f'{outside} pixels changed outside the three original readouts'
assert inside>0, 'font adjustment should be visible'
recovery=json.loads((root/'recovery/SOURCE_RECOVERY.json').read_text())
for path,value in recovery['files'].items():
    if path.startswith('assets/'):
        assert hashlib.sha256((root/path).read_bytes()).hexdigest()==value
original=(root/'recovery/original/Source/PluginEditor.cpp').read_text(encoding='utf-8')
current=(root/'Source/PluginEditor.cpp').read_text(encoding='utf-8')
def bounds(s):
    first=s.index('void YuayStutterAudioProcessorEditor::resized()')
    return s[first:s.index('// TIMER',first)]
original_bounds=bounds(original)
# The visible design stays fixed; expand only the invisible BYPASS hit target
# so its existing label and lamp are both clickable.
old_hit='341,\n    459,\n    30,\n    30'
new_hit='330,\n    455,\n    58,\n    42'
assert bounds(current).replace(new_hit,old_hit)==original_bounds
report=dict(original_size=list(old.size),changed_pixels_in_original_readouts=inside,
    changed_pixels_elsewhere=outside,artwork_hashes_unchanged=True,original_visible_control_bounds_unchanged=True,
    bypass_invisible_hit_target=[330,455,58,42])
(root/'reports/UI_COMPARISON.json').write_text(json.dumps(report,indent=2)+'\n',encoding='utf-8')
print('PASS: original artwork and bounds; zero pixel changes outside original three readouts')
