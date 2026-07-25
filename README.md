## Knight of Jail

https://www.youtube.com/watch?v=z3aof6jWfaI
[![Knight of Jail プレイ映像](https://img.youtube.com/vi/z3aof6jWfaI/maxresdefault.jpg)](https://www.youtube.com/watch?v=z3aof6jWfaI)

<img width="2559" height="1439" alt="スクリーンショット 2026-07-25 215831" src="https://github.com/user-attachments/assets/34ed1c71-2b2c-4040-be4a-5d82036a2fba" />

<img width="2559" height="1439" alt="スクリーンショット 2026-07-25 215920" src="https://github.com/user-attachments/assets/74bbfc08-944e-4795-b60a-e799eb0bd861" />




### ゲームについて
#### URL
https://kir-thread.site/storage/KirThread_KnightOfJail.zip

※クリックすると zip ファイルのインストールが始まります。（約1.44GB）

#### 操作方法
```
【ゲーム中】
[W][A][S][D]: 移動
[左クリック]: 攻撃
[Space]: ジャンプ
[左Shift] : 回避 / 長押しでダッシュ
[ESC]: メニュー画面を開く

【タイトル・メニュー画面】
[左クリック]選択
[右クリック][ESC] : 戻る
```

#### ゲームの説明
キーボードとマウスで遊ぶ 3D アクションゲームです。  
囚われてしまった騎士が敵を倒して牢獄の脱出を目指します。

### 開発環境等
* 製作時間：250 時間ほど
* Unreal Engine Ver：5.5
* ジャンル：3D アクション


### 作成の目的
以下の理解を目的としました。
* Unreal Engine を用いたゲームの設計手順
* 3D ゲームに必要となる技術概念（リギング、アニメーション等）
* C++(Unreal C++)の習熟
* Enhanced Input を用いたプレイヤーのアクション管理
* Blueprint ノードを用いたゲームイベント実装
* Level Sequence を用いたムービーイベント実装
* Behavior Tree を用いた NPC 用の AI 実装
* Geometry Collection を用いた Mesh モデルの破砕処理
* 非同期セーブ/ロード実装


### 考慮した部分など
#### Interface
インターフェースを用いた共通化処理の実装
* インタラクト関連
  * https://github.com/skonishi1125/ue5_linear_dungeon/blob/main/Source/LinearDungeon/Public/Interfaces/InteractInterface.h
  * https://github.com/skonishi1125/ue5_linear_dungeon/blob/8638f9e1c0dd7ce266224a9283423a13dcc566d2/Source/LinearDungeon/Private/Characters/LinearPlayerCharacter.cpp#L588-L611
  * 扉の開閉、装備、NPC との会話等
* ダメージなど、被弾
  * https://github.com/skonishi1125/ue5_linear_dungeon/blob/main/Source/LinearDungeon/Public/Interfaces/HitInterface.h
  * 武器や敵の攻撃判定が触れたとき、各 Actor 別の処理を呼ぶ
    * https://github.com/skonishi1125/ue5_linear_dungeon/blob/8638f9e1c0dd7ce266224a9283423a13dcc566d2/Source/LinearDungeon/Private/Items/Weapon.cpp#L120-L124
    * https://github.com/skonishi1125/ue5_linear_dungeon/blob/8638f9e1c0dd7ce266224a9283423a13dcc566d2/Source/LinearDungeon/Private/Enemies/EnemyBase.cpp#L511-L514
  * 対象  
    * プレイヤー
      * https://github.com/skonishi1125/ue5_linear_dungeon/blob/main/Source/LinearDungeon/Private/Characters/LinearPlayerCharacter.cpp#L908-L1010
    * 敵
      * https://github.com/skonishi1125/ue5_linear_dungeon/blob/8638f9e1c0dd7ce266224a9283423a13dcc566d2/Source/LinearDungeon/Private/Enemies/EnemyBase.cpp#L713-L789
    * Geometry Collection で用意した、壊れる小道具等（Blueprint 側で実装）

#### Behavior Tree
敵の State を Enum で管理することで、シンプルな形で敵 AI を設計できるようにした。
* https://github.com/skonishi1125/ue5_linear_dungeon/blob/main/Source/LinearDungeon/Private/BT/BTService_UpdateEnemyAIState.cpp
* <img width="4019" height="1568" alt="BT_EnemyBase_v2-Behavior Tree_0" src="https://github.com/user-attachments/assets/02fa6c0e-bf3c-4f7e-ae27-33bfed4475d6" />

#### Level Sequence
透明な判定を持ったトリガーを Blueprint で用意することで、特定の地点にキャラクターが触れるときにイベントを再生できるようにした。  
また、一度終わったイベントは Subsystem に登録し、同じセーブデータで複数回再生されないような設計とした。
* https://github.com/skonishi1125/ue5_linear_dungeon/blob/main/Source/LinearDungeon/Private/Subsystems/LinearEventSubsystem.cpp
* https://github.com/skonishi1125/ue5_linear_dungeon/blob/main/Source/LinearDungeon/Private/Subsystems/LinearSaveSubsystem.cpp
* <img width="6803" height="2151" alt="BP_CinematicTrigger-EventGraph" src="https://github.com/user-attachments/assets/b6b38ef9-d507-4f66-ba77-23bc43aadb89" />

#### Animation Blueprint
Idle, Walk の State を Main State として定義。  
そちらをベースに、ポーションを飲む挙動や両手武器用の補正用 IK, 攻撃用の Animation Montage を挟んで、適宜調整できるようにした。
<img width="2091" height="978" alt="ABP_CPaladin-AnimGraph" src="https://github.com/user-attachments/assets/9055a3c5-0e3a-48a9-a7e1-4e4245d25cff" />

#### Animation Montage
攻撃アニメーション等では Animation Notify State を用いて攻撃判定の有効期間を決定できるようにした。  
また、ダメージ倍率なども Editor 側に公開することで、コードを考慮せずモーションごとのバランス調整ができるようにした。
<img width="1916" height="1300" alt="スクリーンショット 2026-07-25 232730" src="https://github.com/user-attachments/assets/ec36214e-f4fc-4771-a824-78d710d5eb8c" />

