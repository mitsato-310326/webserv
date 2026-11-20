
## 2026-11-19 HTTPリクエストの構文・本文境界fixture作成

1. 変更ファイルと理由
   - `tests/generate.py`：Python3標準ライブラリで140件の生HTTPリクエストを再生成する定義。
   - `tests/fixtures/`：valid54、invalid46、incomplete15、policy16、unsupported5、streams4。実際のCRLFとバイナリ本文を保持。
   - `tests/manifest.json`：分類・日本語の目的・期待状態・本文・消費長・EOF時の扱い・ハッシュを記録。
   - `tests/README.md`：分類、ケース一覧、使い方、実パーサ未検証という限界を説明。
   - `tests/validate.py`：fixture/metadata/再生成/改変検出の整合検査。HTTPパーサの実装ではない。
   - `tests/.gitattributes`：fixtureの改行をGitが変換しない指定。
2. 実行したコマンド
   - `python3 tests/generate.py`、`python3 tests/validate.py`。
   - `PYTHONPYCACHEPREFIX=/private/tmp/http-fixtures-pycache python3 -m py_compile tests/generate.py tests/validate.py`。
   - `git status --short`、`find`、RFC9110/9112の一次資料確認。
3. テスト集計
   - データ整合8群 passed 8 / failed 0。Python構文検査 passed 1。
   - 最初のpy_compileはMacのキャッシュ出力先の権限制限で失敗。キャッシュを/private/tmpへ指定して再実行し成功。
   - 実パーサの動作テスト skipped 1（まだ実装がないため）。
4. 既知の限界
   - 全入力の数学的網羅ではない。validatorは期待値の独立したHTTP適合性証明ではない。
   - 分割受信は今後の実パーサ試験の指定のみ。splitデータ再結合の整合は確認したが、パーサへ投入していない。
   - パスの存在、デコード後のファイル解決、subject照合は対象外。
5. 残リスク
   - 方針依存の受理・拒否、上限値、未対応メソッド/版/転送符号化の通知契約はチーム方針と合わせる必要がある。
   - 既存inc/srcは変更していない。fixtureをエディタで改行変換すると検査は失敗する。
