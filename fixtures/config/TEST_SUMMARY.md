# Webserv Configuration Parser Test Suite

仕様書（en.subject.pdf）IV.3「Configuration file」に基づいた包括的なテストスイート。
NGINX風の設定ファイルパーサーの実装テスト用です。

## 📊 テスト統計（NGINX 1.31.9 検証後）

| カテゴリ | ケース数 | 説明 |
|---------|---------|------|
| **valid** | 13 | 正常な設定ファイル（仕様準拠、4ケース NGINX 検証済） |
| **invalid** | 13 | 不正な設定ファイル（エラー検出、1ケース NGINX 検証済） |
| **edge-cases** | 8 | 境界値・複雑な構成 |
| **policy** | 0 | ポリシー依存ケース（全て valid/invalid へ統合） |
| **合計** | **34** | **全テストケース** |

## 🎯 仕様に基づいたディレクティブ

### サーバーレベル（server ブロック内）
- **`listen`** - インターフェース:ポートペア（複数ポート対応）
- **`server_name`** - サーバー識別（複数値対応）
- **`root`** - ドキュメントルートパス
- **`index`** - デフォルトファイル（複数値対応）
- **`error_page`** - エラーページマッピング（複数コード対応）
- **`client_max_body_size`** - 最大本文サイズ（単位付き: M, K）

### ロケーションレベル（location ブロック内）
- **`allowed_methods`** - HTTPメソッド制限（GET, POST, PUT, DELETE, HEAD等）
- **`return`** - HTTPリダイレクト（ステータスコード＋URL）
- **`cgi_path`** - CGIプログラムパス（ファイル拡張子別）
- **`autoindex`** - ディレクトリリスティング（on/off）
- **`upload_store`** - ファイルアップロード保存場所

## 📋 テストの実行方法

### すべてのテストを実行
```bash
./webserv $(find ./fixtures/config -type f -name "*.conf")
```

### カテゴリごとに実行
```bash
# 正常系のみ
./webserv $(find ./fixtures/config/valid -type f -name "*.conf")

# 不正系のみ
./webserv $(find ./fixtures/config/invalid -type f -name "*.conf")

# 境界値テスト
./webserv $(find ./fixtures/config/edge-cases -type f -name "*.conf")

# ポリシーテスト
./webserv $(find ./fixtures/config/policy -type f -name "*.conf")
```

## 🧪 テストケース詳細

### Valid Cases - 正常な設定（13件）

#### 基本機能（9ケース）
1. **basic-server.conf**
   - 最小限の設定：listen, server_name, root
   - テスト: ポート指定、サーバー名、ドキュメントルート

2. **with-location-method.conf**
   - ロケーションブロックと allowed_methods
   - テスト: 複数のロケーション、メソッド制限

3. **with-error-pages.conf**
   - error_page ディレクティブ
   - テスト: 404、500系エラーコードのマッピング

4. **with-cgi.conf**
   - CGI設定（cgi_path）
   - テスト: PHP、Python等の拡張子別CGI

5. **with-redirect.conf**
   - return ディレクティブによるリダイレクト
   - テスト: リダイレクトコード、URL指定

6. **with-index.conf**
   - index ディレクティブ（複数ファイル）
   - テスト: デフォルトファイル指定

7. **with-upload.conf**
   - ファイルアップロード設定
   - テスト: upload_store、client_max_body_size

8. **multiple-servers.conf**
   - 複数ポート・複数サーバーブロック
   - テスト: 3つのサーバー設定（ポート80, 8080, 443）

9. **with-directory-listing.conf**
   - autoindex ディレクティブ
   - テスト: オン/オフの切り替え

#### NGINX 検証済み（4ケース）
10. **multiple-server-names.conf** ✅ NGINX検証済
    - 複数 server_name 値を1行指定
    - テスト: 複数ドメイン設定

11. **multiple-error-codes.conf** ✅ NGINX検証済
    - 複数の error_page ディレクティブ
    - テスト: 複数エラーコード対応

12. **return-redirect.conf** ✅ NGINX検証済
    - return ディレクティブのリダイレクトコード
    - テスト: 301/302 リダイレクト

13. **multiple-cgi-extensions.conf** ✅ NGINX検証済
    - 複数ファイル拡張子の CGI 設定
    - テスト: .php, .py, .sh 等の複数拡張子

### Invalid Cases - 不正な設定（13件）

#### 必須ディレクティブ欠落
- **missing-listen.conf** - 必須の listen ディレクティブ欠落

#### 構文エラー
- **missing-closing-brace.conf** - 閉じ括弧なし
- **missing-semicolon.conf** - セミコロン終端なし
- **no-directive-value.conf** - ディレクティブ値が空
- **location-without-path.conf** - location のパス指定なし

#### ポート指定エラー
- **invalid-port.conf** - 非数値ポート（abc）
- **port-out-of-range.conf** - 範囲外ポート（70000 > 65535）
- **negative-port.conf** - 負数ポート

#### ディレクティブ値エラー
- **invalid-method.conf** - 無効なHTTPメソッド指定
- **client-max-body-size-invalid.conf** - 無効なサイズ値
- **invalid-error-code.conf** - 無効なHTTPコード（999）
- **invalid-autoindex.conf** - autoindex の無効な値（on/off以外）

#### NGINX 検証済み
- **case-sensitive-directive.conf** ✅ NGINX検証済 - ディレクティブ名の大文字小文字（Server_Name → 拒否）

### Edge Cases - 境界値・複雑な構成（8件）

#### 境界値テスト
- **boundary-port-min.conf** - ポート 1（最小値）
- **boundary-port-max.conf** - ポート 65535（最大値）

#### 複雑な構成
- **max-body-size-with-unit.conf** - 単位付きサイズ（10M）
- **whitespace-handling.conf** - 複数スペース・タブの処理
- **many-locations.conf** - 5つのロケーションブロック
- **all-http-methods.conf** - 全HTTPメソッド指定（7個）
- **multiple-index-files.conf** - 複数デフォルトファイル

#### ネットワーク設定
- **ipv6-address.conf** - IPv6アドレス指定（[::1]:8080）

### Policy Cases - ポリシー依存（5件）

- **multiple-server-names.conf** - 1行での複数 server_name 値
- **multiple-error-codes.conf** - 複数の error_page ディレクティブ
- **return-redirect.conf** - return ディレクティブのリダイレクトコード
- **case-sensitive-directive.conf** - ディレクティブ名の大文字小文字（Server_Name）
- **multiple-cgi-extensions.conf** - 複数ファイル拡張子のCGI設定

## 📐 検証項目

### 1. 構文検証
- [ ] ブロック構造（`{}`）の対応
- [ ] セミコロン終端
- [ ] ホワイトスペース処理（複数スペース、タブ）
- [ ] コメント行（`#`）の処理

### 2. ディレクティブ検証
- [ ] 既知ディレクティブのリスト確認
- [ ] ディレクティブ値の型チェック
- [ ] 値の範囲検証

### 3. ブロック構造検証
- [ ] `server` ブロック内のディレクティブ
- [ ] `location` ブロック内のディレクティブ
- [ ] ブロックのネスト禁止（location内のlocation禁止）
- [ ] 必須ディレクティブの確認

### 4. 値の型検証

#### 数値型
- ポート番号：1 ～ 65535
- HTTPコード：400 ～ 599
- サイズ（単位付き）：数値＋単位（B, K, M, G）

#### 文字列型
- ホスト名：有効なDNS名形式
- パス：有効なファイルシステムパス
- メソッド：GET, POST, PUT, DELETE, HEAD, OPTIONS, PATCH

## 🚀 実装時の推奨事項

1. **段階的な実装**
   - Phase 1: 基本的な構文解析（基本 valid cases）
   - Phase 2: エラーハンドリング（invalid cases）
   - Phase 3: 境界値処理（edge cases）
   - Phase 4: NGINX 検証済み機能（NGINX valid/invalid cases）

2. **エラーメッセージ**
   - ファイル名と行番号を含める
   - エラーの種類を明確にする
   - 修正方法のヒントを提示

3. **テストの実行**
   - 各フェーズ完了後に該当カテゴリのテストを実行
   - 正常系と異常系を分離してテスト
   - 最後に統合テストを実施

## 📚 参考資料

- HTTP リクエストパーサーテスト構造（`fixtures/`）
- TestScript.hpp - テストフレームワーク
- manifest.json - テストメタデータ

## ✅ チェックリスト

- [x] テストケース 34 個作成完了（仕様ベース）
- [x] README.md - テスト説明書（仕様対応）
- [x] manifest.json - テストメタデータ
- [x] ConfigParser.hpp - インターフェース定義
- [x] ConfigParser.cpp - スケルトン実装
- [x] ConfigParserTest.cpp - テストハーネス
- [x] NGINX 検証（1.31.9）- policy ケース 5個全検証
  - [x] 4ケースが valid へ移動（NGINX で許容）
  - [x] 1ケースが invalid へ移動（NGINX で拒否）
- [ ] 詳細な実装開始
  - [ ] トークンナイザー（字句解析）
  - [ ] パーサー（構文解析）
  - [ ] バリデーター（値検証）
- [ ] ユニットテスト実行
- [ ] 統合テスト実行
- [ ] ストレステスト
