// Fill out your copyright notice in the Description page of Project Settings.


#include "AkaPostSection.h"

#include "Channels/MovieSceneChannelProxy.h"
#include "IMovieScenePlayer.h"
#include "Camera/CameraActor.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Engine/PostProcessVolume.h"
#include "Channels/MovieSceneChannelEditorData.h"
#include "Channels/MovieSceneStringChannel.h"
#include "Channels/MovieSceneFloatChannel.h"
#include "Channels/MovieSceneChannelData.h"

#define LOCTEXT_NAMESPACE "PostSection"

using namespace UE::MovieScene;

#if WITH_EDITOR
struct FPostStruct
{
	FMovieSceneChannelMetaData Data[16];
	FPostStruct()
	{
		Data[0].SetIdentifiers("CameraActorTag", FText::FromString("PostTag"),FText::FromString("Main"));

		Data[1].SetIdentifiers("Vector1Name",	FText::FromString("ParamName"),	FText::FromString("Vector1"));
		Data[2].SetIdentifiers("Vector1R",		FText::FromString("R"),			FText::FromString("Vector1"));
		Data[3].SetIdentifiers("Vector1G",		FText::FromString("G"),			FText::FromString("Vector1"));
		Data[4].SetIdentifiers("Vector1B",		FText::FromString("B"),			FText::FromString("Vector1"));
		Data[5].SetIdentifiers("Vector1A",		FText::FromString("A"),			FText::FromString("Vector1"));
		
		Data[6]. SetIdentifiers("Vector2Name",	FText::FromString("ParamName"),	FText::FromString("Vector2"));
		Data[7]. SetIdentifiers("Vector2R",		FText::FromString("R"),			FText::FromString("Vector2"));
		Data[8]. SetIdentifiers("Vector2G",		FText::FromString("G"),			FText::FromString("Vector2"));
		Data[9]. SetIdentifiers("Vector2B",		FText::FromString("B"),			FText::FromString("Vector2"));
		Data[10].SetIdentifiers("Vector2A",		FText::FromString("A"),			FText::FromString("Vector2"));
		
		Data[11].SetIdentifiers("Vector3Name",	FText::FromString("ParamName"),	FText::FromString("Vector3"));
		Data[12].SetIdentifiers("Vector3R",		FText::FromString("R"),		    FText::FromString("Vector3"));
		Data[13].SetIdentifiers("Vector3G",		FText::FromString("G"),		    FText::FromString("Vector3"));
		Data[14].SetIdentifiers("Vector3B",		FText::FromString("B"),		    FText::FromString("Vector3"));
		Data[15].SetIdentifiers("Vector3A",		FText::FromString("A"),		    FText::FromString("Vector3"));


		//CameraActorTag
		Data[0].SortOrder = 0;
		
		// Vec1Name                 Vec1.R                      Vec1.G                      Vec1.B                      Vec1.A
		Data[1].SortOrder = 1;	    Data[2].SortOrder = 2;		Data[3].SortOrder = 3;		Data[4].SortOrder = 4;	    Data[5].SortOrder = 5;
		
		// Vec2Name                 Vec2.R                      Vec2.G                      Vec2.B                      Vec2.A
		Data[6].SortOrder = 6;		Data[7].SortOrder = 7;		Data[8].SortOrder = 8;	    Data[9].SortOrder = 9;	    Data[10].SortOrder = 10;
		
		// Vec3Name                 Vec3.R                      Vec3.G                      Vec3.B                      Vec3.A
		Data[11].SortOrder = 11;	Data[12].SortOrder = 12;    Data[13].SortOrder = 13;    Data[14].SortOrder = 14;	Data[15].SortOrder = 15;
		
	}
};
#endif

UAkaPostSection::UAkaPostSection(const FObjectInitializer& ObjInit) : Super(ObjInit)
{
	bRequiresRangedHook = true;
	bRequiresTriggerHooks = true;
	EvalOptions.CompletionMode = EMovieSceneCompletionMode::RestoreState;
	MatInstance= nullptr;

	CameraTagChannel.SetDefault("PostCam");

	Vector1Name.SetDefault("Vector1");
	Vector1R.SetDefault(1.f);
	Vector1G.SetDefault(1.f);
	Vector1B.SetDefault(1.f);
	Vector1A.SetDefault(1.f);

	Vector2Name.SetDefault("Vector2");
	Vector2R.SetDefault(1.f);
	Vector2G.SetDefault(1.f);
	Vector2B.SetDefault(1.f);
	Vector2A.SetDefault(1.f);

	Vector3Name.SetDefault("Vector3");
	Vector3R.SetDefault(1.f);
	Vector3G.SetDefault(1.f);
	Vector3B.SetDefault(1.f);
	Vector3A.SetDefault(1.f);
}




EMovieSceneChannelProxyType UAkaPostSection::CacheChannelProxy()
{
	FMovieSceneChannelProxyData PostChannelProxyData;

#if WITH_EDITOR
	FPostStruct EditorData;
	
	PostChannelProxyData.Add(CameraTagChannel, EditorData.Data[0],	TMovieSceneExternalValue<FString>());

	PostChannelProxyData.Add(Vector1Name,	  EditorData.Data[1],	TMovieSceneExternalValue<FString>());
	PostChannelProxyData.Add(Vector1R,		  EditorData.Data[2],	TMovieSceneExternalValue<float>());
	PostChannelProxyData.Add(Vector1G,		  EditorData.Data[3],	TMovieSceneExternalValue<float>());
	PostChannelProxyData.Add(Vector1B,		  EditorData.Data[4],	TMovieSceneExternalValue<float>());
	PostChannelProxyData.Add(Vector1A,		  EditorData.Data[5],	TMovieSceneExternalValue<float>());

	PostChannelProxyData.Add(Vector2Name,	  EditorData.Data[6],	TMovieSceneExternalValue<FString>());
	PostChannelProxyData.Add(Vector2R,		  EditorData.Data[7],   TMovieSceneExternalValue<float>());
	PostChannelProxyData.Add(Vector2G,		  EditorData.Data[8],   TMovieSceneExternalValue<float>());
	PostChannelProxyData.Add(Vector2B,		  EditorData.Data[9],   TMovieSceneExternalValue<float>());
	PostChannelProxyData.Add(Vector2A,		  EditorData.Data[10],  TMovieSceneExternalValue<float>());
	
	PostChannelProxyData.Add(Vector3Name,	  EditorData.Data[11],  TMovieSceneExternalValue<FString>());
	PostChannelProxyData.Add(Vector3R,		  EditorData.Data[12],  TMovieSceneExternalValue<float>());
	PostChannelProxyData.Add(Vector3G,		  EditorData.Data[13],  TMovieSceneExternalValue<float>());
	PostChannelProxyData.Add(Vector3B,		  EditorData.Data[14],  TMovieSceneExternalValue<float>());
	PostChannelProxyData.Add(Vector3A,		  EditorData.Data[15],  TMovieSceneExternalValue<float>());
#else
	PostChannelProxyData.Add(CameraTagChannel);

	PostChannelProxyData.Add(Vector1Name);
	PostChannelProxyData.Add(Vector1R);
	PostChannelProxyData.Add(Vector1G);
	PostChannelProxyData.Add(Vector1B);
	PostChannelProxyData.Add(Vector1A);

	PostChannelProxyData.Add(Vector2Name);
	PostChannelProxyData.Add(Vector2R);
	PostChannelProxyData.Add(Vector2G);
	PostChannelProxyData.Add(Vector2B);
	PostChannelProxyData.Add(Vector2A);
	
	PostChannelProxyData.Add(Vector3Name);
	PostChannelProxyData.Add(Vector3R);
	PostChannelProxyData.Add(Vector3G);
	PostChannelProxyData.Add(Vector3B);
	PostChannelProxyData.Add(Vector3A);
#endif
	ChannelProxy = MakeShared<FMovieSceneChannelProxy>(MoveTemp(PostChannelProxyData));
	return EMovieSceneChannelProxyType::Dynamic;
}


void UAkaPostSection::Begin(IMovieScenePlayer* Player, const FEvaluationHookParams& Params) const
{
	const auto PlayerWorld= Player->GetPlaybackContext()->GetWorld();
	if(PlayerWorld->WorldType==EWorldType::PIE || PlayerWorld->WorldType == EWorldType::Game)
	{
		World = PlayerWorld;
	}
	else
	{
		World = nullptr;
	}

	const auto CameraTagKey = CameraTagChannel.GetData().GetValues();
	FName CameraTag = "PostCam";
	
	if(!CameraTagKey.IsEmpty())
	{
		CameraTag = *CameraTagKey[0];
	}
	
	TArray<AActor*> AllActor;
	UGameplayStatics::GetAllActorsWithTag(PlayerWorld,CameraTag,AllActor);

	if(AllActor.Num() == 0)
	{
		bShouldTick = false;
		//UE_LOG(LogTemp,Warning,TEXT("No PostCam Actor"));
		return;
	}
	
	if(DynamicMat == nullptr)
	{
		DynamicMat = UMaterialInstanceDynamic::Create(MatInstance, nullptr, FName("PostMaterial"));
	}

	for(auto TagActor : ActorWithTag)
	{
		if(auto TargetCam = Cast<ACameraActor>(TagActor))
		{
			TargetCam->GetCameraComponent()->PostProcessSettings.WeightedBlendables.Array.Empty();
		}
		else if(auto TargetVolume = Cast<APostProcessVolume>(TagActor))
		{
			TargetVolume->Settings.WeightedBlendables.Array.Empty();
		}
	}
	ActorWithTag.Empty();
	
	for(const auto Actor : AllActor)
	{
		if(auto TargetCam = Cast<ACameraActor>(Actor))
		{
			TargetCam->GetCameraComponent()->PostProcessSettings.WeightedBlendables.Array.Empty();
			TargetCam->GetCameraComponent()->PostProcessSettings.WeightedBlendables.Array.Add(FWeightedBlendable(1.0,DynamicMat));
			bShouldTick = true;
			ActorWithTag.Add(TargetCam);
		}
		else if(auto TargetVolume = Cast<APostProcessVolume>(Actor))
		{
			TargetVolume->Settings.WeightedBlendables.Array.Empty();
			TargetVolume->Settings.WeightedBlendables.Array.Add(FWeightedBlendable(1.0,DynamicMat));
			bShouldTick = true;
			ActorWithTag.Add(TargetVolume);
		}
	}
	
}

inline float GetEvaluateValue(const FMovieSceneFloatChannel& FloatChannel, const FFrameTime& Time)
{
	float Value = 0.f;
	FloatChannel.Evaluate(Time,Value);
	return Value;
}

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

		const float Vec1R =  GetEvaluateValue( Vector1R, EvalTime);
		const float Vec1G =  GetEvaluateValue( Vector1G, EvalTime);
		const float Vec1B =  GetEvaluateValue( Vector1B, EvalTime);
		const float Vec1A =  GetEvaluateValue( Vector1A, EvalTime);

		const float Vec2R =  GetEvaluateValue( Vector2R, EvalTime);
		const float Vec2G =  GetEvaluateValue( Vector2G, EvalTime);
		const float Vec2B =  GetEvaluateValue( Vector2B, EvalTime);
		const float Vec2A =  GetEvaluateValue( Vector2A, EvalTime);

		const float Vec3R =  GetEvaluateValue( Vector3R, EvalTime);
		const float Vec3G =  GetEvaluateValue( Vector3G, EvalTime);
		const float Vec3B =  GetEvaluateValue( Vector3B, EvalTime);
		const float Vec3A =  GetEvaluateValue( Vector3R, EvalTime);

		const FName Vec1 = FName(*Vector1Name.Evaluate(EvalTime));
		const FName Vec2 = FName(*Vector2Name.Evaluate(EvalTime));
		const FName Vec3 = FName(*Vector3Name.Evaluate(EvalTime));
		
		DynamicMat->SetVectorParameterValue(Vec1,FLinearColor(Vec1R,Vec1G,Vec1B,Vec1A));
		DynamicMat->SetVectorParameterValue(Vec2,FLinearColor(Vec2R,Vec2G,Vec2B,Vec2A));
		DynamicMat->SetVectorParameterValue(Vec3,FLinearColor(Vec3R,Vec3G,Vec3B,Vec3A));
	}
}

void UAkaPostSection::End(IMovieScenePlayer* Player, const FEvaluationHookParams& Params) const
{
	
}

void UAkaPostSection::CancelMaterialLink()
{
	if(bIsCancelLink)
	{
		return;
	}
	for(auto Actor : ActorWithTag)
	{
		if(auto TargetCam = Cast<ACameraActor>(Actor))
		{
			TargetCam->GetCameraComponent()->PostProcessSettings.WeightedBlendables.Array.Empty();
			bShouldTick = false;
		}
		else if(auto TargetVolume = Cast<APostProcessVolume>(Actor))
		{
			TargetVolume->Settings.WeightedBlendables.Array.Empty();
			bShouldTick = false;
		}
	}
	bIsCancelLink = true;
}

UWorld* UAkaPostSection::GetWorld() const
{
	return World;
}


#undef LOCTEXT_NAMESPACE
