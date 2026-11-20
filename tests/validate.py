#!/usr/bin/env python3
"""Check fixture metadata/reproducibility, not production parser behavior."""
import contextlib
import hashlib
import importlib.util
import io
import json
from pathlib import Path
import tempfile

ROOT = Path(__file__).resolve().parent

def check_file(root, item):
    data = (root/item['file']).read_bytes()
    assert len(data) == item['byte_length'], item['id'] + ': byte length'
    assert hashlib.sha256(data).hexdigest() == item['sha256'], item['id'] + ': sha256'
    assert (b'\r\n\r\n' in data) == item['header_terminator_present'], item['id'] + ': header delimiter'
    if 'body_hex' in item:
        body = bytes.fromhex(item['body_hex'])
        if item.get('framing') == 'content-length':
            head, actual = data.split(b'\r\n\r\n', 1)
            lengths = [x.split(b':',1)[1].strip() for x in head.split(b'\r\n')[1:]
                       if x.split(b':',1)[0].lower() == b'content-length']
            assert len(lengths) == 1 and int(lengths[0]) == len(body), item['id'] + ': CL metadata'
            assert actual == body, item['id'] + ': fixed body'
    if item['category'] == 'valid':
        assert item['total_consumed'] == len(data), item['id'] + ': consumed'
        assert b'\r\n\r\n' in data, item['id'] + ': valid delimiter'
        head = data.split(b'\r\n\r\n',1)[0]
        assert b'\n' not in head.replace(b'\r\n',b''), item['id'] + ': valid CRLF'
        assert b'\r' not in head.replace(b'\r\n',b''), item['id'] + ': valid CRLF'
    if item['category'] == 'streams':
        assert sum(item['request_lengths']) + len(bytes.fromhex(item['suffix_hex'])) == len(data), item['id'] + ': stream lengths'
        assert data.endswith(bytes.fromhex(item['suffix_hex'])), item['id'] + ': stream suffix'
    return data

def main():
    manifest = json.loads((ROOT/'manifest.json').read_text())
    cases = manifest['cases']
    ids = [x['id'] for x in cases]
    files = [x['file'] for x in cases]
    assert len(ids) == len(set(ids)) and len(files) == len(set(files))
    assert set(files) == {str(p.relative_to(ROOT)) for p in (ROOT/'fixtures').rglob('*.http')}
    allowed = {'valid':'Complete','invalid':'Error','incomplete':'NeedMoreData',
               'policy':'PolicyDependent','unsupported':'Error','streams':'Complete'}
    for item in cases:
        expected = 'Complete' if item['id'] in ['options-asterisk','connect-authority'] else allowed[item['category']]
        assert item['expected_status'] == expected, item['id']
        assert item['description'] and item['basis']
        check_file(ROOT,item)
        if item['category'] == 'incomplete':
            assert item['at_eof'] == ('CleanClose' if item['byte_length']==0 else 'Error')
    selected = manifest['fragmentation']['selected_ids']
    for identifier in selected:
        item = next(x for x in cases if x['id']==identifier)
        assert item['category'] == 'valid'
        raw = (ROOT/item['file']).read_bytes()
        for split in range(len(raw)+1):
            assert raw[:split]+raw[split:] == raw
        assert b''.join(raw[i:i+1] for i in range(len(raw))) == raw
    assert sum(x['byte_length'] for x in cases) < 10*1024*1024
    spec = importlib.util.spec_from_file_location('fixture_generator',ROOT/'generate.py')
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    with tempfile.TemporaryDirectory(prefix='http-fixtures-') as tmp:
        module.ROOT = Path(tmp)
        with contextlib.redirect_stdout(io.StringIO()):
            module.main()
        for name in files+['manifest.json','README.md']:
            assert (ROOT/name).read_bytes() == (Path(tmp)/name).read_bytes(), name + ': reproducibility'
        chosen = cases[0]
        dest = Path(tmp)/chosen['file']
        dest.write_bytes(dest.read_bytes()+b'X')
        try:
            check_file(Path(tmp),chosen)
        except AssertionError:
            pass
        else:
            raise AssertionError('corrupted fixture was not detected')
    print(f'PASS: {len(cases)} fixtures; 8 check groups (inventory, category, hash/length, body/stream, EOF, split metadata, reproducibility, corruption detection)')
    print('Production HTTP parser tests: NOT RUN (parser not implemented).')

if __name__ == '__main__':
    main()
