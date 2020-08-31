**WioLTEと加速度センサでEdgeAIを体験しようハンズオン**
===============================================

　このハンズオンではWioLTE上でのニューラルネットワークの実装とSORACOM プラットフォームとの連携方法について学習します。

　ハンズオンの題材として加速度センサを利用した機械の振動監視システムを構築します。このアプリケーションは機械の利用状況の確認や、特定の状態の監視を想定しています。

　このハンズオンを通して以下の知識を獲得することを目標とします。

* ニューラルネットワークの基礎知識
* TensorFlwo Lite for microcontrollersを利用したEdge-AIの実装
* EdgeAIとSORACOMプラットフォームとのインテグレーション

### **ハンズオンを進めるにあたって必要な知識**

* WioLTEの基礎知識
* SORACOM プラットフォームの基礎知識

### **ハンズオンで実装するアプリケーション**

 加速度センサを利用した機械の振動監視システムを構築します。今回はデバイスとしてWioLTE JP Verを利用します。加速度センサを振動を監視したい機械に設置し、振動を計測します。今回は監視対象の機械としてWioLTE JP Verのキットに含まれるブザーを利用します。ブザーの音を振動として加速度センサで計測します。

　WioLTE JP Verの



## **ハンズオン**

ハンズオンは以下の４つの手順で進めます。

1.  ハードウェアのセットアップ
2.  教師データの収集
3.  AI学習
4.  推論プログラムの実行 
 
### **1. ハードウェアのセットアップ** 

**手順1. WioLTE必要な部品を取り付ける**

 * LTE通信用アンテナ
 * ボタン => D20
 * ブザー => D38
 * 加速度センサ => I2C

><img src="contents/hardware_setup.png" width="720">

**手順2. 加速度センサーとブザーの貼り合わせ**

ブザーの振動(音)が加速度センサーへ伝わるように、２つのセンサを貼り合わせ、テープで固定します。貼り合わせの際にはお互いの部品を傷つけないように、突起物を避けて貼り合わせします。

><img src="contents/sensor.jpg" width="720">



**手順3. SORACOM Air SIMの取り付け**

以下のページを参考にSIRACOM Air SIMをWioLTEへ取り付けます。

[WioLTEへのSORACOM Air SIMの取り付け方](https://soracom.github.io/jp-training/WioLTE/1/2-uptime.html)



### **2. 教師データの収集**

AIを学習させるためのデータを収集します。実際に、ブザーをON/OFFし、加速度センサの値を収集します。
<br><br>


**手順1. WioLTEへロガー用ファームウェアを書き込む**

データを収集するため、WioLTEへデータロガー用のファームウェアを書き込みます。このサンプルプログラムはWioLTEに接続した３軸加速度センサの値をシリアル通信で送信します。また、ボタンを押すことで、ブザーを鳴らすことができます。


```buzzer-acc-logger/buzzer-acc-logger.ino```をArduino IDEで開き、WioLTEへ書き込みを行います。


書き込み終了後、WioLTEのリセットボタンを押下し、Arduino IDEのシリアルプロッタを開き、加速度のデータが送られてきいるか、確認します。また、WioLTEに取り付けたボタンを押下して、ブザーがなった場合に加速度が変化することを確認します。

> <img src="contents/acc_sample.png" width="720">
<br><br>


**手順2. シリアルデータロガーの実行**

```tools/loggercsv.py```を実行して、シリアル通信で送られてくる加速度センサの値をCSV形式で保存します。

以下のコマンドを実行後、10秒後にファイルへの記録が開始されます。

```bash
python tools/loggercsv.py -p <シリアルポート名> -f <csvファイル名>
```
* 例

```bash
python tools/loggercsv.py -p /dev/cu.usbserialxxxxxx -f sample/own_train_data.csv

```
<br><br>


**手順3. データの収集**

ロギング開始後、３分ほどボタンを押さずに、加速度センサを様々な覚悟に変えながらデータを取得します。

３分経過後、ボタンを押してブザーを鳴らしながらデータを取得します。先ほどと同様に、加速度センサの向きを変えながらもういちど3分程度データを取得します。


十分な量のデータを取得した後に、```loggercsv.py```を停止させ、データの収集を完了します。


### **3. AIの学習**

収集したデータを元にAIの学習を行います。AIの学習はTensorFlowが搭載されたマシンで実行します。このハンズオンではGoogle Colaboratoryを利用して、AIの学習を実行します。

手順1. Google Colaboratoryの準備

以下のボタンを押下して、学習用のノートブックをGoogle Colaboratoryで開きます。Googleアカウントへのログインが求められる場合はログインしてください。

[![Open In Colab](https://colab.research.google.com/assets/colab-badge.svg)](https://colab.research.google.com/github/masato-ka/wiolte-ai-handson/blob/master/tools/ai_wiolte_train_pynb.ipynb)


### **4. 推論プログラムの実行**

### **5. SORACOM Harvestの設定**



## Contribution

修正点や機能追加の提案はGithub issueへの登録、またはプルリクエストの作成をお願いします。

## Licnese

MIT Lisence

## Author

masato-ka jp8uzv@gmail.com

