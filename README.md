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

# 代码修改
## 添加Vector数量.
目前的版本里面只定义了3个Vector参数 用来K帧， 如果要修改数量，可以模仿：

AkaPostSection.h 添加 4个FMovieSceneFloatChannel 和 1个 FMovieSceneStringChannel，

例如
```
UPROPERTY()
FMovieSceneFloatChannel Vector4R{};
UPROPERTY()
FMovieSceneFloatChannel Vector4G{};
UPROPERTY()
FMovieSceneFloatChannel Vector4B{};
UPROPERTY()
FMovieSceneFloatChannel Vector4A{};
UPROPERTY()
FMovieSceneStringChannel Vector4Name;
```

AkaPostSection.cpp 修改FPostStruct结构体， 数组容量+5，
```
#if WITH_EDITOR
struct FPostStruct
{
	FMovieSceneChannelMetaData Data[21]; // 原本是16，16+5 = 21
	FPostStruct()
	{
		Data[0].SetIdentifiers("CameraActorTag", FText::FromString("PostTag"),FText::FromString("Main"));

		Data[1].SetIdentifiers("Vector1Name",		FText::FromString("ParamName"),	FText::FromString("Vector1"));
		Data[2].SetIdentifiers("Vector1R",		FText::FromString("R"),			FText::FromString("Vector1"));
		Data[3].SetIdentifiers("Vector1G",		FText::FromString("G"),			FText::FromString("Vector1"));
		Data[4].SetIdentifiers("Vector1B",		FText::FromString("B"),			FText::FromString("Vector1"));
		Data[5].SetIdentifiers("Vector1A",		FText::FromString("A"),			FText::FromString("Vector1"));
		
		Data[6]. SetIdentifiers("Vector2Name",		FText::FromString("ParamName"),	FText::FromString("Vector2"));
		Data[7]. SetIdentifiers("Vector2R",		FText::FromString("R"),			FText::FromString("Vector2"));
		Data[8]. SetIdentifiers("Vector2G",		FText::FromString("G"),			FText::FromString("Vector2"));
		Data[9]. SetIdentifiers("Vector2B",		FText::FromString("B"),			FText::FromString("Vector2"));
		Data[10].SetIdentifiers("Vector2A",		FText::FromString("A"),			FText::FromString("Vector2"));
		
		Data[11].SetIdentifiers("Vector3Name",		FText::FromString("ParamName"),	FText::FromString("Vector3"));
		Data[12].SetIdentifiers("Vector3R",		FText::FromString("R"),		    FText::FromString("Vector3"));
		Data[13].SetIdentifiers("Vector3G",		FText::FromString("G"),		    FText::FromString("Vector3"));
		Data[14].SetIdentifiers("Vector3B",		FText::FromString("B"),		    FText::FromString("Vector3"));
		Data[15].SetIdentifiers("Vector3A",		FText::FromString("A"),		    FText::FromString("Vector3"));

///////////////新增的第4组Vector
		Data[16].SetIdentifiers("Vector4Name",		FText::FromString("ParamName"),	FText::FromString("Vector3"));
		Data[17].SetIdentifiers("Vector4R",		FText::FromString("R"),		    FText::FromString("Vector4"));
		Data[18].SetIdentifiers("Vector4G",		FText::FromString("G"),		    FText::FromString("Vector4"));
		Data[19].SetIdentifiers("Vector4B",		FText::FromString("B"),		    FText::FromString("Vector4"));
		Data[20].SetIdentifiers("Vector4A",		FText::FromString("A"),		    FText::FromString("Vector4"));


		//CameraActorTag
		Data[0].SortOrder = 0;
		
		// Vec1Name                 Vec1.R                      	Vec1.G                      Vec1.B                         Vec1.A
		Data[1].SortOrder = 1;	    Data[2].SortOrder = 2;		Data[3].SortOrder = 3;	    Data[4].SortOrder = 4;	   Data[5].SortOrder = 5;
		
		// Vec2Name                 Vec2.R                      	Vec2.G                      Vec2.B                         Vec2.A
		Data[6].SortOrder = 6;	    Data[7].SortOrder = 7;		Data[8].SortOrder = 8;	    Data[9].SortOrder = 9;	   Data[10].SortOrder = 10;
		
		// Vec3Name                 Vec3.R                      	Vec3.G                      Vec3.B                         Vec3.A
		Data[11].SortOrder = 11;    Data[12].SortOrder = 12;   	 	Data[13].SortOrder = 13;    Data[14].SortOrder = 14;	   Data[15].SortOrder = 15;

///////////////新增的第4组Vector
		// Vec4Name                 Vec4.R                      	Vec4.G                      Vec4.B                         Vec4.A
		Data[16].SortOrder = 11;    Data[17].SortOrder = 12;   	 	Data[18].SortOrder = 13;    Data[19].SortOrder = 14;	   Data[20].SortOrder = 15;
	}
};
#endif

```

在构造函数初始化第四组Vector

```
UAkaPostSection::UAkaPostSection(const FObjectInitializer& ObjInit) : Super(ObjInit)
{

//////////省略****************
	Vector4Name.SetDefault("Vector4");
	Vector4R.SetDefault(1.f);
	Vector4G.SetDefault(1.f);
	Vector4B.SetDefault(1.f);
	Vector4A.SetDefault(1.f);

}
```
在UAkaPostSection::CacheChannelProxy()函数里添加到FMovieSceneChannelProxyData

```
EMovieSceneChannelProxyType UAkaPostSection::CacheChannelProxy()
{
	FMovieSceneChannelProxyData PostChannelProxyData;

#if WITH_EDITOR
	FPostStruct EditorData;
	
	PostChannelProxyData.Add(CameraTagChannel, EditorData.Data[0],	TMovieSceneExternalValue<FString>());

/////////省略******************

	PostChannelProxyData.Add(Vector4Name,	  	  EditorData.Data[16],  TMovieSceneExternalValue<FString>());
	PostChannelProxyData.Add(Vector4R,		  EditorData.Data[17],  TMovieSceneExternalValue<float>());
	PostChannelProxyData.Add(Vector4G,		  EditorData.Data[18],  TMovieSceneExternalValue<float>());
	PostChannelProxyData.Add(Vector4B,		  EditorData.Data[19],  TMovieSceneExternalValue<float>());
	PostChannelProxyData.Add(Vector4A,		  EditorData.Data[20],  TMovieSceneExternalValue<float>());
#else
	
////////省略********************

	PostChannelProxyData.Add(Vector4Name);
	PostChannelProxyData.Add(Vector4R);
	PostChannelProxyData.Add(Vector4G);
	PostChannelProxyData.Add(Vector4B);
	PostChannelProxyData.Add(Vector4A);
#endif
	ChannelProxy = MakeShared<FMovieSceneChannelProxy>(MoveTemp(PostChannelProxyData));
	return EMovieSceneChannelProxyType::Dynamic;
}
```

在 void UAkaPostSection::Update 函数里更新第四组Vector
```

void UAkaPostSection::Update(IMovieScenePlayer* Player, const FEvaluationHookParams& Params) const
{
	if(bShouldTick == false)
	{
		//UE_LOG(LogTemp,Warning,TEXT("No PostCam Actor, No Tick"));
		return ;
	}
	
	if(DynamicMat)
	{
		const FFrameTime EvalTime = Params.Context.GetTime();

		//省略****************

		const float Vec4R =  GetEvaluateValue( Vector4R, EvalTime);
		const float Vec4G =  GetEvaluateValue( Vector4G, EvalTime);
		const float Vec4B =  GetEvaluateValue( Vector4B, EvalTime);
		const float Vec4A =  GetEvaluateValue( Vector4R, EvalTime);

		// 省略***************

		const FName Vec4 = FName(*Vector3Name.Evaluate(EvalTime));

		DynamicMat->SetVectorParameterValue(Vec1,FLinearColor(Vec1R,Vec1G,Vec1B,Vec1A));
		DynamicMat->SetVectorParameterValue(Vec2,FLinearColor(Vec2R,Vec2G,Vec2B,Vec2A));
		DynamicMat->SetVectorParameterValue(Vec3,FLinearColor(Vec3R,Vec3G,Vec3B,Vec3A));

		// 使用第四组Vector更新材质参数
		DynamicMat->SetVectorParameterValue(Vec4,FLinearColor(Vec4R,Vec4G,Vec4B,Vec4A));
	}
}
```
