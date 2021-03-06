# mineSweeper
講義で作成した"マインスイーパー"ゲームです.  
端末上で動くTUI(text user interface)ですが，基本操作はクリック，文字に着色もしています．  
ただし，ncursesで実装されているため，linux(Ubuntu)でしか動作しません．  

<img width="200" alt="ゲーム画面" src="./ゲーム画面/ゲーム中.png"> <img width="200" alt="ゲームオーバー" src="./ゲーム画面/ゲームオーバー.png"> <img width="200" alt="クリア" src="./ゲーム画面/ゲームクリア.png"> <img width="200" alt="オプション" src="./ゲーム画面/オプション.png"><img width="400" alt="エキスパート" src="./ゲーム画面/エキスパート.png">

# 説明
大学講義での演習課題の一環として作成したゲームです．  
講義ではC言語，Ubuntu環境，端末上で動作することが前提条件となっていました．  
制作は一人かつ外部，内部，テスト仕様書の作成も同時並行であり，課題の制作期間は3ヶ月．  
ですが，はじめに考えていたRPG風ダンジョンゲームの制作が難航したため，変更により実質1ヶ月で完成させています．  
仕様書や，構想初期のゲームは両方ともGithubで参照可能ですので，興味があればぜひ．  
 
# 仕様
* 仕様言語 C
* 動作OS Ubuntu

# 操作
ゲームの操作説明は`/仕様書`フォルダの外部仕様書に詳しいためここでは割愛  
 
# 創意工夫点
過去のWindowsOSに搭載されていたマインスイーパーの完全再現が目標であるため，  
見た目の形状や色はもちろん，端末で動作するTUIでありながらクリック動作やオプションウィンドウの展開も可能にしました．  
特にオプションウィンドウの展開で，TUIならではのゲーム画面を崩さず描画する工程に苦労しており，  
一度フローチャートに書いて工程を整理することで描画タイミングを調整し，難を逃れました．  
そのほか時間計測やランキング，難易度変更機能を加え，  
難易度変更時には，マスの大きさに沿って端末のウィンドウサイズを自動変更する機能も追加しました．  
 
# 製作者
* 永廣幸太郎  
* 大阪工業大学 情報科学研究科 情報科学専攻   
