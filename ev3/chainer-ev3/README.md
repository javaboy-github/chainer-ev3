# このプログラムについて
このプログラムは、コンパイルして、ev3内で実行するプログラムです。Pythonから送られたデータをもとに機械を動かすので、アダプターみたい感じです。

# Pythonとの通信
そのために実機にあるようにコードで繋ぎます。以下の信号を送ると実行されます。

なお、:star:がついているものは、このフォークを作った人のオリジナル。

| 送信コード | 実行内容 |
|--|--|
|0|モーターの設定。続けて、モーターのポートと、モーターのタイプを送信|
|1|監視中のタスクを有効化|
|10|モーターを動かす。続けて左右のモーターのポート、スピード、旋回値を送信|
|100|センサーの設定。続けてセンサーのポート、センサーのタイプを送信|
|110|タッチセンサーの値を取得|続けてポートを送信。254,シリアル,結果,シリアルの順に返される|
|120|カラーセンサーの情報を取得。続けてポートを送信。254,シリアル,結果,シリアルの順に返される|
|200|モニターに描画。続けて、描画する行、100バイト以内(`\0`を忘れないで)の文字を送信。|
|210|ボタンが押されているか取得。続けてボタンを送信。254,シリアル,結果,シリアルの順に返される|
|220:star:|モーターを回転。続けてポート、方向、スピードを送信|
|221:star:|モーターの力を設定。続けて、ポート、スピードを送信|
|255|何もしない|
