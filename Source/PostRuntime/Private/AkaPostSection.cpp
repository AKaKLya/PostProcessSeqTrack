// Fill out your copyright notice in the Description page of Project Settings.


#include "AkaPostSection.h"

#include "Channels/MovieSceneChannelProxy.h"
#include "IMovieScenePlayer.h"
#include "Camera/CameraActor.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"

#define LOCTEXT_NAMESPACE "UMGSceneSection"

#if WITH_EDITOR

struct FPostEditorData
{
public:

	//Array of channel metadat(as Metadata)
	FMovieSceneChannelMetaData Data[10];

	FPostEditorData()
	{
		Data[0].SetIdentifiers("Float0", FText::FromString("Float0"),FText::FromString("Float"));
		Data[1].SetIdentifiers("Float1", FText::FromString("Float1"),FText::FromString("Float"));
		Data[2].SetIdentifiers("Float2", FText::FromString("Float2"),FText::FromString("Float"));
		Data[3].SetIdentifiers("Float3", FText::FromString("Float3"),FText::FromString("Float"));
		Data[4].SetIdentifiers("Float4", FText::FromString("Float4"),FText::FromString("Float"));
		Data[5].SetIdentifiers("Float5", FText::FromString("Float5"),FText::FromString("Float"));
		Data[6].SetIdentifiers("Float6", FText::FromString("Float6"),FText::FromString("Float"));
		Data[7].SetIdentifiers("Float7", FText::FromString("Float7"),FText::FromString("Float"));
		Data[8].SetIdentifiers("Float8", FText::FromString("Float8"),FText::FromString("Float"));
		Data[9].SetIdentifiers("Float9", FText::FromString("Float9"),FText::FromString("Float"));
	}
	
	
};

#endif // WITH_EDITOR


UAkaPostSection::UAkaPostSection(const FObjectInitializer& ObjInit) : Super(ObjInit)
{
	bRequiresRangedHook = true;
	bRequiresTriggerHooks = true;
	EvalOptions.CompletionMode = EMovieSceneCompletionMode::RestoreState;

	//Remove const limitations.
	This = this;
	
	FloatCurve0.SetDefault(1.f);
	FloatCurve1.SetDefault(1.f);
	FloatCurve2.SetDefault(1.f);
	FloatCurve3.SetDefault(1.f);
	FloatCurve4.SetDefault(1.f);
	FloatCurve5.SetDefault(1.f);
	FloatCurve6.SetDefault(1.f);
	FloatCurve7.SetDefault(1.f);
	FloatCurve8.SetDefault(1.f);
	FloatCurve9.SetDefault(1.f);
}


EMovieSceneChannelProxyType UAkaPostSection::CacheChannelProxy()
{
	FMovieSceneChannelProxyData Channels;

#if WITH_EDITOR
	
	FPostEditorData EditorData;
	Channels.Add(FloatCurve0, EditorData.Data[0], TMovieSceneExternalValue<float>());
	Channels.Add(FloatCurve1, EditorData.Data[1], TMovieSceneExternalValue<float>());
	Channels.Add(FloatCurve2, EditorData.Data[2], TMovieSceneExternalValue<float>());
	Channels.Add(FloatCurve3, EditorData.Data[3], TMovieSceneExternalValue<float>());
	Channels.Add(FloatCurve4, EditorData.Data[4], TMovieSceneExternalValue<float>());
	Channels.Add(FloatCurve5, EditorData.Data[5], TMovieSceneExternalValue<float>());
	Channels.Add(FloatCurve6, EditorData.Data[6], TMovieSceneExternalValue<float>());
	Channels.Add(FloatCurve7, EditorData.Data[7], TMovieSceneExternalValue<float>());
	Channels.Add(FloatCurve8, EditorData.Data[8], TMovieSceneExternalValue<float>());
	Channels.Add(FloatCurve9, EditorData.Data[9], TMovieSceneExternalValue<float>());


#else

	Channels.Add(FloatCurve0);
	Channels.Add(FloatCurve1);
	Channels.Add(FloatCurve2);
	Channels.Add(FloatCurve3);
	Channels.Add(FloatCurve4);
	Channels.Add(FloatCurve5);
	Channels.Add(FloatCurve6);
	Channels.Add(FloatCurve7);
	Channels.Add(FloatCurve8);
	Channels.Add(FloatCurve9);


#endif
	ChannelProxy = MakeShared<FMovieSceneChannelProxy>(MoveTemp(Channels));

	return EMovieSceneChannelProxyType::Dynamic;
}


void UAkaPostSection::Begin(IMovieScenePlayer* Player, const UE::MovieScene::FEvaluationHookParams& Params) const
{


	UWorld* ContextRenderWorld= Player->GetPlaybackContext()->GetWorld();

	if(ContextRenderWorld->WorldType==EWorldType::PIE|| ContextRenderWorld->WorldType == EWorldType::Game)
		This->RenderingWorld = ContextRenderWorld;
	else
		This->RenderingWorld = nullptr;


	UMaterialInstanceDynamic* Mat = UMaterialInstanceDynamic::Create(MatInstance, This, FName("PostMaterial"));
	TArray<AActor*> AllActor;
	UGameplayStatics::GetAllActorsWithTag(ContextRenderWorld,"PostCam",AllActor);
	for(auto Actor : AllActor)
	{
		//UE_LOG(LogTemp,Warning,TEXT("Found PostCam, %s"),*Actor->GetName());
		auto TargetCam = Cast<ACameraActor>(Actor)->GetCameraComponent();
		TargetCam->PostProcessSettings.WeightedBlendables.Array.Empty();
		TargetCam->PostProcessSettings.WeightedBlendables.Array.Add(FWeightedBlendable(1.0,Mat));
	}
}

void UAkaPostSection::Update(IMovieScenePlayer* Player, const UE::MovieScene::FEvaluationHookParams& Params) const
{
	UWorld* ContextRenderWorld= Player->GetPlaybackContext()->GetWorld();
	TArray<AActor*> AllActor;
	UGameplayStatics::GetAllActorsWithTag(ContextRenderWorld,"PostCam",AllActor);
	
	auto EvalTime = Params.Context.GetTime();
	
	float Float0 = 0.f;
	FloatCurve0.Evaluate(EvalTime, Float0);

	float Float1 = 0.f;
	FloatCurve1.Evaluate(EvalTime, Float1);

	float Float2 = 0.f;
	FloatCurve2.Evaluate(EvalTime, Float2);
		
	float Float3 = 0.f;
	FloatCurve3.Evaluate(EvalTime, Float3);

	float Float4 = 0.f;
	FloatCurve4.Evaluate(EvalTime, Float4);
		
	float Float5 = 0.f;
	FloatCurve5.Evaluate(EvalTime, Float5);

	float Float6 = 0.f;
	FloatCurve6.Evaluate(EvalTime, Float6);
		
	float Float7 = 0.f;
	FloatCurve7.Evaluate(EvalTime, Float7);

	float Float8 = 0.f;
	FloatCurve8.Evaluate(EvalTime, Float8);

	float Float9 = 0.f;
	FloatCurve9.Evaluate(EvalTime, Float9);

	for(auto Actor : AllActor)
	{
		auto TargetCam = Cast<ACameraActor>(Actor)->GetCameraComponent();
		auto DynamicMat = Cast<UMaterialInstanceDynamic>(TargetCam->PostProcessSettings.WeightedBlendables.Array[0].Object);
		DynamicMat->SetScalarParameterValue("Float0", Float0);
		DynamicMat->SetScalarParameterValue("Float1", Float1);
		DynamicMat->SetScalarParameterValue("Float2", Float2);
		DynamicMat->SetScalarParameterValue("Float3", Float3);
		DynamicMat->SetScalarParameterValue("Float4", Float4);
		DynamicMat->SetScalarParameterValue("Float5", Float5);
		DynamicMat->SetScalarParameterValue("Float6", Float6);
		DynamicMat->SetScalarParameterValue("Float7", Float7);
		DynamicMat->SetScalarParameterValue("Float8", Float8);
		DynamicMat->SetScalarParameterValue("Float9", Float9);
		//TargetCam->PostProcessSettings.WeightedBlendables.Array.Add(FWeightedBlendable(1.0,Mat));
	}
	
	auto PlaybackStatus = Player->GetPlaybackStatus();
}

void UAkaPostSection::End(IMovieScenePlayer* Player, const UE::MovieScene::FEvaluationHookParams& Params) const
{
	
}

UWorld* UAkaPostSection::GetWorld() const
{
	return RenderingWorld;
}

double UAkaPostSection::GetPlayerTimeAsSeconds(const UE::MovieScene::FEvaluationHookParams& Params) const
{
	FFrameTime CurrentFrameTime = Params.Context.GetTime();
	FFrameRate CurrentFrameRate = Params.Context.GetFrameRate();
	double CurrentTimeAsSeconds = CurrentFrameTime / CurrentFrameRate;
	return CurrentTimeAsSeconds;
}

double UAkaPostSection::GetSectionLocalTime(const UE::MovieScene::FEvaluationHookParams& Params) const
{
	FFrameNumber SectionStartFrameNumber = GetInclusiveStartFrame();
	FFrameTime SectionStartFrameTime(SectionStartFrameNumber);
	FFrameRate CurrentFrameRate = Params.Context.GetFrameRate();
	double SectionStartTimeAsSeconds = SectionStartFrameTime / CurrentFrameRate;
	double PlayerTimeAsSeconds = GetPlayerTimeAsSeconds(Params);
	double SectionLocalCurrentTime = PlayerTimeAsSeconds - SectionStartTimeAsSeconds;
	return SectionLocalCurrentTime;
	
}


#undef LOCTEXT_NAMESPACE
