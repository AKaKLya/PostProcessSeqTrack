# 插件解释

## 轨道注册
PostTrack/Private/PostTrack.cpp StartupModule() 获得ISequencerModule的模块，向他扔出Power，它接到了 就变强.

## 轨道创建
FAkaPostTrackEditor::OnMatAssetSelected 获得选中的 材质实例 之后，尝试创建一个轨道 TryCreateMatSceneTrack，

1.检测有没有已经存在的 UAkaPostTrack轨道， 

2.如果不存在 就创建一个轨道，并且把 材质实例 塞到轨道里面， 如果存在一个轨道了，那就不创建

3.通知 新创建的轨道 (或 已经存在的轨道) 让它再创建一个UAkaPostSection (PostTrack->AddNewMatSection)，并且把 材质实例 塞到Section里面.

3.1 什么是Section？ 就是K帧的那个轨道 (虽然说法不太严谨，但是帧就是在这里K的)

4.UAkaPostSection被创建出来了，它在构造函数里初始化一堆东西.  重点是 Begin、Update、End 3个函数，

定序器每次从头播放时，会调用轨道的Begin函数， 播放过程不断调用Update.

## 轨道运行
在UAkaPostSection的Begin函数里， 根据Actor的Tag获取摄像机/后期盒子，创建了动态材质 并且把 动态材质 给这些摄像机/后期盒子 

每一次Begin都要重新获取一遍，如果第一次播放 获取了一些Actor，第二遍播放时 世界里的Actor的Tag被修改了，那就不对了，必须重新获取一遍.

Update函数 看一眼就明白了

## 动态材质
上面提到了动态材质， 在UAkaPostSection里面， 动态材质的变量 是用UPROPERTY(Transient)修饰的，因为它是动态创建出来的 没有实体，每次创建的结果都会不一样，所以这个变量不能被保存下来.

如果不用Transient，定序器会因为这个变量 而不能保存，K帧半天 存不下来，GG.  材质实例的变量是可以保存的， 因为材质实例是有实体的， 它就在硬盘里放着.

## 轨道销毁
情况一:
删除主轨道下面的子轨道时， UAkaPostTrack::RemoveSection 和 RemoveSectionAt 函数会被执行，在这俩函数里 调用 要删除的轨道 的CancelMaterialLink函数.

UAkaPostSection::CancelMaterialLink 遍历根据Tag获取到的 摄像机/后期盒子 们，把它们的动态材质给清除掉. Section赋予的材质 由 Section来清除.

情况二:
在定序器里删除整个主轨道时，定序器会触发一个Event，并且Event会通知是哪个轨道被销毁了， 所以需要在 UAkaPostTrack 创建时， Link这一个Event，

在《轨道创建》那一段里面提到了 轨道是在 FAkaPostTrackEditor::TryCreateMatSceneTrack 函数里创建的，轨道创建后 Link到定序器的事件里，
```
UAkaPostTrack* FAkaPostTrackEditor::TryCreateMatSceneTrack(UMovieScene* InMovieScene, UMaterialInstance* InMatInstance)

if(!PostTrack)
	{
		FText SessionName = FText::FromString("Create Post Master Track");
		const FScopedTransaction ScopedTransaction(SessionName);
		InMovieScene->Modify();

		PostTrack = InMovieScene->AddTrack<UAkaPostTrack>();
		check(PostTrack);
		PostTrack->SetDisplayName(FText::FromString("PostProcess"));
		InMovieScene->EventHandlers.Link(PostTrack); // ←←←←在这里Link
	}
```
为了实现主轨道的Link，需要让主轨道继承自 UE::MovieScene::TIntrusiveEventHandler<UE::MovieScene::ISequenceDataEventHandler>
```
class POSTRUNTIME_API UAkaPostTrack : public UMovieSceneNameableTrack
	,public	UE::MovieScene::TIntrusiveEventHandler<UE::MovieScene::ISequenceDataEventHandler>
```
主轨道只会创建一个，第二次创建时 只让轨道创建一个Section.

那就是说 始终只有一个主轨道存在，其余的子轨道都是Section， 当定序器删除主轨道时， 只要让主轨道接收这个Event就行了， 让主轨道在被删除时 删除它的所有子轨道.

继承以后，要实现一个虚函数，遍历所有的子轨道，通知它们CancelMaterialLink，在销毁之前 清除所有被修改的 摄像机/后期盒子 的材质.
```
virtual void OnTrackRemoved(UMovieSceneTrack*) override;
void UAkaPostTrack::OnTrackRemoved(UMovieSceneTrack* MovieSceneTrack)
{
	for(auto Section : AkaPostSections)
	{
		if(auto PostSection = Cast<UAkaPostSection>(Section))
		{
			PostSection->CancelMaterialLink();
		}
	}
	TIntrusiveEventHandler<ISequenceDataEventHandler>::OnTrackRemoved(MovieSceneTrack);
}
```
