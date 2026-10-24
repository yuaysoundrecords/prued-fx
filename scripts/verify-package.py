"""Verify the delivered binary architecture, class IDs, ZIP and historical baseline."""
import hashlib, json, re, struct, zipfile
from pathlib import Path

root = Path(__file__).resolve().parents[1]
def sha(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()

baseline = json.loads((root / 'BASELINE.json').read_text())
assert sha(root / baseline['archive']) == baseline['archive_sha256']
delivery = root / 'delivery'
bundle = delivery / 'Prued FX.vst3'
binary = bundle / 'Contents/x86_64-win/Prued FX.vst3'
data = binary.read_bytes()
assert data[:2] == b'MZ'
pe = struct.unpack_from('<I', data, 0x3c)[0]
assert data[pe:pe+4] == b'PE\0\0'
assert struct.unpack_from('<H', data, pe+4)[0] == 0x8664
metadata = (bundle / 'Contents/Resources/moduleinfo.json').read_text()
assert '"Version": "2.0.0"' in metadata
cids = re.findall(r'"CID": "([A-F0-9]+)"', metadata)
assert cids == ['ABCDEF019182FAEB4D616E7559554159', 'ABCDEF011234ABCD4D616E7559554159']
archive = delivery / 'Prued-FX-2.0.0-Windows-x64-test.zip'
assert sha(archive)==sha(delivery/'Prued-FX-2.0.0-Windows-x64-Stutter-RC2.zip')
with zipfile.ZipFile(archive) as z:
    assert z.testzip() is None
    names = {name.replace('\\', '/'): name for name in z.namelist()}
    dll_name = 'Prued FX.vst3/Contents/x86_64-win/Prued FX.vst3'
    assert hashlib.sha256(z.read(names[dll_name])).hexdigest() == sha(binary)
    for name in ['TESTING-TH.md', 'ui-preview.png', 'VALIDATION.md']:
        assert name in names
test_log=(root/'reports/automated-tests.log').read_text()
passed=test_log.count('Test Passed.')
assert passed>=6 and 'Test Failed.' not in test_log
assert 'BypassAndKnobRegression' in test_log
assert 'actual VST3 host recognizes BYPASS' in test_log
record = dict(product='Prued FX', version='2.0.0', architecture='Windows x64',
    source_recovery_commit='c63d74c', branch='feature/prued-fx-2-performance-looper',
    juce='8.0.12', vst3_class_ids=cids,
    original_macos_baseline_sha256=baseline['archive_sha256'],
    package_sha256=sha(archive), package_bytes=archive.stat().st_size,
    binary_sha256=sha(binary), binary_bytes=len(data),
    automated_tests=f'{passed}/{passed} passed; reports/automated-tests.log',
    real_daw_listening_tested=False,
    files={p.relative_to(bundle).as_posix(): sha(p) for p in bundle.rglob('*') if p.is_file()})
(root / 'reports/BUILD_MANIFEST.json').write_text(json.dumps(record, indent=2)+'\n')
print('PASS: PE x64, version 2.0.0, original class IDs, ZIP integrity, binary hash, unchanged macOS baseline')
print('Package SHA-256:', record['package_sha256'])
