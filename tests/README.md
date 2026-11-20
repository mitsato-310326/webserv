# HTTPリクエストの構文・本文境界テストデータ

パーサ実装前の入力例と期待値。パスの存在、正規化、ファイルアクセスは対象外。
全入力の数学的な網羅ではなく、主要な境界・異常系を分類した例。
**HTTPパーサはまだ実装していないため、パーサの合格を示すテスト結果ではない。**

## 使用方法

リポジトリ直下から `python3 tests/validate.py` でデータ整合を確認。
`python3 tests/generate.py` で再生成。Python3標準ライブラリだけを使用。
`.http` は実際のCRLFと本文バイトを含む。エディタで改行変換しないこと。
NULや非UTF-8本文もあるため、テキストとしての表示だけでは内容を確認できない。

## 期待値の区別

- valid：構文と境界が正しい。未知メソッドも含むが、後段の501/405とは別。
- invalid：構文またはHTTP/1.1の本文境界・Host要件が不正。
- incomplete：まだ受信途中。EOF前は待つ。EOF時は不完全（空接続だけ通常終了）。
- policy：寛容な受理、重複の整理、設定上限、機能対応などで結果が変わる。
- unsupported：構文を読めることと、その機能を実行できることを分ける。OPTIONS/CONNECTは解析Complete、版・転送符号化の未対応はErrorと理由を返す設計案。
- streams：複数要求や次の要求の途中を連結。消費長と残りを検証。

CompleteはHeadersReady→設定判断→resumeを含む最終結果。Errorの検出時点は実装により異なる。
policyの具体的な採否は、manifestのdescriptionと設計書を照合して確定する。
GET/DELETE本文の例は、アプリが本文を使用する推奨ではなく受信境界の試験。
empty Host、obs-fold、重複CL等を単純に全て構文違反とはしない。

## 分割受信

manifestのfragmentationを使い、選択した正常入力を全位置で2分割し、さらに1バイトずつ渡す。
空のfeedだけでEOFを意味しない。EOFは別イベントとして通知する。
chunkedのチャンク境界とrecvの境界は別。本文中のGETやCRLFを新規要求と誤認しない。

## 集計

- incomplete: 15件
- invalid: 46件
- policy: 16件
- streams: 4件
- unsupported: 5件
- valid: 54件

## ケース一覧

|分類|ファイル|目的|
|---|---|---|
|valid|[method-get](fixtures/valid/method-get.http)|メソッド GET はtokenとして正しい。処理対応は別判定。|
|valid|[method-post](fixtures/valid/method-post.http)|メソッド POST はtokenとして正しい。処理対応は別判定。|
|valid|[method-delete](fixtures/valid/method-delete.http)|メソッド DELETE はtokenとして正しい。処理対応は別判定。|
|valid|[method-put](fixtures/valid/method-put.http)|メソッド PUT はtokenとして正しい。処理対応は別判定。|
|valid|[method-head](fixtures/valid/method-head.http)|メソッド HEAD はtokenとして正しい。処理対応は別判定。|
|valid|[method-patch](fixtures/valid/method-patch.http)|メソッド PATCH はtokenとして正しい。処理対応は別判定。|
|valid|[method-options](fixtures/valid/method-options.http)|メソッド OPTIONS はtokenとして正しい。処理対応は別判定。|
|valid|[method-get-lower](fixtures/valid/method-get-lower.http)|メソッド get はtokenとして正しい。処理対応は別判定。|
|valid|[method-frob](fixtures/valid/method-frob.http)|メソッド FROB はtokenとして正しい。処理対応は別判定。|
|valid|[method-token-symbols](fixtures/valid/method-token-symbols.http)|メソッド M!#$%&'*+-.^_`&#124;~09 はtokenとして正しい。処理対応は別判定。|
|valid|[target-root](fixtures/valid/target-root.http)|要求先の構文例。ファイル存在・復号・公開範囲は検証対象外。|
|valid|[target-path](fixtures/valid/target-path.http)|要求先の構文例。ファイル存在・復号・公開範囲は検証対象外。|
|valid|[target-query](fixtures/valid/target-query.http)|要求先の構文例。ファイル存在・復号・公開範囲は検証対象外。|
|valid|[target-empty-query](fixtures/valid/target-empty-query.http)|要求先の構文例。ファイル存在・復号・公開範囲は検証対象外。|
|valid|[target-duplicate-query](fixtures/valid/target-duplicate-query.http)|要求先の構文例。ファイル存在・復号・公開範囲は検証対象外。|
|valid|[target-encoded-question](fixtures/valid/target-encoded-question.http)|要求先の構文例。ファイル存在・復号・公開範囲は検証対象外。|
|valid|[target-encoded-hash](fixtures/valid/target-encoded-hash.http)|要求先の構文例。ファイル存在・復号・公開範囲は検証対象外。|
|valid|[target-encoded-slash](fixtures/valid/target-encoded-slash.http)|要求先の構文例。ファイル存在・復号・公開範囲は検証対象外。|
|valid|[target-dot-segments](fixtures/valid/target-dot-segments.http)|要求先の構文例。ファイル存在・復号・公開範囲は検証対象外。|
|valid|[target-utf8-percent](fixtures/valid/target-utf8-percent.http)|要求先の構文例。ファイル存在・復号・公開範囲は検証対象外。|
|valid|[target-query-plus](fixtures/valid/target-query-plus.http)|要求先の構文例。ファイル存在・復号・公開範囲は検証対象外。|
|valid|[target-query-question](fixtures/valid/target-query-question.http)|要求先の構文例。ファイル存在・復号・公開範囲は検証対象外。|
|valid|[target-absolute](fixtures/valid/target-absolute.http)|要求先の構文例。ファイル存在・復号・公開範囲は検証対象外。|
|valid|[target-absolute-empty-path](fixtures/valid/target-absolute-empty-path.http)|要求先の構文例。ファイル存在・復号・公開範囲は検証対象外。|
|valid|[target-absolute-other-host](fixtures/valid/target-absolute-other-host.http)|要求先の構文例。ファイル存在・復号・公開範囲は検証対象外。|
|valid|[header-lowercase](fixtures/valid/header-lowercase.http)|ヘッダー正常例。値や重複を一律に破棄しない。|
|valid|[header-mixed-case](fixtures/valid/header-mixed-case.http)|ヘッダー正常例。値や重複を一律に破棄しない。|
|valid|[header-port](fixtures/valid/header-port.http)|ヘッダー正常例。値や重複を一律に破棄しない。|
|valid|[header-ipv4](fixtures/valid/header-ipv4.http)|ヘッダー正常例。値や重複を一律に破棄しない。|
|valid|[header-ipv6](fixtures/valid/header-ipv6.http)|ヘッダー正常例。値や重複を一律に破棄しない。|
|valid|[header-empty-value](fixtures/valid/header-empty-value.http)|ヘッダー正常例。値や重複を一律に破棄しない。|
|valid|[header-duplicate-unknown](fixtures/valid/header-duplicate-unknown.http)|ヘッダー正常例。値や重複を一律に破棄しない。|
|valid|[header-colon-value](fixtures/valid/header-colon-value.http)|ヘッダー正常例。値や重複を一律に破棄しない。|
|valid|[header-ows](fixtures/valid/header-ows.http)|ヘッダー正常例。値や重複を一律に破棄しない。|
|valid|[header-obs-text](fixtures/valid/header-obs-text.http)|ヘッダー正常例。値や重複を一律に破棄しない。|
|valid|[header-connection-close](fixtures/valid/header-connection-close.http)|ヘッダー正常例。値や重複を一律に破棄しない。|
|valid|[header-connection-tokens](fixtures/valid/header-connection-tokens.http)|ヘッダー正常例。値や重複を一律に破棄しない。|
|valid|[fixed-text](fixtures/valid/fixed-text.http)|本文はメソッドや内容ではなくContent-Lengthで区切る。|
|valid|[fixed-empty](fixtures/valid/fixed-empty.http)|本文はメソッドや内容ではなくContent-Lengthで区切る。|
|valid|[fixed-get-body](fixtures/valid/fixed-get-body.http)|本文はメソッドや内容ではなくContent-Lengthで区切る。|
|valid|[fixed-delete-body](fixtures/valid/fixed-delete-body.http)|本文はメソッドや内容ではなくContent-Lengthで区切る。|
|valid|[fixed-binary](fixtures/valid/fixed-binary.http)|本文はメソッドや内容ではなくContent-Lengthで区切る。|
|valid|[fixed-crlf](fixtures/valid/fixed-crlf.http)|本文はメソッドや内容ではなくContent-Lengthで区切る。|
|valid|[fixed-fake-request](fixtures/valid/fixed-fake-request.http)|本文はメソッドや内容ではなくContent-Lengthで区切る。|
|valid|[cl-leading-zero](fixtures/valid/cl-leading-zero.http)|先頭の0は数字文法として有効。|
|valid|[chunk-empty](fixtures/valid/chunk-empty.http)|chunkedを復号し、サイズ・区切りを本文へ含めない。|
|valid|[chunk-one](fixtures/valid/chunk-one.http)|chunkedを復号し、サイズ・区切りを本文へ含めない。|
|valid|[chunk-multiple](fixtures/valid/chunk-multiple.http)|chunkedを復号し、サイズ・区切りを本文へ含めない。|
|valid|[chunk-hex-upper](fixtures/valid/chunk-hex-upper.http)|chunkedを復号し、サイズ・区切りを本文へ含めない。|
|valid|[chunk-hex-lower](fixtures/valid/chunk-hex-lower.http)|chunkedを復号し、サイズ・区切りを本文へ含めない。|
|valid|[chunk-binary](fixtures/valid/chunk-binary.http)|chunkedを復号し、サイズ・区切りを本文へ含めない。|
|valid|[chunk-extension](fixtures/valid/chunk-extension.http)|chunkedを復号し、サイズ・区切りを本文へ含めない。|
|valid|[chunk-quoted-extension](fixtures/valid/chunk-quoted-extension.http)|chunkedを復号し、サイズ・区切りを本文へ含めない。|
|valid|[minor-future](fixtures/valid/minor-future.http)|上位minor版は1.1の機能範囲で扱う設計案。|
|invalid|[line-missing-method](fixtures/invalid/line-missing-method.http)|リクエスト行の構文違反。|
|invalid|[line-missing-target](fixtures/invalid/line-missing-target.http)|リクエスト行の構文違反。|
|invalid|[line-missing-version](fixtures/invalid/line-missing-version.http)|リクエスト行の構文違反。|
|invalid|[line-extra-item](fixtures/invalid/line-extra-item.http)|リクエスト行の構文違反。|
|invalid|[line-method-paren](fixtures/invalid/line-method-paren.http)|リクエスト行の構文違反。|
|invalid|[line-method-nul](fixtures/invalid/line-method-nul.http)|リクエスト行の構文違反。|
|invalid|[line-version-text](fixtures/invalid/line-version-text.http)|リクエスト行の構文違反。|
|invalid|[line-version-no-dot](fixtures/invalid/line-version-no-dot.http)|リクエスト行の構文違反。|
|invalid|[line-version-two-digit](fixtures/invalid/line-version-two-digit.http)|リクエスト行の構文違反。|
|invalid|[line-version-lowercase](fixtures/invalid/line-version-lowercase.http)|リクエスト行の構文違反。|
|invalid|[line-fragment](fixtures/invalid/line-fragment.http)|リクエスト行の構文違反。|
|invalid|[line-relative-target](fixtures/invalid/line-relative-target.http)|リクエスト行の構文違反。|
|invalid|[line-bad-percent](fixtures/invalid/line-bad-percent.http)|リクエスト行の構文違反。|
|invalid|[line-short-percent](fixtures/invalid/line-short-percent.http)|リクエスト行の構文違反。|
|invalid|[header-no-colon](fixtures/invalid/header-no-colon.http)|ヘッダー行の構文違反。|
|invalid|[header-empty-name](fixtures/invalid/header-empty-name.http)|ヘッダー行の構文違反。|
|invalid|[header-space-name](fixtures/invalid/header-space-name.http)|ヘッダー行の構文違反。|
|invalid|[header-space-before-colon](fixtures/invalid/header-space-before-colon.http)|ヘッダー行の構文違反。|
|invalid|[header-tab-before-colon](fixtures/invalid/header-tab-before-colon.http)|ヘッダー行の構文違反。|
|invalid|[header-nul-value](fixtures/invalid/header-nul-value.http)|ヘッダー行の構文違反。|
|invalid|[header-del-value](fixtures/invalid/header-del-value.http)|ヘッダー行の構文違反。|
|invalid|[header-control-value](fixtures/invalid/header-control-value.http)|ヘッダー行の構文違反。|
|invalid|[header-name-paren](fixtures/invalid/header-name-paren.http)|ヘッダー行の構文違反。|
|invalid|[missing-host](fixtures/invalid/missing-host.http)|HTTP/1.1のHost要件違反。|
|invalid|[duplicate-host](fixtures/invalid/duplicate-host.http)|HTTP/1.1のHost要件違反。|
|invalid|[host-space](fixtures/invalid/host-space.http)|HTTP/1.1のHost要件違反。|
|invalid|[host-unclosed-ipv6](fixtures/invalid/host-unclosed-ipv6.http)|HTTP/1.1のHost要件違反。|
|invalid|[host-bad-port](fixtures/invalid/host-bad-port.http)|HTTP/1.1のHost要件違反。|
|invalid|[cl-negative](fixtures/invalid/cl-negative.http)|Content-Lengthは1桁以上のASCII数字。|
|invalid|[cl-positive-sign](fixtures/invalid/cl-positive-sign.http)|Content-Lengthは1桁以上のASCII数字。|
|invalid|[cl-alpha](fixtures/invalid/cl-alpha.http)|Content-Lengthは1桁以上のASCII数字。|
|invalid|[cl-suffix](fixtures/invalid/cl-suffix.http)|Content-Lengthは1桁以上のASCII数字。|
|invalid|[cl-inner-space](fixtures/invalid/cl-inner-space.http)|Content-Lengthは1桁以上のASCII数字。|
|invalid|[cl-empty](fixtures/invalid/cl-empty.http)|Content-Lengthは1桁以上のASCII数字。|
|invalid|[cl-conflict](fixtures/invalid/cl-conflict.http)|異なるContent-Lengthは不正な本文境界。|
|invalid|[te-repeated](fixtures/invalid/te-repeated.http)|不正なTransfer-Encodingまたはリクエストの本文境界。|
|invalid|[te-not-final](fixtures/invalid/te-not-final.http)|不正なTransfer-Encodingまたはリクエストの本文境界。|
|invalid|[te-missing-final](fixtures/invalid/te-missing-final.http)|不正なTransfer-Encodingまたはリクエストの本文境界。|
|invalid|[te-empty](fixtures/invalid/te-empty.http)|不正なTransfer-Encodingまたはリクエストの本文境界。|
|invalid|[chunk-not-hex](fixtures/invalid/chunk-not-hex.http)|チャンクのサイズ・拡張・区切り・トレーラー文法の違反。|
|invalid|[chunk-negative](fixtures/invalid/chunk-negative.http)|チャンクのサイズ・拡張・区切り・トレーラー文法の違反。|
|invalid|[chunk-plus](fixtures/invalid/chunk-plus.http)|チャンクのサイズ・拡張・区切り・トレーラー文法の違反。|
|invalid|[chunk-empty-size](fixtures/invalid/chunk-empty-size.http)|チャンクのサイズ・拡張・区切り・トレーラー文法の違反。|
|invalid|[chunk-bad-data-crlf](fixtures/invalid/chunk-bad-data-crlf.http)|チャンクのサイズ・拡張・区切り・トレーラー文法の違反。|
|invalid|[chunk-bad-extension](fixtures/invalid/chunk-bad-extension.http)|チャンクのサイズ・拡張・区切り・トレーラー文法の違反。|
|invalid|[chunk-trailer-no-colon](fixtures/invalid/chunk-trailer-no-colon.http)|チャンクのサイズ・拡張・区切り・トレーラー文法の違反。|
|incomplete|[partial-empty](fixtures/incomplete/partial-empty.http)|受信途中ではNeedMoreData。EOF後は不完全として扱う（空接続は通常終了）。|
|incomplete|[partial-method](fixtures/incomplete/partial-method.http)|受信途中ではNeedMoreData。EOF後は不完全として扱う（空接続は通常終了）。|
|incomplete|[partial-request-line](fixtures/incomplete/partial-request-line.http)|受信途中ではNeedMoreData。EOF後は不完全として扱う（空接続は通常終了）。|
|incomplete|[partial-line-cr](fixtures/incomplete/partial-line-cr.http)|受信途中ではNeedMoreData。EOF後は不完全として扱う（空接続は通常終了）。|
|incomplete|[partial-header-name](fixtures/incomplete/partial-header-name.http)|受信途中ではNeedMoreData。EOF後は不完全として扱う（空接続は通常終了）。|
|incomplete|[partial-header-value](fixtures/incomplete/partial-header-value.http)|受信途中ではNeedMoreData。EOF後は不完全として扱う（空接続は通常終了）。|
|incomplete|[partial-header-end](fixtures/incomplete/partial-header-end.http)|受信途中ではNeedMoreData。EOF後は不完全として扱う（空接続は通常終了）。|
|incomplete|[partial-fixed-body](fixtures/incomplete/partial-fixed-body.http)|受信途中ではNeedMoreData。EOF後は不完全として扱う（空接続は通常終了）。|
|incomplete|[partial-chunk-size](fixtures/incomplete/partial-chunk-size.http)|受信途中ではNeedMoreData。EOF後は不完全として扱う（空接続は通常終了）。|
|incomplete|[partial-chunk-size-cr](fixtures/incomplete/partial-chunk-size-cr.http)|受信途中ではNeedMoreData。EOF後は不完全として扱う（空接続は通常終了）。|
|incomplete|[partial-chunk-data](fixtures/incomplete/partial-chunk-data.http)|受信途中ではNeedMoreData。EOF後は不完全として扱う（空接続は通常終了）。|
|incomplete|[partial-chunk-data-cr](fixtures/incomplete/partial-chunk-data-cr.http)|受信途中ではNeedMoreData。EOF後は不完全として扱う（空接続は通常終了）。|
|incomplete|[partial-chunk-next-size](fixtures/incomplete/partial-chunk-next-size.http)|受信途中ではNeedMoreData。EOF後は不完全として扱う（空接続は通常終了）。|
|incomplete|[partial-chunk-final](fixtures/incomplete/partial-chunk-final.http)|受信途中ではNeedMoreData。EOF後は不完全として扱う（空接続は通常終了）。|
|incomplete|[partial-trailer](fixtures/incomplete/partial-trailer.http)|受信途中ではNeedMoreData。EOF後は不完全として扱う（空接続は通常終了）。|
|policy|[lf-only](fixtures/policy/lf-only.http)|LFのみの行末は寛容に受理可能。設計案はCRLFを要求。|
|policy|[multiple-spaces](fixtures/policy/multiple-spaces.http)|連続SPの寛容な受理か厳密な拒否かを統一。|
|policy|[tab-separator](fixtures/policy/tab-separator.http)|リクエスト行の空白を寛容に扱うかの方針。|
|policy|[obs-fold](fixtures/policy/obs-fold.http)|旧式折返しは拒否またはSPへ置換。設計案は拒否。|
|policy|[cl-duplicate-equal](fixtures/policy/cl-duplicate-equal.http)|一致した重複CLは拒否か単一値へ整理。|
|policy|[cl-list-equal](fixtures/policy/cl-list-equal.http)|一致したCLリストは拒否か単一値へ整理。|
|policy|[te-and-cl](fixtures/policy/te-and-cl.http)|TEとCLの併存は拒否可能。設計案は400後切断。CL優先は禁止。|
|policy|[huge-cl](fixtures/policy/huge-cl.http)|数字文法は正しい。型・受入上限を超える値を安全に拒否。|
|policy|[huge-chunk](fixtures/policy/huge-chunk.http)|16進文法は正しい。型・累積本文上限で拒否。|
|policy|[chunk-parameter](fixtures/policy/chunk-parameter.http)|chunkedのパラメータは定義されずエラー扱い推奨。サイズ行拡張とは別。|
|policy|[trailer-unknown](fixtures/policy/trailer-unknown.http)|トレーラー文法は正しい。許可名の方針と保持方法を決める。|
|policy|[trailer-content-length](fixtures/policy/trailer-content-length.http)|本文境界を変えるトレーラー。ヘッダーへ混ぜず無視または拒否。|
|policy|[empty-host](fixtures/policy/empty-host.http)|URIにauthorityがない場合の空Hostは仕様上の扱いがある。設計案は拒否。|
|policy|[expect-continue](fixtures/policy/expect-continue.http)|本文送信前の100応答待ち。ヘッダー完成を通知し後段判断。|
|policy|[expect-unknown](fixtures/policy/expect-unknown.http)|ヘッダー構文は正しい。期待機能の未対応は417等を後段判断。|
|policy|[long-target](fixtures/policy/long-target.http)|ターゲット文法は正しい。暫定リクエスト行上限を超える例。|
|unsupported|[http-1-0](fixtures/unsupported/http-1-0.http)|HTTP/1.0形式。設計案では対応外。構文違反と分ける。|
|unsupported|[http-2-text-version](fixtures/unsupported/http-2-text-version.http)|version token文法は正しいが、HTTP/2の通信形式を実装する意味ではない。|
|unsupported|[options-asterisk](fixtures/unsupported/options-asterisk.http)|OPTIONSのasterisk-form。未実装メソッド判定は後段。|
|unsupported|[connect-authority](fixtures/unsupported/connect-authority.http)|CONNECTのauthority-form。トンネル実装は対象外。|
|unsupported|[gzip-chunked](fixtures/unsupported/gzip-chunked.http)|転送符号化リストとして正しいがgzip復号未実装。本文のgzip内容検証前に未対応通知。|
|streams|[two-get](fixtures/streams/two-get.http)|先頭1件の消費長だけ削除し、次の要求を残す。|
|streams|[get-post](fixtures/streams/get-post.http)|先頭1件の消費長だけ削除し、次の要求を残す。|
|streams|[partial-next](fixtures/streams/partial-next.http)|先頭1件の消費長だけ削除し、次の要求を残す。|
|streams|[no-cl-extra](fixtures/streams/no-cl-extra.http)|先頭1件の消費長だけ削除し、次の要求を残す。|

## 根拠と限界

- [RFC 9112](https://www.rfc-editor.org/rfc/rfc9112.html)：§3要求先、§5ヘッダー、§6本文境界、§7chunked、§8不完全メッセージ。
- [RFC 9110](https://www.rfc-editor.org/rfc/rfc9110.html)：§5.6.2 token、§8.6 Content-Length、§9メソッド。
- [RFC 3986](https://www.rfc-editor.org/rfc/rfc3986.html)：URI構文とパーセント表記。
validatorはファイル・期待値・再生成の整合確認であり、HTTP解釈の独立した正しさ証明ではない。
課題subject・チーム方針と照合してからパーサの実テストへ接続する。
