# Engine X Configuration File Parser Tests

設定ファイルパーサーの実装前の入力例と期待値。設定ファイルの存在確認や権限検証は対象外。
主要な構文・ディレクティブ・値の検証を分類した例。

## 使用方法

テストスイートを実行：
```bash
./webserv $(find ./fixtures/config -type f -name "*.conf")
```

## 期待値の区別

- **valid**：構文が正しく、全ディレクティブが有効。正常系として期待。
- **invalid**：構文違反またはディレクティブ値が不正。エラーとして拒否。
- **edge-cases**：境界値・複雑な構成・ホワイトスペース処理など。正常系として受理。
- **policy**：大文字小文字の区別、複数値の取扱い、ポリシー依存など。設計時に判断。

## テストケース一覧

### Valid（正常な設定）

| ファイル | 目的 |
|---------|------|
| basic-server.conf | 最小限のサーバー設定 |
| with-location.conf | ロケーションブロック付き設定 |
| multiple-servers.conf | 複数サーバーの設定 |
| with-ssl.conf | SSL/TLS設定を含む |
| with-error-pages.conf | エラーページのカスタマイズ |
| with-cgi.conf | CGI処理の設定 |
| with-upload.conf | ファイルアップロード設定 |
| with-redirect.conf | リダイレクト設定 |

### Invalid（不正な設定）

| ファイル | 目的 |
|---------|------|
| missing-closing-brace.conf | 閉じ括弧の欠落 |
| missing-opening-brace.conf | 開き括弧の欠落 |
| missing-semicolon.conf | セミコロン終端の欠落 |
| invalid-port.conf | ポート番号が非数値 |
| port-out-of-range.conf | ポート番号が範囲外（70000） |
| negative-port.conf | ポート番号が負数 |
| empty-server.conf | サーバーブロックが空 |
| no-directive-value.conf | ディレクティブの値なし |
| location-without-path.conf | locationのパス指定なし |
| duplicate-listen.conf | listen重複（方針依存） |
| invalid-method.conf | 無効なHTTPメソッド |
| client-max-body-size-invalid.conf | 無効な数値単位 |
| invalid-error-code.conf | 無効なHTTPエラーコード |
| nested-location.conf | ネストされたlocationブロック |
| unknown-directive.conf | 不明なディレクティブ |

### Edge Cases（境界ケース）

| ファイル | 目的 |
|---------|------|
| boundary-port-min.conf | ポート番号の最小値（1） |
| boundary-port-max.conf | ポート番号の最大値（65535） |
| max-body-size-with-unit.conf | 単位付きサイズ値（10M） |
| whitespace-handling.conf | 複数スペースの処理 |
| tab-separation.conf | タブを使用した分離 |
| empty-location-block.conf | 空のlocationブロック |
| many-locations.conf | 複数のlocationブロック |
| long-directive-value.conf | 長いディレクティブ値 |
| special-chars-in-path.conf | パスに特殊文字を含む |
| ipv6-address.conf | IPv6アドレス指定 |

### Policy（ポリシー依存）

| ファイル | 目的 |
|---------|------|
| case-sensitive-directive.conf | ディレクティブの大文字小文字 |
| multiple-server-names.conf | 複数のserver_name値 |
| multiple-error-codes.conf | 複数のエラーコード指定 |
| redirect-code.conf | リダイレクトコード |
| multiple-allowed-methods.conf | 複数のHTTPメソッド指定 |

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

## 集計

- valid: 8件
- invalid: 15件
- edge-cases: 10件
- policy: 5件
- **total: 38件**
