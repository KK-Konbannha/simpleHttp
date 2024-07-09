## 概要

- サーバは全ての場合でraspberry pi 4 model b 4GBを使用する。
- localhostと有線, 無線(自宅LAN)の3パターンで実験を行う。
- keep-aliveありとなしの2パターンをそれぞれについて実験を行う。
- サーバ側は0(default), 1(select), 2(thread), 3(fork), 4(epoll)の5パターンについて実験を行う。
- 総接続回数は2から2の10乗までの2の累乗回数で、同時接続数も2から2の10乗までの2の累乗回数で行う。
- これをそれぞれの総接続、同時接続数に対して3回ずつ行ない平均を取る。
- ただし、同時接続数は総接続回数よりも小さい値とする。
- ベンチマークソフトにはabコマンドを用いる。
- スクリプトをrustで記述し、それを用いてabコマンドを実行しcsvファイルを作成する。
- pythonのmatplotlibを使用してcsvを加工しグラフを作成する。

### rustスクリプトの使用方法

```bash
./target/release/bench <output_file> <max_i> <max_j> <iterations> <ip> <port> <is_keep_alive>
```

## 予想

### サーバー側オプションによる影響

- 0(default)は同時接続に対応しないため、同時接続数が増えると接続が失敗すると予想される。
- 1(select)は同時接続に対応するが0を除くパターンの中で一番遅いと予想される。
- 2(thread)は同時接続に対応しforkよりも高速、selectよりも高速であると考えれるがepollと比べた場合は遅いと考えられるため全てのパターンの中で2番目に早いと予想される。
- 3(fork)は同時接続に対応し、selectよりも高速であると考えれるがepollと比べた場合は遅いと考えられるため全てのパターンの中で3番目に早いと予想される。
- 4(epoll)は同時接続に対応し、全てのパターンの中で一番早いと予想される。
- 2, 3についてはスレッドや子プロセスの最大数があるため、その数を超えると接続が失敗すると予想される。

### keep-aliveによる影響

- 総回数が少ない場合はkeep-aliveがボトルネックになり遅くなると予想される。
- 総回数が多い場合はkeep-aliveが有効になり、高速になると予想される。

### クライアントの接続方法による影響

- localhostが一番高速で、有線が次に高速、無線が一番遅いと予想される。

### 予想まとめ

- サーバ側は4 > 2 > 3 > 1 > 0の順で高速であると予想される。
- keep-aliveが有効な場合は総回数が多いほど高速であると予想される。
- クライアントの接続方法はlocalhost > 有線 > 無線の順で高速であると予想される。
- 以上の予想から、localhostでkeep-aliveありの、サーバ側が4(epoll)の場合が一番高速であると予想される。
- また、無線でkeep-aliveなしの、サーバ側が0(default)の場合が一番遅いと予想される。

## 1. localhost

- raspberry pi 4 model b 4GBで実験を行う。
- サーバ側は0(default), 1(select), 2(thread), 3(fork), 4(epoll)の5パターンについて実験を行う。
- クライアント側はlocalhostで実験を行う。

```bash
for i in {0..5}
do
./target/release/bench result/localhost/keep-alive/$i.csv 10 10 3 localhost 10000 true
./target/release/bench result/localhost/no-keep-alive/$i.csv 10 10 3 localhost 10000 false

check=n
while true
do
    if [ ${check} = "y" ];then
        break
    fi
    echo -n "continue? (y/n): " && read check
done
done
```

### 1.1. keep-aliveあり

#### 1.1.1. default

##### 1.1.1.1. Time taken for tests/総回数、同時接続数グラフ

##### 1.1.1.2. Time per request/総回数、同時接続数グラフ

##### 1.1.1.3. Failed requests/総回数、同時接続数グラフ

##### 1.1.1.4. まとめ

#### 1.1.2. select

#### 1.1.3. thread

#### 1.1.4. fork

#### 1.1.5 epoll

### 1.2. keep-aliveなし

## 2. 有線

- raspberry pi 4 model b 4GBをサーバとして、Let's note CF-FV arch linuxをクライアントとして実験を行う。
- サーバ側は0(default), 1(select), 2(thread), 3(fork), 4(epoll)の5パターンについて実験を行う。
- クライアントをサーバに直接有線で接続して実験を行う。

```bash
for i in {0..5}
do
./target/release/bench result/wired/keep-alive/$i.csv 10 10 3 192.168.1.101 10000 true
./target/release/bench result/wired/no-keep-alive/$i.csv 10 10 3 192.168.1.101 10000 false

check=n
while true
do
    if [ ${check} = "y" ];then
        break
    fi
    echo -n "continue? (y/n): " && read check
done
done
```

### 2.1. keep-aliveあり

### 2.2. keep-aliveなし

## 3. 無線(自宅LAN)

- raspberry pi 4 model b 4GBをサーバとして、Let's note CF-FV arch linuxをクライアントとして実験を行う。
- サーバ側は0(default), 1(select), 2(thread), 3(fork), 4(epoll)の5パターンについて実験を行う。
- クライアントをサーバに自宅の無線LANを介して接続して実験を行う。

```bash
for i in {0..5}
do
./target/release/bench result/wireless/keep-alive/$i.csv 10 10 3 192.168.2.101 10000 true
./target/release/bench result/wireless/no-keep-alive/$i.csv 10 10 3 192.168.2.101 10000 false

check=n
while true
do
    if [ ${check} = "y" ];then
        break
    fi
    echo -n "continue? (y/n): " && read check
done
done
```

### 3.1. keep-aliveあり

### 3.2. keep-aliveなし
