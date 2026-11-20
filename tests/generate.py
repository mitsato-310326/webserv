#!/usr/bin/env python3
"""Generate byte-exact HTTP request fixtures; this is not a parser."""
import hashlib
import json
from pathlib import Path

ROOT = Path(__file__).resolve().parent
CASES = []
DEFAULT = [('Host', 'example.test')]

def request(method='GET', target='/', headers=None, body=b'', version='HTTP/1.1'):
    if headers is None:
        headers = DEFAULT
    line = f'{method} {target} {version}\r\n'.encode('ascii')
    return line + b''.join((k + ': ' + v + '\r\n').encode('latin1') for k, v in headers) + b'\r\n' + body

def add(category, name, data, description, body=None, **extra):
    status = {'valid':'Complete', 'invalid':'Error', 'incomplete':'NeedMoreData',
              'policy':'PolicyDependent', 'unsupported':'Error', 'streams':'Complete'}[category]
    item = dict(id=name, category=category, file=f'fixtures/{category}/{name}.http',
                description=description, expected_status=status,
                byte_length=len(data), sha256=hashlib.sha256(data).hexdigest(),
                header_terminator_present=b'\r\n\r\n' in data,
                basis='RFC 9112 §§2–8; RFC 9110 §§5,8,9')
    if category == 'incomplete':
        item['at_eof'] = 'Error' if data else 'CleanClose'
    if body is not None:
        item['body_hex'] = body.hex()
    if category == 'valid':
        item['total_consumed'] = len(data)
    item.update(extra)
    if item['expected_status'] == 'Complete' and 'total_consumed' not in item:
        item['total_consumed'] = len(data)
    CASES.append((item, data))

# 正常系。未知メソッドも文法が正しければvalidであり、処理可否は別。
for method in ['GET', 'POST', 'DELETE', 'PUT', 'HEAD', 'PATCH', 'OPTIONS', 'get', 'FROB', "M!#$%&'*+-.^_`|~09"]:
    add('valid', 'method-' + ('token-symbols' if method.startswith('M!') else method.lower() + ('-lower' if method == 'get' else '')),
        request(method=method), f'メソッド {method} はtokenとして正しい。処理対応は別判定。', body=b'', method=method,
        application_supported=method in ['GET','POST','DELETE'])
for name, target in [('root','/'),('path','/a/b.txt'),('query','/search?q=cat&page=2'),('empty-query','/?'),
                     ('duplicate-query','/?q=a&q=b'),('encoded-question','/a%3Fb?q=x'),('encoded-hash','/a%23b'),
                     ('encoded-slash','/a%2Fb'),('dot-segments','/a/../b'),('utf8-percent','/%E3%81%82'),
                     ('query-plus','/?q=a+b'),('query-question','/?q=a?b'),('absolute','http://example.test/a?q=b'),
                     ('absolute-empty-path','http://example.test'),('absolute-other-host','http://other.test/a')]:
    add('valid','target-'+name,request(target=target), '要求先の構文例。ファイル存在・復号・公開範囲は検証対象外。',body=b'',target=target)
for name, headers in [('lowercase',[('host','example.test')]),('mixed-case',[('hOsT','example.test')]),
                      ('port',[('Host','example.test:8080')]),('ipv4',[('Host','127.0.0.1')]),
                      ('ipv6',[('Host','[::1]:8080')]),('empty-value',DEFAULT+[('X-Empty','')]),
                      ('duplicate-unknown',DEFAULT+[('X-Item','a'),('X-Item','b')]),
                      ('colon-value',DEFAULT+[('X-Item','a:b:c')]),('ows',DEFAULT+[('X-Item','\t a \t')]),
                      ('obs-text',DEFAULT+[('X-Item','\x80\xff')]),
                      ('connection-close',DEFAULT+[('Connection','close')]),
                      ('connection-tokens',DEFAULT+[('Connection','keep-alive, X-Custom')])]:
    add('valid','header-'+name,request(headers=headers),'ヘッダー正常例。値や重複を一律に破棄しない。',body=b'')
for name, method, body in [('text','POST',b'hello'),('empty','POST',b''),('get-body','GET',b'abc'),
                         ('delete-body','DELETE',b'abc'),('binary','POST',b'\x00\xff\x80abc'),
                         ('crlf','POST',b'a\r\n\r\nb'),('fake-request','POST',b'GET / HTTP/1.1\r\nHost: x\r\n\r\n')]:
    add('valid','fixed-'+name,request(method=method,headers=DEFAULT+[('Content-Length',str(len(body)))],body=body),
        '本文はメソッドや内容ではなくContent-Lengthで区切る。',body=body, framing='content-length')
add('valid','cl-leading-zero',request(method='POST',headers=DEFAULT+[('Content-Length','0003')],body=b'abc'),'先頭の0は数字文法として有効。',body=b'abc',framing='content-length')
CH = DEFAULT + [('Transfer-Encoding','chunked')]
for name, wire, body in [('empty',b'0\r\n\r\n',b''),('one',b'5\r\nhello\r\n0\r\n\r\n',b'hello'),
                        ('multiple',b'2\r\nab\r\n3\r\ncde\r\n0\r\n\r\n',b'abcde'),
                        ('hex-upper',b'A\r\n0123456789\r\n0\r\n\r\n',b'0123456789'),
                        ('hex-lower',b'a\r\n0123456789\r\n0\r\n\r\n',b'0123456789'),
                        ('binary',b'3\r\n\x00\xff\n\r\n0\r\n\r\n',b'\x00\xff\n'),
                        ('extension',b'3;foo=bar\r\nabc\r\n0\r\n\r\n',b'abc'),
                        ('quoted-extension',b'3;foo="a;b"\r\nabc\r\n0\r\n\r\n',b'abc')]:
    add('valid','chunk-'+name,request(method='POST',headers=CH,body=wire),'chunkedを復号し、サイズ・区切りを本文へ含めない。',body=body,framing='chunked')
add('valid','minor-future',request(version='HTTP/1.2'),'上位minor版は1.1の機能範囲で扱う設計案。',body=b'')
# 確定した構文・フレーミング違反。
for name, line in [('missing-method',b' / HTTP/1.1'),('missing-target',b'GET  HTTP/1.1'),
                   ('missing-version',b'GET /'),('extra-item',b'GET / HTTP/1.1 extra'),
                   ('method-paren',b'GE(T / HTTP/1.1'),('method-nul',b'GE\x00T / HTTP/1.1'),
                   ('version-text',b'GET / HTTP/abc'),('version-no-dot',b'GET / HTTP/11'),
                   ('version-two-digit',b'GET / HTTP/1.10'),('version-lowercase',b'GET / http/1.1'),
                   ('fragment',b'GET /a#b HTTP/1.1'),('relative-target',b'GET abc HTTP/1.1'),
                   ('bad-percent',b'GET /a%GG HTTP/1.1'),('short-percent',b'GET /a%2 HTTP/1.1')]:
    add('invalid','line-'+name,line+b'\r\nHost: example.test\r\n\r\n','リクエスト行の構文違反。',http_status_hint=400)
for name, raw in [('no-colon',b'Bad Header'),('empty-name',b': value'),('space-name',b'X Bad: value'),
                  ('space-before-colon',b'Host : example.test'),('tab-before-colon',b'Host\t: example.test'),
                  ('nul-value',b'X-Test: a\x00b'),('del-value',b'X-Test: a\x7fb'),
                  ('control-value',b'X-Test: a\x01b'),('name-paren',b'X(Test: a')]:
    add('invalid','header-'+name,b'GET / HTTP/1.1\r\nHost: example.test\r\n'+raw+b'\r\n\r\n','ヘッダー行の構文違反。',http_status_hint=400)
for name, hs in [('missing-host',[]),('duplicate-host',DEFAULT+DEFAULT),('host-space',[('Host','bad host')]),
                 ('host-unclosed-ipv6',[('Host','[::1')]),('host-bad-port',[('Host','example.test:abc')])]:
    add('invalid',name,request(headers=hs),'HTTP/1.1のHost要件違反。',http_status_hint=400)
for name, value in [('negative','-1'),('positive-sign','+1'),('alpha','abc'),('suffix','12x'),('inner-space','1 2'),('empty','')]:
    add('invalid','cl-'+name,request(method='POST',headers=DEFAULT+[('Content-Length',value)]),'Content-Lengthは1桁以上のASCII数字。',http_status_hint=400)
add('invalid','cl-conflict',request(method='POST',headers=DEFAULT+[('Content-Length','1'),('Content-Length','2')],body=b'ab'),'異なるContent-Lengthは不正な本文境界。',http_status_hint=400)
for name, te in [('repeated','chunked, chunked'),('not-final','chunked, gzip'),('missing-final','gzip'),('empty',''),('empty-element','chunked,')]:
    # Empty list elements have tolerant list handling; classify separately below.
    if name == 'empty-element':
        continue
    add('invalid','te-'+name,request(method='POST',headers=DEFAULT+[('Transfer-Encoding',te)],body=b'0\r\n\r\n'),'不正なTransfer-Encodingまたはリクエストの本文境界。',http_status_hint=400)
for name, wire in [('not-hex',b'Z\r\nabc\r\n0\r\n\r\n'),('negative',b'-1\r\na\r\n0\r\n\r\n'),
                    ('plus',b'+1\r\na\r\n0\r\n\r\n'),('empty-size',b'\r\n0\r\n\r\n'),
                    ('bad-data-crlf',b'1\r\naXX0\r\n\r\n'),('bad-extension',b'1;=x\r\na\r\n0\r\n\r\n'),
                    ('trailer-no-colon',b'0\r\nBadTrailer\r\n\r\n')]:
    add('invalid','chunk-'+name,request(method='POST',headers=CH,body=wire),'チャンクのサイズ・拡張・区切り・トレーラー文法の違反。',http_status_hint=400)

# 受信途中。EOFまでは不正と断定しない。
for name, data in [('empty',b''),('method',b'GE'),('request-line',b'GET / HTTP/1.'),('line-cr',b'GET / HTTP/1.1\r'),
                   ('header-name',b'GET / HTTP/1.1\r\nHo'),('header-value',b'GET / HTTP/1.1\r\nHost: example'),
                   ('header-end',b'GET / HTTP/1.1\r\nHost: example.test\r\n\r'),
                   ('fixed-body',request(method='POST',headers=DEFAULT+[('Content-Length','5')],body=b'he')),
                   ('chunk-size',request(method='POST',headers=CH,body=b'A')),
                   ('chunk-size-cr',request(method='POST',headers=CH,body=b'A\r')),
                   ('chunk-data',request(method='POST',headers=CH,body=b'5\r\nhe')),
                   ('chunk-data-cr',request(method='POST',headers=CH,body=b'1\r\na\r')),
                   ('chunk-next-size',request(method='POST',headers=CH,body=b'1\r\na\r\n')),
                   ('chunk-final',request(method='POST',headers=CH,body=b'0\r\n')),
                   ('trailer',request(method='POST',headers=CH,body=b'0\r\nX-Note: a\r\n'))]:
    add('incomplete','partial-'+name,data,'受信途中ではNeedMoreData。EOF後は不完全として扱う（空接続は通常終了）。')

# HTTP仕様の許容・実装方針・上限で結論が変わるもの。
policies = [
 ('lf-only',b'GET / HTTP/1.1\nHost: example.test\n\n','LFのみの行末は寛容に受理可能。設計案はCRLFを要求。'),
 ('multiple-spaces',b'GET  / HTTP/1.1\r\nHost: example.test\r\n\r\n','連続SPの寛容な受理か厳密な拒否かを統一。'),
 ('tab-separator',b'GET\t/\tHTTP/1.1\r\nHost: example.test\r\n\r\n','リクエスト行の空白を寛容に扱うかの方針。'),
 ('obs-fold',request(headers=DEFAULT+[('X-Note','a\r\n b')]),'旧式折返しは拒否またはSPへ置換。設計案は拒否。'),
 ('cl-duplicate-equal',request(method='POST',headers=DEFAULT+[('Content-Length','3'),('Content-Length','3')],body=b'abc'),'一致した重複CLは拒否か単一値へ整理。'),
 ('cl-list-equal',request(method='POST',headers=DEFAULT+[('Content-Length','3, 3')],body=b'abc'),'一致したCLリストは拒否か単一値へ整理。'),
 ('te-and-cl',request(method='POST',headers=CH+[('Content-Length','3')],body=b'0\r\n\r\n'),'TEとCLの併存は拒否可能。設計案は400後切断。CL優先は禁止。'),
 ('huge-cl',request(method='POST',headers=DEFAULT+[('Content-Length','9'*80)]),'数字文法は正しい。型・受入上限を超える値を安全に拒否。'),
 ('huge-chunk',request(method='POST',headers=CH,body=b'F'*40+b'\r\n'),'16進文法は正しい。型・累積本文上限で拒否。'),
 ('chunk-parameter',request(method='POST',headers=DEFAULT+[('Transfer-Encoding','chunked;x=y')],body=b'0\r\n\r\n'),'chunkedのパラメータは定義されずエラー扱い推奨。サイズ行拡張とは別。'),
 ('trailer-unknown',request(method='POST',headers=CH,body=b'0\r\nX-Note: a\r\n\r\n'),'トレーラー文法は正しい。許可名の方針と保持方法を決める。'),
 ('trailer-content-length',request(method='POST',headers=CH,body=b'0\r\nContent-Length: 5\r\n\r\n'),'本文境界を変えるトレーラー。ヘッダーへ混ぜず無視または拒否。'),
 ('empty-host',request(headers=[('Host','')]),'URIにauthorityがない場合の空Hostは仕様上の扱いがある。設計案は拒否。'),
 ('expect-continue',request(method='POST',headers=DEFAULT+[('Content-Length','3'),('Expect','100-continue')]),'本文送信前の100応答待ち。ヘッダー完成を通知し後段判断。'),
 ('expect-unknown',request(headers=DEFAULT+[('Expect','something-new')]),'ヘッダー構文は正しい。期待機能の未対応は417等を後段判断。'),
 ('long-target',request(target='/'+'a'*9000),'ターゲット文法は正しい。暫定リクエスト行上限を超える例。'),
]
for name, data, desc in policies:
    add('policy',name,data,desc,design_policy='reject_or_handle_explicitly; see description')
for name, data, desc in [
 ('http-1-0',request(version='HTTP/1.0'),'HTTP/1.0形式。設計案では対応外。構文違反と分ける。'),
 ('http-2-text-version',request(version='HTTP/2.0'),'version token文法は正しいが、HTTP/2の通信形式を実装する意味ではない。'),
 ('options-asterisk',request(method='OPTIONS',target='*'),'OPTIONSのasterisk-form。未実装メソッド判定は後段。'),
 ('connect-authority',request(method='CONNECT',target='example.test:443'),'CONNECTのauthority-form。トンネル実装は対象外。'),
 ('gzip-chunked',request(method='POST',headers=DEFAULT+[('Transfer-Encoding','gzip, chunked')],body=b'0\r\n\r\n'),'転送符号化リストとして正しいがgzip復号未実装。本文のgzip内容検証前に未対応通知。')]:
    add('unsupported',name,data,desc,application_supported=False,
        expected_status='Complete' if name in ['options-asterisk','connect-authority'] else 'Error',
        parser_error_reason=None if name in ['options-asterisk','connect-authority'] else ('UnsupportedTransferCoding' if name == 'gzip-chunked' else 'UnsupportedVersion'))

first=request(); second=request(method='POST',headers=DEFAULT+[('Content-Length','3')],body=b'abc')
for name, data, lengths, suffix in [('two-get',first+first,[len(first),len(first)],b''),
                                    ('get-post',first+second,[len(first),len(second)],b''),
                                    ('partial-next',first+b'GET /ne',[len(first)],b'GET /ne'),
                                    ('no-cl-extra',request(method='POST')+first,[len(request(method='POST')),len(first)],b'')]:
    add('streams',name,data,'先頭1件の消費長だけ削除し、次の要求を残す。',request_lengths=lengths,suffix_hex=suffix.hex())


def main():
    ids = [c['id'] for c, _ in CASES]
    assert len(ids) == len(set(ids)), 'duplicate id'
    for item, data in CASES:
        dest = ROOT / item['file']
        dest.parent.mkdir(parents=True, exist_ok=True)
        dest.write_bytes(data)
    manifest = dict(schema_version=1, scope='HTTP/1.1 request syntax/framing; no path resolution',
                    note='Expected statuses describe full parsing after HeadersReady/resume, not a single parse call.',
                    fragmentation=dict(strategy='For complete valid fixtures, test every two-part split including empty parts, and one byte per feed.',
                                       selected_ids=['fixed-binary','chunk-multiple','target-query']),
                    cases=[item for item, _ in CASES])
    (ROOT/'manifest.json').write_text(json.dumps(manifest,ensure_ascii=False,indent=2)+'\n')
    counts = {c:sum(i['category']==c for i,_ in CASES) for c in sorted({i['category'] for i,_ in CASES})}
    lines=['# HTTPリクエストの構文・本文境界テストデータ','',
           'パーサ実装前の入力例と期待値。パスの存在、正規化、ファイルアクセスは対象外。',
           '全入力の数学的な網羅ではなく、主要な境界・異常系を分類した例。',
           '**HTTPパーサはまだ実装していないため、パーサの合格を示すテスト結果ではない。**','',
           '## 使用方法','', 'リポジトリ直下から `python3 tests/validate.py` でデータ整合を確認。',
           '`python3 tests/generate.py` で再生成。Python3標準ライブラリだけを使用。',
           '`.http` は実際のCRLFと本文バイトを含む。エディタで改行変換しないこと。',
           'NULや非UTF-8本文もあるため、テキストとしての表示だけでは内容を確認できない。','',
           '## 期待値の区別','',
           '- valid：構文と境界が正しい。未知メソッドも含むが、後段の501/405とは別。',
           '- invalid：構文またはHTTP/1.1の本文境界・Host要件が不正。',
           '- incomplete：まだ受信途中。EOF前は待つ。EOF時は不完全（空接続だけ通常終了）。',
           '- policy：寛容な受理、重複の整理、設定上限、機能対応などで結果が変わる。',
           '- unsupported：構文を読めることと、その機能を実行できることを分ける。OPTIONS/CONNECTは解析Complete、版・転送符号化の未対応はErrorと理由を返す設計案。',
           '- streams：複数要求や次の要求の途中を連結。消費長と残りを検証。','',
           'CompleteはHeadersReady→設定判断→resumeを含む最終結果。Errorの検出時点は実装により異なる。',
           'policyの具体的な採否は、manifestのdescriptionと設計書を照合して確定する。',
           'GET/DELETE本文の例は、アプリが本文を使用する推奨ではなく受信境界の試験。',
           'empty Host、obs-fold、重複CL等を単純に全て構文違反とはしない。','',
           '## 分割受信','',
           'manifestのfragmentationを使い、選択した正常入力を全位置で2分割し、さらに1バイトずつ渡す。',
           '空のfeedだけでEOFを意味しない。EOFは別イベントとして通知する。',
           'chunkedのチャンク境界とrecvの境界は別。本文中のGETやCRLFを新規要求と誤認しない。','',
           '## 集計','']
    lines += [f'- {c}: {n}件' for c,n in counts.items()]
    lines += ['', '## ケース一覧','', '|分類|ファイル|目的|','|---|---|---|']
    lines += [f"|{i['category']}|[{i['id']}]({i['file']})|{i['description'].replace('|','&#124;')}|" for i,_ in CASES]
    lines += ['', '## 根拠と限界','',
              '- [RFC 9112](https://www.rfc-editor.org/rfc/rfc9112.html)：§3要求先、§5ヘッダー、§6本文境界、§7chunked、§8不完全メッセージ。',
              '- [RFC 9110](https://www.rfc-editor.org/rfc/rfc9110.html)：§5.6.2 token、§8.6 Content-Length、§9メソッド。',
              '- [RFC 3986](https://www.rfc-editor.org/rfc/rfc3986.html)：URI構文とパーセント表記。',
              'validatorはファイル・期待値・再生成の整合確認であり、HTTP解釈の独立した正しさ証明ではない。',
              '課題subject・チーム方針と照合してからパーサの実テストへ接続する。','']
    (ROOT/'README.md').write_text('\n'.join(lines))
    print(f'generated {len(CASES)} fixtures: {counts}')

if __name__ == '__main__':
    main()
