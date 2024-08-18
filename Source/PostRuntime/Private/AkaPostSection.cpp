// Fill out your copyright notice in the Description page of Project Settings.


#include "AkaPostSection.h"

#include "Materials/MaterialInstance.h"
#include "HelperMacro.h"
#include "Channels/MovieSceneChannelProxy.h"
#include "IMovieScenePlayer.h"
#include "Camera/CameraActor.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Engine/PostProcessVolume.h"
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
		MetaDataInitial(Data[0],"CameraActorTag", "PostTag","Main",0);
		
		INIT_METADATA(1, 1);
		INIT_METADATA(6, 2);
		INIT_METADATA(11, 3);
		
		//-------------Change this to Add Vector--------------//
		// Add Vector4 ,Step 1.↓
		// INIT_METADATA(16, 4);
	}
};
#endif


UAkaPostSection::UAkaPostSection(const FObjectInitializer& ObjInit) : Super(ObjInit)
{
	bRequiresRangedHook = true;
	bRequiresTriggerHooks = true;
	EvalOptions.CompletionMode = EMovieSceneCompletionMode::RestoreState;
	MatInstance= nullptr;
	This=this;
	ActorTagChannel.SetDefault("PostCam");


	Vector1.SetName("Post1");
	Vector2.SetName("Post2");
	Vector3.SetName("Post3");
	
	//-------------Change this to Add Vector--------------//
	// Add Vector4 ,Step 2.↓
	// Vector4.SetName("Post4");
}


EMovieSceneChannelProxyType UAkaPostSection::CacheChannelProxy()
{
	FMovieSceneChannelProxyData PostChannelProxyData;

#if WITH_EDITOR
	FPostStruct EditorData;
	if(!bIsPostActor)
	{
		PostChannelProxyData.Add(ActorTagChannel, EditorData.Data[0],	TMovieSceneExternalValue<FString>());
	}
	
	ADD_VECTOR_DATA_EDITOR(Vector1,1);
	ADD_VECTOR_DATA_EDITOR(Vector2,6);
	ADD_VECTOR_DATA_EDITOR(Vector3,11);
	
	//-------------Change this to Add Vector--------------//
	// Add Vector4 ,Step 3.↓
	//ADD_VECTOR_DATA_EDITOR(Vector4, 16);
	
#else
	PostChannelProxyData.Add(ActorTagChannel);
	ADD_VECTOR_DATA(Vector1);
	ADD_VECTOR_DATA(Vector2);
	ADD_VECTOR_DATA(Vector3);
	//-------------Change this to Add Vector--------------//
	// Add Vector4 ,Step 4.↓
	//ADD_VECTOR_DATA(Vector4);
	
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
	if(bIsPostActor)
	{
		BeginWithTagPostActor(PlayerWorld,Player,Params);
	}
	else
	{
		BeginWithTagActor(PlayerWorld);
	}
}

float UAkaPostSection::GetEvaluateValue(const FMovieSceneFloatChannel& FloatChannel, const FFrameTime& Time) const
{
	float Value = 0.f;
	FloatChannel.Evaluate(Time,Value);
	return Value;
}

void UAkaPostSection::Update(IMovieScenePlayer* Player, const FEvaluationHookParams& Params) const
{
	if(bShouldTick == false)
	{
		return ;
	}
	
	if(DynamicMat)
	{
		const FFrameTime EvalTime = Params.Context.GetTime();
		SET_VECTOR_PARAMETER(Vector1,1);
		SET_VECTOR_PARAMETER(Vector2,2);
		SET_VECTOR_PARAMETER(Vector3,3);
		
		//-------------Change this to Add Vector--------------//
		// Add Vector4 ,Step 5.↓  this is the final Step!!
		//SET_VECTOR_PARAMETER(Vector4,4);
	}
}

void UAkaPostSection::End(IMovieScenePlayer* Player, const FEvaluationHookParams& Params) const
{
	
}

void UAkaPostSection::CancelMaterialLink()
{
	if(bIsPostActor)
	{
		if(PostActor != nullptr)
		{
			PostActor->Settings.WeightedBlendables.Array.Empty();
			if(MatInstance)
			{
				PostActor->Settings.WeightedBlendables.Array.Add(FWeightedBlendable(1.0,MatInstance));
			}
		}
	}
	else
	{
		for(auto Actor : ActorWithTag)
		{
			if(auto TargetCam = Cast<ACameraActor>(Actor))
			{
				TargetCam->GetCameraComponent()->PostProcessSettings.WeightedBlendables.Array.Empty();
				if(MatInstance)
				{
					PostActor->Settings.WeightedBlendables.Array.Add(FWeightedBlendable(1.0,MatInstance));
				}
			}
			else if(auto TargetVolume = Cast<APostProcessVolume>(Actor))
			{
				TargetVolume->Settings.WeightedBlendables.Array.Empty();
				if(MatInstance)
				{
					PostActor->Settings.WeightedBlendables.Array.Add(FWeightedBlendable(1.0,MatInstance));
				}
			}
		}
	}
}



UWorld* UAkaPostSection::GetWorld() const
{
	return World;
}

/*
 * 1. Get Tag Key
 * 2. Find Actor By Tag,  Is Camera / PostProcessVolume ?  if not ---> return ,Do nothing
 * 3. if Yes ---> Clear Camera/PP Material
 * 4. Create [Dynamic Material] And Set Actor's Material With [DynamicMat].
 */
void UAkaPostSection::BeginWithTagActor(UWorld* InWorld) const
{
	const auto CameraTagKey = ActorTagChannel.GetData().GetValues();
	FName CameraTag = "PostCam";
	
	if(!CameraTagKey.IsEmpty())
	{
		CameraTag = *CameraTagKey[0];
	}

	// Find Actor
	TArray<AActor*> AllActor;
	UGameplayStatics::GetAllActorsWithTag(InWorld,CameraTag,AllActor);

	if(AllActor.Num() == 0)
	{
		bShouldTick = false;
		return;
	}
	// Find Actor
	
	if(DynamicMat == nullptr)
	{
		auto MatName = MatInstance->GetName() + "_Animated";
		DynamicMat = UMaterialInstanceDynamic::Create(MatInstance, nullptr, FName(MatName));
	}

	// Clear Previous Actor's Material
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
	// Clear Previous Actor's Material

	// Set Actor's Material
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
	// Set Actor's Material
	
}

void UAkaPostSection::BeginWithTagPostActor(UWorld* InWorld,IMovieScenePlayer* Player, const FEvaluationHookParams& Params) const
{
	bShouldTick = false;
	for (TWeakObjectPtr<> BoundObject : Player->FindBoundObjects(PostActorGuid, Params.SequenceID))
	{
		if (APostProcessVolume* FindPostActor = Cast<APostProcessVolume>(BoundObject.Get()))
		{
			PostActor = FindPostActor;
		}
	}
	if(PostActor == nullptr)
	{
		//UE_LOG(LogTemp,Warning,TEXT("PostActor Not Found"));
		return;
	}


	if(DynamicMat == nullptr)
	{
		if(MatInstance)
		{
			auto MatName = MatInstance->GetName() + "_Animated";
			DynamicMat = UMaterialInstanceDynamic::Create(MatInstance, nullptr, FName(MatName));
		}
	}

	if(PostActor)
	{
		if(PostActor->Settings.WeightedBlendables.Array[0].Object != DynamicMat)
		{
			PostActor->Settings.WeightedBlendables.Array.Empty();
			PostActor->Settings.WeightedBlendables.Array.Add(FWeightedBlendable(1.0,DynamicMat));
		}
		bShouldTick = true;
	}
	else
	{
		bShouldTick = false;
	}
	
	
}

#undef LOCTEXT_NAMESPACE
