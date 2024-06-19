# SimpleHttp Server

これは、シンプルなHTTPサーバです。

## 使い方

```sh
$ ./bin/main 0 8080 0
```

./bin/main <モード> <ポート番号> <認証機能>

- モード: 0=シングルスレッド, 1=select, 2=マルチスレッド, 3=マルチプロセス, 4=ePoll (必須)
- ポート番号: 0~65535 (必須)
- 認証機能: 0=なし, 1=BASIC認証 (任意)

## リダイレクト

redirecting.csvにリダイレクト設定を記述することで、リダイレクトを行うことができます。

```csv
301,/moved,/new
```

## 拒否設定

rejection.csvに拒否設定を記述することで、アクセスを拒否することができます。

```csv
403,/forbidden
```

## 認証設定

.authに認証情報を記述することで、認証が可能です。

```txt
user: admin
pass: admin
```

## ファイル構成

```
.
├── .auth
├── Makefile
├── README.md
├── bin
│   └── main
├── include
├── obj
├── public
│   ├── done.html
│   ├── index.html
│   ├── new.html
│   ├── php
│   ├── static
│   │   ├── img
│   │   ├── js
│   │   └── video
│   ├── test.html
│   └── test_cgi.cgi
├── redirecting.csv
├── rejection.csv
└── src
    ├── accept_loop
    │   ├── default_loop.c
    │   ├── epoll_loop.c
    │   ├── process_loop.c
    │   ├── select_loop.c
    │   └── thread_loop.c
    ├── route_handler.c
    ├── http_session.c
    ├── lib.c
    ├── main.c
    ├── request_handler.c
    ├── router.c
    └── send_status.c
```

- .auth: 認証情報ファイル
- Makefile: ビルドスクリプト
- README.md: このファイル
- bin/main: 実行ファイル
- include/: ヘッダディレクトリ
- obj/: オブジェクトファイルディレクトリ
- public/: 公開ディレクトリ
- redirecting.csv: リダイレクト設定ファイル
- rejection.csv: 拒否設定ファイル
- src/: ソースディレクトリ
  - accept_loop/: アクセプトループの実装
  - route_handler.c: ルートハンドラ
  - http_session.c: HTTPセッション
  - lib.c: ライブラリ
  - main.c: メイン関数
  - request_handler.c: リクエストハンドラ
  - router.c: ルータ
  - send_status.c: ステータス送信

### 補足

- ルートハンドラは、ルート毎の具体的な処理を記述するファイルです。
- ルータは、リクエストのルーティングを行うファイルです。
- リクエストハンドラは、リクエスト毎の処理を記述するファイルです。
