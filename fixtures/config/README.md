# Webserv Configuration File Parser Tests

NGINX風の設定ファイルパーサー実装のための包括的テストスイート。
仕様書（en.subject.pdf）IV.3「Configuration file」の要件に基づいています。

## 使用方法

テストスイートを実行：
```bash
./webserv $(find ./fixtures/config -type f -name "*.conf")
```

## 期待値の区別

- **valid**：構文が正しく、全ディレクティブが仕様で定義されている。正常系として受理。
- **invalid**：構文違反またはディレクティブ値が仕様に違反。エラーとして拒否。
- **edge-cases**：境界値・複数値・複雑な構成など。正常系として受理。
- **policy**：設計時に判断が必要な項目（大文字小文字、複数値など）。

## テストケース一覧（仕様ベース）

### Valid（正常な設定）- 9ケース

| ファイル | 目的 |
|---------|------|
| basic-server.conf | 最小限のサーバー設定（listen, server_name, root） |
| with-location-method.conf | ロケーションブロックと allowed_methods |
| with-error-pages.conf | error_page ディレクティブ |
| with-cgi.conf | CGI設定（cgi_path） |
| with-redirect.conf | リダイレクト設定（return） |
| with-index.conf | index ディレクティブ |
| with-upload.conf | ファイルアップロード設定（upload_store） |
| multiple-servers.conf | 複数ポートの複数サーバーブロック |
| with-directory-listing.conf | autoindex ディレクティブ |

### Invalid（不正な設定）- 12ケース

| ファイル | 目的 |
|---------|------|
| missing-listen.conf | 必須の listen ディレクティブ欠落 |
| missing-closing-brace.conf | 閉じ括弧の欠落 |
| invalid-port.conf | ポート番号が非数値 |
| port-out-of-range.conf | ポート番号が範囲外（70000 > 65535） |
| negative-port.conf | ポート番号が負数 |
| invalid-method.conf | 無効なHTTPメソッド |
| client-max-body-size-invalid.conf | 無効なサイズ値（単位なし） |
| invalid-error-code.conf | 無効なHTTPコード（999） |
| location-without-path.conf | location のパス指定なし |
| invalid-autoindex.conf | autoindex の無効な値 |
| no-directive-value.conf | ディレクティブの値なし |
| missing-semicolon.conf | セミコロン終端の欠落 |

### Edge Cases（境界値・複雑な構成）- 8ケース

| ファイル | 目的 |
|---------|------|
| boundary-port-min.conf | ポート番号の最小値（1） |
| boundary-port-max.conf | ポート番号の最大値（65535） |
| max-body-size-with-unit.conf | 単位付きサイズ値（10M） |
| whitespace-handling.conf | 複数スペース・タブの処理 |
| many-locations.conf | 複数のロケーションブロック（5個） |
| ipv6-address.conf | IPv6アドレス指定（[::1]:8080） |
| all-http-methods.conf | 全HTTPメソッド（GET, POST, PUT, DELETE, HEAD, OPTIONS, PATCH） |
| multiple-index-files.conf | 複数のデフォルトファイル指定 |

### Policy（ポリシー依存）- 5ケース

| ファイル | 目的 |
|---------|------|
| multiple-server-names.conf | 複数の server_name 値（1行指定） |
| multiple-error-codes.conf | 複数の error_page ディレクティブ |
| return-redirect.conf | return ディレクティブのリダイレクトコード |
| case-sensitive-directive.conf | ディレクティブの大文字小文字（Server_Name） |
| multiple-cgi-extensions.conf | 複数ファイル拡張子のCGI設定 |

## テストの実装方針

### 構文検証

1. **ブロック構造**：`server { ... }` の開き括弧・閉じ括弧の対応
2. **セミコロン終端**：ディレクティブの終端（`location` ブロック除外）
3. **ホワイトスペース**：複数スペース・タブの処理

### ディレクティブ検証

1. **既知ディレクティブ**：許可リストに基づく検証
2. **値の型検証**：
   - 数値：ポート（1-65535）、エラーコード（3桁）、サイズ（単位対応）
   - 文字列：ホスト名、パス、メソッド名
3. **値の範囲検証**：ポート、HTTPコード、サイズ上限

### ブロック構造検証

1. **server ブロック**：必須ディレクティブの確認
2. **location ブロック**：パス指定の確認、ネスト禁止
3. **ブロック入れ子**：許可されたネスト構造のみ

## 既知の限界

- ファイルの存在確認は対象外
- ディレクティブ値の意味的検証（パスの存在など）は対象外
- キャッシュコントロール、圧縮設定など高度なディレクティブは未含
- 設定の有効性の最終確認は実装時に設計書と合わせる必要あり

## 仕様要件との対応

### サーバーレベルディレクティブ
- `listen` - ポート:インターフェースペア（複数ポート対応）
- `server_name` - サーバー識別（複数値対応）
- `root` - ドキュメントルート
- `index` - デフォルトファイル（複数値対応）
- `error_page` - エラーページ設定
- `client_max_body_size` - 最大本文サイズ（単位対応）

### ロケーションレベルディレクティブ
- `allowed_methods` - HTTPメソッド制限（複数値対応）
- `return` - HTTP リダイレクト
- `cgi_path` - CGIプログラムパス（拡張子別）
- `autoindex` - ディレクトリリスティング
- `upload_store` - ファイルアップロード場所

## 集計

- valid: 9件
- invalid: 12件
- edge-cases: 8件
- policy: 5件
- **total: 34件**

## 仕様への準拠

このテストスイートは以下の仕様要件に基づいています：
- **en.subject.pdf** IV.3「Configuration file」
- NGINX風の設定ファイル形式
- 複数ポート・複数サーバーのサポート
- CGI、ファイルアップロード、リダイレクト対応
