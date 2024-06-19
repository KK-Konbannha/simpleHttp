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
