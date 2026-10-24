"""Record unmodified recovered files before any feature edits."""
import hashlib, json
from pathlib import Path

root = Path(__file__).resolve().parents[1]
archive = Path('D:/JUICED/Yuay Stutter.rar')
def sha(path):
    h = hashlib.sha256()
    with path.open('rb') as f:
        for chunk in iter(lambda: f.read(1024 * 1024), b''):
            h.update(chunk)
    return h.hexdigest()

files = {}
for path in sorted((root / 'Source').glob('*')):
    original = Path('D:/JUICED/Yuay Stutter/Yuay Stutter/Source') / path.name
    assert sha(path) == sha(original), path
    files[path.relative_to(root).as_posix()] = sha(path)
for path in sorted((root / 'assets').glob('*')):
    files[path.relative_to(root).as_posix()] = sha(path)
record = dict(archive=str(archive), archive_sha256=sha(archive),
    source_origin='Source files extracted from the user supplied RAR; identical to the existing D: project.',
    version='1.0.0 (generated plugin defines and bundled moduleinfo); installer filename says 1.0.2',
    juce_version='8.0.12', manufacturer_code='Manu', plugin_code='YUAY',
    processor_cid='ABCDEF019182FAEB4D616E7559554159',
    controller_cid='ABCDEF011234ABCD4D616E7559554159',
    provenance_limit='Matching IDs and source identity do not prove a bit-for-bit rebuild of the macOS binary.',
    files=files)
(root / 'recovery' / 'SOURCE_RECOVERY.json').write_text(json.dumps(record, indent=2) + '\n')
