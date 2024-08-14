# 插件解释

## 轨道注册
PostTrack/Private/PostTrack.cpp StartupModule() 获得ISequencerModule的模块，向他扔出Power，它接到了 就变强.

## 轨道创建
FAkaPostTrackEditor::OnMatAssetSelected 获得选中的 材质实例 之后，尝试创建一个轨道 TryCreateMatSceneTrack，

1.检测有没有已经存在的 UAkaPostTrack轨道， 

2.如果不存在 就创建一个轨道，并且把 材质实例 塞到轨道里面，

3.通知 新创建的轨道  让它再创建一个UAkaPostSection (PostTrack->AddNewMatSection)，并且把 材质实例 塞到Section里面.

3.1 什么是Section？ 就是K帧的那个轨道 (虽然说法不太严谨，但是帧就是在这里K的)

4.UAkaPostSection被创建出来了，它在构造函数里初始化一堆东西.  重点是 Begin、Update、End 3个函数，

定序器每次从头播放时，会调用轨道的Begin函数， 播放过程不断调用Update.

## 轨道运行
在UAkaPostSection的Begin函数里， 根据Actor的Tag获取摄像机/后期盒子，创建了动态材质 并且把 动态材质 给这些摄像机/后期盒子 

每一次Begin都要重新获取一遍，如果第一次播放 获取了一些Actor，第二遍播放时 世界里的Actor的Tag被修改了，那就不对了，必须重新获取一遍.
