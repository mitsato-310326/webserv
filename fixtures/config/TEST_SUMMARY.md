# Engine X Configuration Parser Test Suite

包括的なテストスイートが完成しました。HTTP リクエストパーサーのテストと同様の構造を採用しています。

## 📊 テスト統計

| カテゴリ | ケース数 | 説明 |
|---------|---------|------|
| **valid** | 8 | 正常な設定ファイル |
| **invalid** | 15 | 不正な設定ファイル |
| **edge-cases** | 10 | 境界値・複雑な構成 |
| **policy** | 5 | ポリシー依存ケース |
| **合計** | **38** | **全テストケース** |

## 🎯 テスト対象の主要ディレクティブ

### サーバーレベル
- `listen` - ポート指定
- `server_name` - サーバー名
- `root` - ドキュメントルート
- `index` - デフォルトファイル
- `error_page` - エラーページ設定
- `client_max_body_size` - 最大アップロードサイズ
- `ssl_certificate` / `ssl_certificate_key` - SSL設定
- `return` - リダイレクト

### ロケーションレベル
- `root` - パス固有のドキュメントルート
- `proxy_pass` - プロキシ設定
- `cgi_pass` - CGI処理
- `allowed_methods` - HTTPメソッド制限

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

### Valid Cases - 正常な設定（8件）

1. **basic-server.conf**
   - 最小限のサーバー設定
   - ディレクティブ: listen, server_name, root

2. **with-location.conf**
   - ロケーションブロック付き
   - 複数のロケーション定義と異なる設定

3. **multiple-servers.conf**
   - 複数のサーバーブロック
   - 異なるポートとドメイン

4. **with-ssl.conf**
   - SSL/TLS設定
   - 証明書パス指定

5. **with-error-pages.conf**
   - エラーページのカスタマイズ
   - 複数のエラーコード対応

6. **with-cgi.conf**
   - CGI処理の設定
   - 複数のスクリプトタイプ（.php, .py）

7. **with-upload.conf**
   - ファイルアップロード設定
   - クライアント最大本文サイズ制限
   - HTTPメソッド制限

8. **with-redirect.conf**
   - リダイレクト設定
   - return ディレクティブの使用

### Invalid Cases - 不正な設定（15件）

#### 構文エラー（6件）
- **missing-closing-brace.conf** - 閉じ括弧なし
- **missing-opening-brace.conf** - 開き括弧なし
- **missing-semicolon.conf** - セミコロンなし
- **no-directive-value.conf** - 値のないディレクティブ
- **location-without-path.conf** - location パス指定なし
- **unknown-directive.conf** - 不明なディレクティブ

#### 値のバリデーションエラー（5件）
- **invalid-port.conf** - 非数値ポート（abc）
- **port-out-of-range.conf** - 範囲外（70000）
- **negative-port.conf** - 負数ポート
- **invalid-method.conf** - 無効なHTTPメソッド
- **invalid-error-code.conf** - 無効なHTTPコード（999）
- **client-max-body-size-invalid.conf** - 無効なサイズ値

#### 構造エラー（4件）
- **empty-server.conf** - 空のサーバーブロック
- **duplicate-listen.conf** - listen の重複（方針依存）
- **nested-location.conf** - ネストされたlocationブロック

### Edge Cases - 境界値テスト（10件）

- **boundary-port-min.conf** - ポート 1（最小）
- **boundary-port-max.conf** - ポート 65535（最大）
- **max-body-size-with-unit.conf** - 単位付きサイズ（10M）
- **whitespace-handling.conf** - 複数スペース処理
- **tab-separation.conf** - タブ分離
- **empty-location-block.conf** - 空のlocationブロック
- **many-locations.conf** - 5つのロケーションブロック
- **long-directive-value.conf** - 長い値の処理
- **special-chars-in-path.conf** - パスの特殊文字
- **ipv6-address.conf** - IPv6アドレス指定

### Policy Cases - ポリシー依存（5件）

- **case-sensitive-directive.conf** - 大文字小文字（Server_Name）
- **multiple-server-names.conf** - 複数のserver_name値
- **multiple-error-codes.conf** - 複数のエラーコード指定
- **redirect-code.conf** - return ディレクティブのコード
- **multiple-allowed-methods.conf** - 複数のHTTPメソッド指定

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
   - Phase 1: 基本的な構文解析（valid cases）
   - Phase 2: エラーハンドリング（invalid cases）
   - Phase 3: 境界値処理（edge cases）
   - Phase 4: ポリシー判定（policy cases）

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

- [x] テストケース 38 個作成完了
- [x] README.md - テスト説明書
- [x] manifest.json - テストメタデータ
- [x] ConfigParser.hpp - インターフェース
- [x] ConfigParser.cpp - スケルトン実装
- [x] ConfigParserTest.cpp - テストハーネス
- [ ] 詳細な実装開始
- [ ] ユニットテスト実行
- [ ] 統合テスト実行
